/* FK_InputBuffer is a class which handles input and input reading, translating them into game actions.
The input is read continuously and stored in a buffer, then pattern-matched to every possible move in
order to get the best possible combination. This file contains the header of the class with comments and
explanations */
#ifndef FK_INPUT_BUFFER_CLASS
#define FK_INPUT_BUFFER_CLASS

#include<irrlicht.h>
#include<deque>
#include<map>
#include "FK_Database.h"
#include "FK_Move.h"

using namespace irr;

namespace fk_engine{

	class FK_Character;

	class FK_InputBuffer{
	public:
		FK_InputBuffer(int newPlayerIndex, 
			const std::map<FK_Input_Buttons, u32> &buttonMap,
			int spanTimeMs = 200, int mergeTimeMs = 30, int holdTimeMs = 70, 
			int nullTimeMs = 150, int maxInputNumber = 10);
		~FK_InputBuffer();
		void setLastInputTime(int newTime);
		void update(int totalTimeMs, bool* keyArray, bool updateBuffer = true);
		void update(int totalTimeMs, std::vector<bool>& keyArray, bool updateBuffer = true);
		/*FK_Move *readBuffer(std::deque<FK_Move> &availableMoves, bool leftScreenSide, FK_Stance_Type playerStance,
			 bool allowFollowupOnlyMoves = false, FK_Move* currentMove = NULL);*/
		FK_Move *readBuffer(FK_Character* character);
		bool checkForSequence(const std::deque<u32>& bufferToCheck);
		float getLikelinessFactor(FK_Move* move, bool leftScreenSide, u32 startingIndex = 0);
		int  getPlayerId();
		int  getLastInputTime();
		u32  getNewDirection(u32 buttonMap);
		u32  getBufferElementFromHistory(int elementIndex);
		u32  getBufferElement(int elementIndex);
		u32  getLastDirection();
		u32  getPressedButtons();
		void setPressedButtons(u32 allButtonsPressed);
		void setLastDirection(u32 lastDirection);
		bool isModified();
		void resetModified();
		std::deque<u32> getBuffer();
		std::deque<u32> getHistoryBuffer();
		void setBuffer(const std::deque<u32>& newBuffer, bool checkBeforeReplacing = false);
		void clearBuffer();
	private:
		std::deque<u32> inputBuffer;
		std::deque<u32> inputBufferHistory;
		std::map<FK_Input_Buttons, u32> rawInputToButtons;
		std::map<u32, s32> holdingTimeForButton;
		u32 historyDepth;
		u32 maxHistoryDepth;
		u32 historyDurationMs;
		u32 historyCounterMs;
		u32 lastRecordedTimeMs;
		int lastInputTimeMs;
		int inputSpanTimeMs;
		int inputMergeTimeMs;
		int inputHoldTimeMs;
		int inputNullTimeMs;
		int playerIndex;
		int bufferCapacity;
		int bufferCurrentSize;
		u32 lastDirection;
		u32 allPressedButtons;
		u32 lastPressedButtons;
		u32 lastButtonMap;
		u32 timeSinceRepeatMs;
		u32 timeForRepeatCounterMs;
		bool *previousState;
		bool modified;
		bool readable;
	};
};

#endif