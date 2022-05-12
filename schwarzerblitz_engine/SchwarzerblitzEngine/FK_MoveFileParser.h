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


#ifndef FK_MOVEFILEPARSER_CLASS
#define FK_MOVEFILEPARSER_CLASS

#include<irrlicht.h>
#include<fstream>
#include<string>
#include<vector>
#include<deque>
#include<map>
#include"FK_Move.h"

using namespace irr;

namespace fk_engine{

	class FK_MoveFileParser{
	private:
		const std::string FollowupAllRootMovesTag = "*ALL_ROOT_MOVES";
		const std::string FollowupAllDamagingRootMovesTag = "*ALL_DAMAGING_ROOT_MOVES";
	public:
		FK_MoveFileParser();
		FK_MoveFileParser(std::string fileName);
		~FK_MoveFileParser();
		std::deque<FK_Move> getParsedMoves();
		std::deque<FK_Move> parseMovesFromFile(std::ifstream &moveInputFile);
		void setFollowupsIds(std::deque<FK_Move>& parsedMoves);
		const std::map<std::string, FK_Hitbox_Type> & getHitboxParsingMap();
		const std::map<std::string, FK_Reaction_Type> & getReactionParsingMap();
	protected:
		/* protected functions */
		void setupParserMap();
		void resetMoveVariables();
		void resetHitboxVariables();
		void resetInputVariables();
		void resetAllVariables();
		FK_Hitbox readHitboxFromLine(std::string line);
	private:
		std::deque<FK_Move> parsedMoves;
		/* enum maps*/
		std::map<FK_FileKey_Type, std::string> readerMap;
		std::map<std::string, FK_Hitbox_Type> hitboxTypeMap;
		std::map<std::string, FK_Reaction_Type> reactionTypeMap;
		std::map<std::string, FK_Stance_Type> stanceTypeMap;
		std::map<std::string, u32> inputMap;
		std::map<std::string, FK_Attack_Type> attackTypeMap;
		std::map<std::string, FK_Attack_Type> attackTypeMapInvincibility;
		std::map<FK_FileKey_BulletType, std::string> bulletAttributesMap;
		std::map<FK_FileKey_ThrowKeys, std::string> throwAttributesMap;
		std::map<FK_FileKey_TransformationKeys, std::string> transformationAttributesMap;
		std::map<FK_FileKey_CinematicKeys, std::string> cinematicAttributesMap;
		/* list of transformations */
		std::map<std::string, FK_Move::FK_TransformationMove> availableTransformations;
		/* temporary variables*/
		FK_Move move;
		std::string moveName;
		std::vector<FK_FollowupMove> moveFollowUp;
		std::vector<FK_FollowupMove> moveCancels;
		int movePriority;
		int moveStartingFrame;
		int moveEndingFrame;
		std::deque<u32> moveInputLeft;
		std::deque<u32> moveInputRight;
		std::vector<FK_Hitbox> hitboxes;
		FK_Hitbox_Type hitboxType;
		FK_Reaction_Type hitboxReaction;
		FK_Attack_Type hitboxAttackType;
		int hitboxDamage;
		int hitboxStartFrame;
		int hitboxEndFrame;
		bool hitboxOneHit;
		bool hitboxGuardBreak;
		int hitboxPriority;
		std::string listedInMoveListIdentifier;
		std::string interpolationIdentifier;
		std::string combinationIdentifier;
		std::string hitboxSingleHitIdentifier;
		std::string hitboxMultiHitIdentifier;
		std::string hitboxStartFrameIdentifier;
		std::string hitboxEndFrameIdentifier;
		std::string hitboxGuardBreakIdentifier;
	};

}

#endif