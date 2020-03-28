#include"FK_InputReceiver.h"
#include"FK_Database.h"
#include<iostream>

namespace fk_engine{

	FK_InputReceiver::FK_InputReceiver()
	{
		joypadRecordedNumber = 0;
		for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i){
			KeyIsDown[i] = false;
		}
		JoypadState[0] = SEvent::SJoystickEvent();
		JoypadState[1] = SEvent::SJoystickEvent();
		KeyIsDownJoypad[0] = NULL;
		KeyIsDownJoypad[1] = NULL;
	};


	FK_InputReceiver::~FK_InputReceiver(){
		for (std::map<int, bool*>::iterator iter = KeyIsDownJoypad.begin(); iter != KeyIsDownJoypad.end(); ++iter)
		{
			delete [] iter->second;
		}
		KeyIsDownJoypad.clear();
		JoypadState.clear();
	};

	void FK_InputReceiver::reset(){
		for (u32 i = 0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
		for (u32 joypadId = 0; joypadId < joypadRecordedNumber; ++joypadId){
			if (KeyIsDownJoypad.count(joypadId) > 0 && KeyIsDownJoypad[joypadId] != NULL) {
				delete [] KeyIsDownJoypad[joypadId];
			}
			JoypadState[joypadId] = SEvent::SJoystickEvent();
			KeyIsDownJoypad[joypadId] = new bool[maxInputSize];
			for (u32 i = 0; i < maxInputSize; ++i){
				KeyIsDownJoypad[joypadId][i] = false;
			}
		}
	}

	void FK_InputReceiver::setJoypadInfo(core::array<SJoystickInfo> &joypadInfo)
	{
		joypadRecordedNumber = joypadInfo.size();
		for (u32 joypadId = 0; joypadId < joypadInfo.size(); ++joypadId){
			if (KeyIsDownJoypad.count(joypadId) > 0 && KeyIsDownJoypad[joypadId] != NULL) {
				delete [] KeyIsDownJoypad[joypadId];
			}
			JoypadState[joypadId] = SEvent::SJoystickEvent();
			KeyIsDownJoypad[joypadId] = new bool[maxInputSize];
			for (u32 i = 0; i < maxInputSize; ++i){
				KeyIsDownJoypad[joypadId][i] = false;
			}
		}
	};

	bool FK_InputReceiver::OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT){
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
		}
		// store joypad state for player 1
		if (event.EventType == irr::EET_JOYSTICK_INPUT_EVENT)
		{
			updateJoypadStatus(event);
		}
		return false;
	}

	void FK_InputReceiver::updateJoypadStatus(const SEvent& event){
		// get joypad id
		u32 joypadId = event.JoystickEvent.Joystick;
		irr::s32 deadzoneForSticks = 35;
		
		// update the status of the joypad
		if (event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_X] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_X] ||
			
			event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Y] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_Y] ||
			
			event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Z] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_Z] ||
			
			event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_R] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_R] ||
			
			event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_U] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_U] ||
			
			event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_V] != JoypadState[joypadId].Axis[SEvent::SJoystickEvent::AXIS_V] ||
			
			event.JoystickEvent.POV != JoypadState[joypadId].POV ||
			event.JoystickEvent.ButtonStates != JoypadState[joypadId].ButtonStates){
			JoypadState[joypadId] = event.JoystickEvent;
			int JoypadBufferSize = JoypadState[joypadId].NUMBER_OF_BUTTONS;
			for (u32 i = 0; i < JoypadBufferSize; i++){
				bool buttonFlag = JoypadState[joypadId].IsButtonPressed(i);
				KeyIsDownJoypad[joypadId][i] = buttonFlag;
			}
			// check for 8 additional buttons (axis U+/-, V+/-, Z+/-, R+/-)
			u32 joypadIndexExtraButtons = JoypadBufferSize;
			f32 threshold = 0.5f;
			//std::cout << "U-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_U] << std::endl;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] = 
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_U] / AxisMaximumMovement > threshold;
			joypadIndexExtraButtons += 1;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_U] / AxisMaximumMovement < -threshold;
			joypadIndexExtraButtons += 1;
			//std::cout << "V-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_V] << std::endl;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_V] / AxisMaximumMovement > threshold;
			joypadIndexExtraButtons += 1;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_V] / AxisMaximumMovement < -threshold;
			joypadIndexExtraButtons += 1;
			//std::cout << "Z-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Z] << std::endl;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Z] / AxisMaximumMovement < -threshold;
			joypadIndexExtraButtons += 1;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Z] / AxisMaximumMovement > threshold;
			joypadIndexExtraButtons += 1;
			//std::cout << "R-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_R] << std::endl;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_R] / AxisMaximumMovement < -threshold;
			joypadIndexExtraButtons += 1;
			KeyIsDownJoypad[joypadId][joypadIndexExtraButtons] =
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_R] / AxisMaximumMovement > threshold;
			// check for directions
			//std::cout << "X-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_X] << std::endl;
			//std::cout << "Y-Axis: " << event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Y] << std::endl << std::endl;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputDown] |= JoypadState[joypadId].POV / 100.0 >= 135 &&
				JoypadState[joypadId].POV / 100.0 <= 225;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputDown] |=
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Y] / AxisMaximumMovement > threshold;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputUp] |= (JoypadState[joypadId].POV / 100.0 >= 315 ||
				JoypadState[joypadId].POV / 100.0 <= 45) && JoypadState[joypadId].POV / 100.0 <= 360;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputUp] |=
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Y] / AxisMaximumMovement < -threshold;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputLeft] |= JoypadState[joypadId].POV / 100.0 >= 225 &&
				JoypadState[joypadId].POV / 100.0 <= 315;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputLeft] |=
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_X] / AxisMaximumMovement < -threshold;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputRight] |= JoypadState[joypadId].POV / 100.0 >= 45 &&
				JoypadState[joypadId].POV / 100.0 <= 135;
			KeyIsDownJoypad[joypadId][FK_JoypadInput::JoypadInputRight] |=
				event.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_X] / AxisMaximumMovement > threshold;
		}
		//std::cout << Joypad1State.ButtonStates << std::endl;
		
	}

	// This is used to check whether a key is being held down
	bool FK_InputReceiver::IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	};

	//get keyboard status
	bool *FK_InputReceiver::KeyboardStatus(){
		return KeyIsDown;
	};

	// get joypad status
	bool *FK_InputReceiver::JoypadStatus(int joypadId)
	{
		return KeyIsDownJoypad[joypadId];
	};

}