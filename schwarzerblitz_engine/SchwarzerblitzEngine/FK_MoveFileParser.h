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