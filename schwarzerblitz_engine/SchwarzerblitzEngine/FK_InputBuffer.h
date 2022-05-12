/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

/* FK_InputBuffer is a class which handles input and input reading, translating them into game actions.
The input is read continuously and stored in a buffer, then pattern-matched to every possible move in
order to get the best possible combination. This file contains the header of the class with comments and
explanations */
#ifndef FK_INPUT_BUFFER_CLASS
#define FK_INPUT_BUFFER_CLASS

//#define DIAGNOSTICS
#ifdef DIAGNOSTICS
#include<fstream>
#include<ctime>
#endif

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
		void update(int totalTimeMs, u32 pressedButtons, bool updateBuffer = true);
		u32 convertKeyArrayToPressedButtons(std::vector<bool>& keyArray);
		u32 convertKeyArrayToPressedButtons(bool* keyArray);
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
#ifdef DIAGNOSTICS
		std::ofstream diagnosticsFile;
#endif
	};
};

#endif