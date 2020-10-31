/* FK_InputBuffer is a class which handles input and input reading, translating them into game actions. 
The input is read continuously and stored in a buffer, then pattern-matched to every possible move in 
order to get the best possible combination. This file contains the implementation of FK_InputBuffer.h
*/
#include "FK_InputBuffer.h"
#include "FK_Character.h"
#include<algorithm>
#include<iostream>

using namespace irr;

namespace fk_engine{

	FK_InputBuffer::FK_InputBuffer(int newPlayerIndex,
		const std::map<FK_Input_Buttons, u32> &buttonMap,
		int spanTimeMs, int mergeTimeMs, int holdTimeMs,
		int nullTimeMs,	int maxInputNumber){

		inputBuffer.clear();
		rawInputToButtons = buttonMap;
		historyCounterMs = 0;
		historyDurationMs = 1000;
		timeSinceRepeatMs = 500;
		timeForRepeatCounterMs = 0;
		lastRecordedTimeMs = 0;
		lastInputTimeMs = 0;
		inputSpanTimeMs = spanTimeMs;
		inputMergeTimeMs = mergeTimeMs;
		inputHoldTimeMs = holdTimeMs;
		inputNullTimeMs = nullTimeMs;
		playerIndex = newPlayerIndex;
		bufferCurrentSize = 0;
		bufferCapacity = maxInputNumber;
		lastDirection = 0;
		lastPressedButtons = 0;
		lastButtonMap = 0;
		allPressedButtons = 0;
		modified = false;
		readable = false;
		maxHistoryDepth = 20;
		previousState = new bool[KEY_KEY_CODES_COUNT + 1];
		for (int i = 0; i < KEY_KEY_CODES_COUNT + 1; i++){
			previousState[i] = false;
		}
		holdingTimeForButton.clear();
		for (std::map<FK_Input_Buttons, u32>::iterator iter = rawInputToButtons.begin(); iter != rawInputToButtons.end(); ++iter)
		{
			FK_Input_Buttons currentButton = iter->first;
			if (holdingTimeForButton.count(currentButton) == 0) {
				holdingTimeForButton[currentButton] = 0;
			}
		}
	}
	FK_InputBuffer::~FK_InputBuffer(){
		delete [] previousState;
	}

	void FK_InputBuffer::setLastInputTime(int newTime){
		lastInputTimeMs = newTime;
	};

	int FK_InputBuffer::getLastInputTime(){
		return lastInputTimeMs;
	}

	int FK_InputBuffer::getPlayerId(){
		return playerIndex;
	}

	u32 FK_InputBuffer::getBufferElement(int elementIndex){
		if (elementIndex >= bufferCurrentSize || elementIndex < 0){
			return -1;
		}
		return inputBuffer[elementIndex];
	}

	u32 FK_InputBuffer::getBufferElementFromHistory(int elementIndex){
		if (elementIndex >= historyDepth || elementIndex < 0){
			return -1;
		}
		return inputBufferHistory[elementIndex];
	}

	/* extract directions from input mask using bitwise operations*/
	u32 FK_InputBuffer::getNewDirection(u32 buttonMap){
		u32 direction = 0;
		/*First, browse the vertical direction*/
		if (buttonMap & FK_Input_Buttons::Up_Direction){
			direction |= FK_Input_Buttons::Up_Direction;
		}
		else if (buttonMap & FK_Input_Buttons::Down_Direction){
			direction |= FK_Input_Buttons::Down_Direction;
		}
		/*then, the horizontal one*/
		if (buttonMap & FK_Input_Buttons::Left_Direction){
			direction |= FK_Input_Buttons::Left_Direction;
		}
		else if (buttonMap & FK_Input_Buttons::Right_Direction){
			direction |= FK_Input_Buttons::Right_Direction;
		}
		/* return the resulting bitmask */
		return direction;
	}

	std::deque<u32> FK_InputBuffer::getBuffer(){
		return inputBuffer;
	};

	std::deque<u32> FK_InputBuffer::getHistoryBuffer(){
		return inputBufferHistory;
	};

	u32 FK_InputBuffer::getLastDirection(){
		return lastDirection;
	}

	bool FK_InputBuffer::isModified(){
		return readable;
	};

	void FK_InputBuffer::resetModified(){
		modified = false;
		readable = false;
	};

	u32 FK_InputBuffer::getPressedButtons(){
		return allPressedButtons;
	}

	void FK_InputBuffer::setPressedButtons(u32 newAllButtonsPressed)
	{
		allPressedButtons = newAllButtonsPressed;
	}

	void FK_InputBuffer::setLastDirection(u32 newDirection)
	{
		lastDirection = newDirection;
	}

	void FK_InputBuffer::update(int totalTimeMs, bool* keyArray, bool updateBuffer){
		/* get time difference from last input received*/
		u32 delta_t_ms = totalTimeMs - lastRecordedTimeMs;
		int timeSinceLast = totalTimeMs - lastInputTimeMs;
		historyCounterMs += delta_t_ms;
		//u32 fixedDeltaDelayMs = 100;
		lastRecordedTimeMs = totalTimeMs;
		bool updateHistory = true;
		/* if too much time has passed, clear the buffer*/
		if (timeSinceLast > inputSpanTimeMs)
		{
			inputBuffer.clear();
			bufferCurrentSize = 0;
		}
		if (timeSinceLast > historyDurationMs){
			historyDepth = 0;
			inputBufferHistory.clear();
		}
		/* re-initialize the pressed buttons value (used by the player for one update) */
		allPressedButtons = 0;
		/* initialize bitmasks */
		u32 buttonMap = 0;
		u32 pressedButtons = 0;
		u32 oldBufferSize = bufferCurrentSize;
		bool addTriggerToInput = false;
		u32 numberOfNonDirectionButtons = 0;
		for (std::map<FK_Input_Buttons, u32>::iterator iter = rawInputToButtons.begin(); iter != rawInputToButtons.end(); ++iter)
		{
			FK_Input_Buttons currentButton = iter->first;
			u32 keyCode = iter->second;
			/* check first for non-direction buttons*/
			if (keyArray[keyCode] == true){
				allPressedButtons |= currentButton;
			}
			if (previousState[keyCode] == false &&
				keyArray[keyCode] == true){
				if (currentButton & FK_Input_Buttons::Any_NonDirectionButton){
					pressedButtons |= currentButton;
					if (currentButton != FK_Input_Buttons::Trigger_Button) {
						numberOfNonDirectionButtons += 1;
					}
				}
				/* store the full input for further analysis*/
				buttonMap |= currentButton;
			}

			if (keyArray[keyCode] == true && currentButton == FK_Input_Buttons::Trigger_Button) {
				addTriggerToInput = true;
			}

			/* in case a direction is pressed, store it anyway even if it was still pressed before*/
			if (keyArray[keyCode] == true && (currentButton & FK_Input_Buttons::Any_Direction)){
				buttonMap |= currentButton;
			}
			/* store currently pressed keys for the following cycles*/
			previousState[keyCode] = keyArray[keyCode];
			/* if button is kept pressed, add time to holding time*/
			if (keyArray[keyCode]) {
				holdingTimeForButton[currentButton] += delta_t_ms;
			}else{
				holdingTimeForButton[currentButton] = -1;
			}
		}
		bool sameInputFlag = false;
		bool sameDirectionFlag = false;
		if (lastButtonMap == allPressedButtons){
			sameInputFlag = true;
			if (!updateBuffer){
				//timeForRepeatCounterMs += delta_t_ms;
				if ((allPressedButtons & FK_Input_Buttons::Any_Direction_Plus_Held) != 0 ||
					allPressedButtons != 0){
					//if (timeForRepeatCounterMs < timeSinceRepeatMs){
					lastInputTimeMs = totalTimeMs;
					//}
					/*else{
						timeForRepeatCounterMs = timeSinceRepeatMs - fixedDeltaDelayMs;
					}*/
				}
				else{
					timeForRepeatCounterMs = 0;
				}
			}
			//updateBuffer = false;
		}
		else{
			timeForRepeatCounterMs = 0;
		}

		if (numberOfNonDirectionButtons > 0 && addTriggerToInput) {
			pressedButtons |= FK_Input_Buttons::Trigger_Button;
		}

		lastButtonMap = allPressedButtons;
		// It is very hard to press two buttons on exactly the same frame.
		// If they are close enough, consider them pressed at the same time.
		bool mergeInput = (bufferCurrentSize > 0 && timeSinceLast < inputMergeTimeMs);
		// now, check for direction and compare it to the last stored
		u32 newDirection = getNewDirection(buttonMap);
		pressedButtons |= newDirection;
		if (lastDirection != newDirection)
		{
			lastDirection = newDirection;
			// combine the direction with the buttons.
			// pressedButtons |= newDirection;
			if (lastDirection != 0){
				if (updateBuffer){
					inputBuffer.push_back(newDirection);
					bufferCurrentSize += 1;
					inputBufferHistory.push_back(newDirection);
					historyDepth += 1;
				}
				lastInputTimeMs = totalTimeMs;
			}
			// Don't merge two different directions. This avoids having impossible
			// directions such as Left+Up+Right. This also has the side effect that
			// the direction needs to be pressed at the same time or slightly before
			// the buttons for merging to work.
			mergeInput = false;			
		}
		// store NULL button
		else if (newDirection == FK_Input_Buttons::None && timeSinceLast > inputNullTimeMs && bufferCurrentSize > 0){
			if (updateBuffer){
				if (inputBuffer[bufferCurrentSize - 1] != newDirection){
					inputBuffer.push_back(newDirection);
					bufferCurrentSize += 1;
					lastInputTimeMs = totalTimeMs;
					inputBufferHistory.push_back(newDirection);
					historyDepth += 1;
				}
			}
		}
		// store button as held (only direction can be held, and only one at any time)
		/*else if (sameInputFlag && newDirection != FK_Input_Buttons::None && bufferCurrentSize > 0 && timeSinceLast > inputHoldTimeMs){
			if (updateBuffer){
				if (inputBuffer[bufferCurrentSize - 1] == newDirection){
					inputBuffer[bufferCurrentSize - 1] = newDirection << FK_Input_Buttons::TaptoHoldButtonShift;
					mergeInput = false;
					lastInputTimeMs = totalTimeMs;
				}
			}
		}*/
		else if (newDirection != FK_Input_Buttons::None && bufferCurrentSize > 0) {
			if (updateBuffer) {
				if (inputBuffer[bufferCurrentSize - 1] == newDirection) {
					bool buttonHeldSuccesfully = false;
					switch (newDirection) {
					case FK_Input_Buttons::UpRight_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Up_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Right_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::UpLeft_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Up_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Left_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::DownRight_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Down_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Right_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::DownLeft_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Down_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Left_Direction] >= inputHoldTimeMs;
						break;
					default:
						buttonHeldSuccesfully = holdingTimeForButton[newDirection] >= inputHoldTimeMs;
						break;
					}
					if (buttonHeldSuccesfully) {
						inputBuffer[bufferCurrentSize - 1] = newDirection << FK_Input_Buttons::TaptoHoldButtonShift;
						mergeInput = false;
						lastInputTimeMs = totalTimeMs;
						if (historyDepth > 0) {
							inputBufferHistory[historyDepth - 1] = inputBuffer[bufferCurrentSize - 1];
						}
					}
				}
			}
		}
		else if (bufferCurrentSize == 0 && lastDirection != FK_Input_Buttons::None && 
			newDirection == lastDirection){
			if (updateBuffer){
				inputBuffer.push_back(lastDirection << FK_Input_Buttons::TaptoHoldButtonShift);
				updateHistory = false;
				bufferCurrentSize = 1;
				mergeInput = false;
				lastInputTimeMs = totalTimeMs;
			}
		}
		/* if at least one button has been pressed, update buffer */
		u32 inputToAddToBuffer = pressedButtons /*& ~fk_constants::FK_TriggerButton*/;
		pressedButtons &= FK_Input_Buttons::Any_NonDirectionButton;
		u32 bitmask = ~((u32)FK_Input_Buttons::Any_SystemButton | (u32)FK_Input_Buttons::Any_MenuButton);
		pressedButtons &= bitmask;
		/* exclude trigger button */
		u32 inputButtons = pressedButtons /*& ~fk_constants::FK_TriggerButton*/;
		if (inputButtons != 0){
			if (updateBuffer){
				if (mergeInput){
					if (!(inputBuffer[bufferCurrentSize - 1] & FK_Input_Buttons::Any_Direction_Plus_Held)){
						inputBuffer[bufferCurrentSize - 1] = inputBuffer[bufferCurrentSize - 1] | inputButtons;
						updateHistory = true;
						if (historyDepth > 0) {
							inputBufferHistory[historyDepth - 1] = inputBuffer[bufferCurrentSize - 1] | inputButtons;
						}
					}
					else{
						inputBuffer.push_back(inputToAddToBuffer);
						bufferCurrentSize += 1;
						inputBufferHistory.push_back(inputToAddToBuffer);
						historyDepth += 1;
						updateHistory = true;
					}
				}
				else{
					inputBuffer.push_back(inputToAddToBuffer);
					bufferCurrentSize += 1;
					inputBufferHistory.push_back(inputToAddToBuffer);
					historyDepth += 1;
					updateHistory = true;
				}
				if (bufferCurrentSize > bufferCapacity){
					inputBuffer.pop_front();
					bufferCurrentSize -= 1;
				}
			}
			/* record the time this function has been called*/
			lastInputTimeMs = totalTimeMs;
		}
		if (lastInputTimeMs == totalTimeMs && updateBuffer){
			modified = true;
			if (updateHistory){
				while (historyDepth > maxHistoryDepth) {
					inputBufferHistory.pop_front();
					historyDepth -= 1;
				}
				historyCounterMs = 0;
			}
		}
		if (modified){
			readable = true;
		}
		lastPressedButtons = pressedButtons;
	};

	void FK_InputBuffer::update(int totalTimeMs, std::vector<bool>&keyArray, bool updateBuffer){
		/* get time difference from last input received*/
		u32 delta_t_ms = totalTimeMs - lastRecordedTimeMs;
		int timeSinceLast = totalTimeMs - lastInputTimeMs;
		historyCounterMs += delta_t_ms;
		//u32 fixedDeltaDelayMs = 100;
		lastRecordedTimeMs = totalTimeMs;
		bool updateHistory = true;
		/* if too much time has passed, clear the buffer*/
		if (timeSinceLast > inputSpanTimeMs)
		{
			inputBuffer.clear();
			bufferCurrentSize = 0;
		}
		if (timeSinceLast > historyDurationMs){
			historyDepth = 0;
			inputBufferHistory.clear();
		}
		/* re-initialize the pressed buttons value (used by the player for one update) */
		allPressedButtons = 0;
		/* initialize bitmasks */
		u32 buttonMap = 0;
		u32 pressedButtons = 0;
		u32 oldBufferSize = bufferCurrentSize;
		bool addTriggerToInput = false;
		u32 numberOfNonDirectionButtons = 0;
		for (std::map<FK_Input_Buttons, u32>::iterator iter = rawInputToButtons.begin(); iter != rawInputToButtons.end(); ++iter)
		{
			FK_Input_Buttons currentButton = iter->first;
			u32 keyCode = iter->second;
			/* check first for non-direction buttons*/
			if (keyArray[keyCode] == true){
				allPressedButtons |= currentButton;
			}
			if (previousState[keyCode] == false &&
				keyArray[keyCode] == true){
				if (currentButton & FK_Input_Buttons::Any_NonDirectionButton){
					pressedButtons |= currentButton;
					if (currentButton != FK_Input_Buttons::Trigger_Button) {
						numberOfNonDirectionButtons += 1;
					}
				}
				/* store the full input for further analysis*/
				buttonMap |= currentButton;
			}

			if (keyArray[keyCode] == true && currentButton == FK_Input_Buttons::Trigger_Button) {
				addTriggerToInput = true;
			}

			/* in case a direction is pressed, store it anyway even if it was still pressed before*/
			if (keyArray[keyCode] == true && (currentButton & FK_Input_Buttons::Any_Direction)){
				buttonMap |= currentButton;
			}
			/* store currently pressed keys for the following cycles*/
			previousState[keyCode] = keyArray[keyCode];
			/* if button is kept pressed, add time to holding time*/
			if (keyArray[keyCode]) {
				holdingTimeForButton[currentButton] += delta_t_ms;
			}else{
				holdingTimeForButton[currentButton] = -1;
			}
		}
		bool sameInputFlag = false;
		bool sameDirectionFlag = false;
		if (lastButtonMap == allPressedButtons){
			sameInputFlag = true;
			if (!updateBuffer){
				//timeForRepeatCounterMs += delta_t_ms;
				if ((allPressedButtons & FK_Input_Buttons::Any_Direction_Plus_Held) != 0 ||
					allPressedButtons != 0){
					//if (timeForRepeatCounterMs < timeSinceRepeatMs){
					lastInputTimeMs = totalTimeMs;
					//}
					/*else{
						timeForRepeatCounterMs = timeSinceRepeatMs - fixedDeltaDelayMs;
					}*/
				}
				else{
					timeForRepeatCounterMs = 0;
				}
			}
			//updateBuffer = false;
		}
		else{
			timeForRepeatCounterMs = 0;
		}

		if (numberOfNonDirectionButtons > 0 && addTriggerToInput) {
			pressedButtons |= FK_Input_Buttons::Trigger_Button;
		}

		lastButtonMap = allPressedButtons;
		// It is very hard to press two buttons on exactly the same frame.
		// If they are close enough, consider them pressed at the same time.
		bool mergeInput = (bufferCurrentSize > 0 && timeSinceLast < inputMergeTimeMs);
		// now, check for direction and compare it to the last stored
		u32 newDirection = getNewDirection(buttonMap);
		pressedButtons |= newDirection;
		if (lastDirection != newDirection)
		{
			lastDirection = newDirection;
			// combine the direction with the buttons.
			// pressedButtons |= newDirection;
			if (lastDirection != 0){
				if (updateBuffer){
					inputBuffer.push_back(newDirection);
					bufferCurrentSize += 1;
					inputBufferHistory.push_back(newDirection);
					historyDepth += 1;
				}
				lastInputTimeMs = totalTimeMs;
			}
			// Don't merge two different directions. This avoids having impossible
			// directions such as Left+Up+Right. This also has the side effect that
			// the direction needs to be pressed at the same time or slightly before
			// the buttons for merging to work.
			mergeInput = false;			
		}
		// store NULL button
		else if (newDirection == FK_Input_Buttons::None && timeSinceLast > inputNullTimeMs && bufferCurrentSize > 0){
			if (updateBuffer){
				if (inputBuffer[bufferCurrentSize - 1] != newDirection){
					inputBuffer.push_back(newDirection);
					bufferCurrentSize += 1;
					lastInputTimeMs = totalTimeMs;
					inputBufferHistory.push_back(newDirection);
					historyDepth += 1;
				}
			}
		}
		// store button as held (only direction can be held, and only one at any time)
		/*else if (sameInputFlag && newDirection != FK_Input_Buttons::None && bufferCurrentSize > 0 && timeSinceLast > inputHoldTimeMs){
			if (updateBuffer){
				if (inputBuffer[bufferCurrentSize - 1] == newDirection){
					inputBuffer[bufferCurrentSize - 1] = newDirection << FK_Input_Buttons::TaptoHoldButtonShift;
					mergeInput = false;
					lastInputTimeMs = totalTimeMs;
				}
			}
		}*/
		else if (newDirection != FK_Input_Buttons::None && bufferCurrentSize > 0) {
			if (updateBuffer) {
				if (inputBuffer[bufferCurrentSize - 1] == newDirection) {
					bool buttonHeldSuccesfully = false;
					switch (newDirection) {
					case FK_Input_Buttons::UpRight_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Up_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Right_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::UpLeft_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Up_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Left_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::DownRight_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Down_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Right_Direction] >= inputHoldTimeMs;
						break;
					case FK_Input_Buttons::DownLeft_Direction:
						buttonHeldSuccesfully = holdingTimeForButton[FK_Input_Buttons::Down_Direction] >= inputHoldTimeMs &&
							holdingTimeForButton[FK_Input_Buttons::Left_Direction] >= inputHoldTimeMs;
						break;
					default:
						buttonHeldSuccesfully = holdingTimeForButton[newDirection] >= inputHoldTimeMs;
						break;
					}
					if (buttonHeldSuccesfully) {
						inputBuffer[bufferCurrentSize - 1] = newDirection << FK_Input_Buttons::TaptoHoldButtonShift;
						mergeInput = false;
						lastInputTimeMs = totalTimeMs;
						if (historyDepth > 0) {
							inputBufferHistory[historyDepth - 1] = inputBuffer[bufferCurrentSize - 1];
						}
					}
				}
			}
		}
		else if (bufferCurrentSize == 0 && lastDirection != FK_Input_Buttons::None && 
			newDirection == lastDirection){
			if (updateBuffer){
				inputBuffer.push_back(lastDirection << FK_Input_Buttons::TaptoHoldButtonShift);
				updateHistory = false;
				bufferCurrentSize = 1;
				mergeInput = false;
				lastInputTimeMs = totalTimeMs;
			}
		}
		/* if at least one button has been pressed, update buffer */
		u32 inputToAddToBuffer = pressedButtons /*& ~fk_constants::FK_TriggerButton*/;
		pressedButtons &= FK_Input_Buttons::Any_NonDirectionButton;
		u32 bitmask = ~((u32)FK_Input_Buttons::Any_SystemButton | (u32)FK_Input_Buttons::Any_MenuButton);
		pressedButtons &= bitmask;
		/* exclude trigger button */
		u32 inputButtons = pressedButtons /*& ~fk_constants::FK_TriggerButton*/;
		if (inputButtons != 0){
			if (updateBuffer){
				if (mergeInput){
					if (!(inputBuffer[bufferCurrentSize - 1] & FK_Input_Buttons::Any_Direction_Plus_Held)){
						inputBuffer[bufferCurrentSize - 1] = inputBuffer[bufferCurrentSize - 1] | inputButtons;
						updateHistory = true;
						if (historyDepth > 0) {
							inputBufferHistory[historyDepth - 1] = inputBuffer[bufferCurrentSize - 1] | inputButtons;
						}
					}
					else{
						inputBuffer.push_back(inputToAddToBuffer);
						bufferCurrentSize += 1;
						inputBufferHistory.push_back(inputToAddToBuffer);
						historyDepth += 1;
						updateHistory = true;
					}
				}
				else{
					inputBuffer.push_back(inputToAddToBuffer);
					bufferCurrentSize += 1;
					inputBufferHistory.push_back(inputToAddToBuffer);
					historyDepth += 1;
					updateHistory = true;
				}
				if (bufferCurrentSize > bufferCapacity){
					inputBuffer.pop_front();
					bufferCurrentSize -= 1;
				}
			}
			/* record the time this function has been called*/
			lastInputTimeMs = totalTimeMs;
		}
		if (lastInputTimeMs == totalTimeMs && updateBuffer){
			modified = true;
			if (updateHistory){
				while (historyDepth > maxHistoryDepth) {
					inputBufferHistory.pop_front();
					historyDepth -= 1;
				}
				historyCounterMs = 0;
			}
		}
		if (modified){
			readable = true;
		}
		lastPressedButtons = pressedButtons;
	};

	/* set new buffer */
	void FK_InputBuffer::setBuffer(const std::deque<u32>& newBuffer, bool checkBeforeReplacing){
		u32 newSize = newBuffer.size();
		if (checkBeforeReplacing) {
			if (bufferCurrentSize == newSize && newSize > 0) {
				bool isDifferent = false;
				for (u32 i = 0; i < bufferCurrentSize; ++i) {
					if (inputBuffer[i] != newBuffer[i]) {
						isDifferent = true;
						break;
					}
				}
				if (!isDifferent) {
					modified = false;
					readable = false;
					return;
				}
			}
		}
		bufferCurrentSize = newBuffer.size();
		inputBuffer.clear();
		for (int i = 0; i < bufferCurrentSize; i++){
			inputBuffer.push_back(newBuffer[i]);
		}
		holdingTimeForButton.clear();
		modified = true;
		readable = true;
	}

	void FK_InputBuffer::clearBuffer()
	{
		inputBuffer.clear();
		bufferCurrentSize = 0;
		lastPressedButtons = 0;
	}

	/* get likeliness factor*/
	float FK_InputBuffer::getLikelinessFactor(FK_Move* move, bool leftScreenSide, u32 startingIndex){
		float likelinessFactor = 100.0f;
		for (int i = 0; i < bufferCurrentSize-startingIndex; i++){
			u32 inputToCheck = move->getInputString(leftScreenSide)[i];
			/* multiple buttons condition*/
			u32 inputBufferItem = (u32)inputBuffer[i + startingIndex];
			// remove trigger from input buffer
			if (!(inputToCheck & FK_Input_Buttons::Trigger_Button)) {
				inputBufferItem &= ~FK_Input_Buttons::Trigger_Button;
			}
			//inputBufferItem &= ~FK_Input_Buttons::Trigger_Button;
			// go on with usual checks
			bool tapCondition = (inputToCheck & FK_Input_Buttons::Any_Direction) &&
				(inputBufferItem & FK_Input_Buttons::Any_HeldButton) &&
				(inputBufferItem >> FK_Input_Buttons::TaptoHoldButtonShift == inputToCheck);
			u32 multiButtonA = inputToCheck & FK_Input_Buttons::Any_NonDirectionButton;
			u32 multiButtonB = inputToCheck & inputBufferItem;
			u32 multiButtonC = inputBufferItem & FK_Input_Buttons::Any_NonDirectionButton;
			bool baseDirectionCheck = 
				tapCondition || (
				(inputToCheck & FK_Input_Buttons::Any_Direction_Plus_Held) == 0 &&
				(inputBufferItem & FK_Input_Buttons::Any_Direction_Plus_Held) != 0) ||
				((inputToCheck & FK_Input_Buttons::Any_Direction_Plus_Held) == 
				(inputBufferItem & FK_Input_Buttons::Any_Direction_Plus_Held));
			bool baseMultiButtonCheck =
				baseDirectionCheck &&
				multiButtonA != FK_Input_Buttons::Punch_Button &&
				multiButtonA != FK_Input_Buttons::Kick_Button &&
				multiButtonA != FK_Input_Buttons::Tech_Button &&
				multiButtonA != FK_Input_Buttons::Guard_Button;
			bool multipleButtonsCondition = baseMultiButtonCheck && (multiButtonA != 0 && multiButtonA == multiButtonC && multiButtonB > 0);
			bool lesserButtonsCondition = baseDirectionCheck && (multiButtonA != 0 && multiButtonC != 0 &&
				(multiButtonA & multiButtonC) == multiButtonA &&
				multiButtonB > 0);
			/* inclusive directions condition */
			bool inclusiveDirectionCondition = false;
			u32 directionCheck = inputToCheck & FK_Input_Buttons::Any_Direction_Plus_Held;
			u32 directionBuffer = inputBufferItem & FK_Input_Buttons::Any_Direction_Plus_Held;
			if ((directionCheck == FK_Input_Buttons::Down_Direction ||
				directionCheck == FK_Input_Buttons::HeldDown_Direction) &&
				(directionBuffer & directionCheck)){
				inclusiveDirectionCondition = true;
			}
			else if ((directionCheck == FK_Input_Buttons::Up_Direction ||
				directionCheck == FK_Input_Buttons::HeldUp_Direction) &&
				(directionBuffer & directionCheck)){
				inclusiveDirectionCondition = true;
			}
			// add a check on non-direction buttons
			inclusiveDirectionCondition &= (inputToCheck & FK_Input_Buttons::Any_NonDirectionButton) == (inputBufferItem & FK_Input_Buttons::Any_NonDirectionButton);
			if ((inputBufferItem != inputToCheck)){
				if(move->requiresPreciseInput()){
					likelinessFactor *= 0.0f;
					break;
				}
				if (!(tapCondition || multipleButtonsCondition || lesserButtonsCondition || inclusiveDirectionCondition)){
					likelinessFactor *= 0.0f;
					break;
				}
				else{
					if (inclusiveDirectionCondition){
						f32 modificationFactor = std::max(1.f, 1.5f / move->getInputString(leftScreenSide).size());
						likelinessFactor /= modificationFactor;
					}
					if (tapCondition){
						f32 modificationFactor = std::max(1.f, 2.5f / move->getInputString(leftScreenSide).size());
						likelinessFactor /= modificationFactor;
					}
					else if (multipleButtonsCondition){
						f32 modificationFactor = std::max(1.f, 3.0f / move->getInputString(leftScreenSide).size());
						likelinessFactor /= modificationFactor;
					}
					else if (lesserButtonsCondition){
						f32 modificationFactor = std::max(1.f, 3.5f / move->getInputString(leftScreenSide).size());
						likelinessFactor /= modificationFactor;
					}
					//likelinessFactor /= 3.0 / move->getInputString(leftScreenSide).size();
				}
			}
			else{
				likelinessFactor *= 2.0;
			}
		}
		return likelinessFactor;
	};


	/* check for specific input sequence */
	bool FK_InputBuffer::checkForSequence(const std::deque<u32>& bufferToCheck) {
		if (bufferCurrentSize < (u32)bufferToCheck.size()) {
			return false;
		}
		u32 startingIndex = (u32)bufferCurrentSize - (u32)bufferToCheck.size();
		for (u32 i = 0; i < bufferCurrentSize - startingIndex; ++i) {
			u32 inputToCheck = bufferToCheck[i];
			/* multiple buttons condition*/
			u32 inputBufferItem = (u32)inputBuffer[i + startingIndex];
			inputBufferItem &= ~(FK_Input_Buttons::Trigger_Button | 
				FK_Input_Buttons::Any_SystemButton | FK_Input_Buttons::Any_MenuButton);
			// remove trigger from input buffer
			if (inputToCheck != inputBufferItem) {
				return false;
			}
		}
		return true;
	}

	/* read buffer and output the move */
	FK_Move* FK_InputBuffer::readBuffer(FK_Character* character){
		FK_Move* new_move = NULL;
		if (!readable){
			return new_move;
		}
		// create variables (as for previous method, directly from character)
		bool allowFollowupOnlyMoves = character->getLastStackedMove() != NULL;
		FK_Move* currentMove = character->getLastStackedMove();
		bool leftScreenSide = character->isOnLeftSide();
		FK_Stance_Type playerStance = character->getStance();
		s32 numberOfCounters = character->getTriggerCounters();

		auto newMoveIterator = character->getMovesCollection().begin();
		float old_likelinessFactor = 0.0f;
		// return an empty move if the current move can only be canceled after a hit was scored
		if (currentMove != NULL && currentMove->canBeFollowedOrCanceledOnHitOnly() && !currentMove->hasConnectedWithTarget()) {
			return NULL;
		}
		// else, follow the usual procedure
		for (auto it = character->getMovesCollection().begin(); it != character->getMovesCollection().end(); ++it)
		{
			FK_Move* move = &(*it);
			if (move->getInputStringLeft().empty()){
				continue;
			}
			if (move->isFollowupOnly() && !allowFollowupOnlyMoves){
				continue;
			}
			if (move->getNumberOfRequiredBulletCounters() > 0){
				if (numberOfCounters < move->getNumberOfRequiredBulletCounters()){
					continue;
				}
			}
			/* if currentMove is not NULL, check ONLY for followup moves OR cancels!*/
			if (currentMove != NULL){
				u32 size = currentMove->getFollowupMovesSet().size();
				bool isFollowup = false;
				for (int j = 0; j < size; ++j){
					if (currentMove->getFollowupMovesSet()[j].getMoveId() == move->getMoveId()){
						isFollowup = true;
						break;
					}
				};
				if (!isFollowup){
					size = currentMove->getCancelIntoMovesSet().size();
					for (int j = 0; j < size; ++j){
						if (currentMove->getCancelIntoMovesSet()[j].getMoveId() == move->getMoveId()){
							isFollowup = true;
							break;
						}
					};
				}
				if (!isFollowup){
					continue;
				}
			}
			else{
				// check player stance only if the move is not a followup
				if (move->getStance() != playerStance){
					continue;
				}
			}
			/* if the move has object requirements, check them before proceeding */
			if (move->hasItemRequirements()){
				bool moveAvailable = true;
				u32 countForbidden = move->getForbiddenObjects().size();
				u32 countRequired = move->getRequiredObjects().size();
				for (u32 i = 0; i < countRequired + countForbidden; ++i) {
					if (i < countRequired) {
						moveAvailable &= (character->getObjectByName(move->getRequiredObjects().at(i)) != NULL &&
							character->getObjectByName(move->getRequiredObjects().at(i))->isActive());
					}else{
						u32 k = i - countRequired;
						moveAvailable &= (character->getObjectByName(move->getForbiddenObjects().at(k)) != NULL &&
							!character->getObjectByName(move->getForbiddenObjects().at(k))->isActive());
					}
					if (!moveAvailable) {
						break;
					}
				}
				if (move->getPickableItemId() != std::string() &&
					!move->canPickUpItem(character->getPickableObjectId()) &&
					!move->isMoveAvailableWithoutPickup()) {
					moveAvailable = false;
				}
				if (!moveAvailable) {
					continue;
				}
				//u32 count = character->getCharacterAdditionalObjects().size();
				//for (u32 i = 0; i < count; ++i){
				//	std::string nameTag = character->getCharacterAdditionalObjects().at(i).uniqueNameId;
				//	if (nameTag != std::string()){
				//		if (move->isObjectRequired(nameTag)){
				//			moveAvailable &= character->getCharacterAdditionalObjects().at(i).isActive();
				//		}
				//		else if (move->isObjectForbidden(nameTag)){
				//			moveAvailable &= !character->getCharacterAdditionalObjects().at(i).isActive();
				//		}
				//		// check if move can pick up object only
				//		if (move->canPickUpItem(nameTag) &&
				//			character->getPickableObjectId() != nameTag &&
				//			!move->canBeTriggered()){
				//			moveAvailable = false;
				//		}
				//	}
				//	if (!moveAvailable){
				//		break;
				//	}
				//}
				//if (!moveAvailable){
				//	continue;
				//}
			}
			int moveInputSize = move->getInputString(leftScreenSide).size();
			/* if the buffer is smaller than the move input requirement, go to the next move*/
			if (bufferCurrentSize < moveInputSize){
				continue;
			}
			/* if the buffer has the exact same length, check all inputs one-by-one*/
			if (bufferCurrentSize == moveInputSize){
				bool skip_flag = false;
				float new_likelinessFactor = getLikelinessFactor(move, leftScreenSide);
				//std::cout << move->getName() << " " << new_likelinessFactor << std::endl;
				if (new_likelinessFactor <= 0){
					skip_flag = true;
				}
				if (skip_flag){
					continue;
				}
				else{
					if (new_likelinessFactor > old_likelinessFactor){
						old_likelinessFactor = new_likelinessFactor;
						newMoveIterator = it;
					}
				}
			}
			/* if the buffer is bigger, slide the input in order to look for a good match */
			if (bufferCurrentSize > moveInputSize){
				int delta = bufferCurrentSize - moveInputSize;
				/* browse the deque for a precise match, popping elements from the front until the length match */
				bool skip_flag = false;
				float new_likelinessFactor = getLikelinessFactor(move, leftScreenSide, delta);
				//std::cout << move->getName() << " " << new_likelinessFactor << std::endl;
				if (new_likelinessFactor <= 0){
					skip_flag = true;
				}
				if (skip_flag){
					continue;
				}
				else{
					if (new_likelinessFactor > old_likelinessFactor){
						old_likelinessFactor = new_likelinessFactor;
						newMoveIterator = it;
					}
				}
			}
		}
		if (old_likelinessFactor > 0.0f){
			if ((*newMoveIterator).canClearInputBufferOnExecution()){
				bufferCurrentSize = 0;
				inputBuffer.clear();
			}
			return &(*newMoveIterator);
		}
		return new_move;
	}

}