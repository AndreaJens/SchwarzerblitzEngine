# ifndef FK_SCENEVIEWUPDATER_CLASS
# define FK_SCENEVIEWUPDATER_CLASS

#include<irrlicht.h>
#include"FK_Character.h"
#include"FK_Stage.h"

using namespace irr;
using namespace scene;

namespace fk_engine{
	class FK_SceneViewUpdater{
	public:
		FK_SceneViewUpdater(ISceneManager* new_smgr, FK_Character* new_player1, FK_Character* new_player2, FK_Stage* new_stage,
			ICameraSceneNode * new_camera, float cameraYposition, float cameraXZdistance, float cameraTargetOffsetY = -15.0, float cameraNearField = 80.0f,
			float cameraFarField = 240.0f, float new_distanceScaleFactorXZ = 320.0f);
		~FK_SceneViewUpdater();
		FK_Character* getLeftSidePlayer();
		FK_Character* getRightSidePlayer();
		int getDirectionDeterminant();
		void update(f32 delta_t_s, f32 additionalCameraZoom = 0.f);
		void updateVictory(int winnerId);
		void updateContinue(int playerId);
		void updateIntro(int playerId);
		bool updateStageViewIntro(f32 delta_t_s);
		void resetCameraPosition();
		void alignCharacters(FK_Character* targetPlayer);
		void alignPlayerTowardsPlayer(FK_Character* playerToAlign, FK_Character* referencePlayer);
	protected:
		void setupThrow();
		void updateBulletStageInteractions();
		void setupThrowTotalDistance(FK_Character* throwingPlayer, f32& distance, 
			core::vector3df& totalMovementThrowingPlayer, core::vector3df& totalMovementThrownPlayer);
		bool areCharactersColliding(FK_Character *one, FK_Character *two, bool ignoreVerticalDirection);
		bool getIfVerticalDistanceIsIgnoredForCalculatingCollisions(FK_Character *one, FK_Character *two);
		bool isCharacterInsideStageWallbox(FK_Character *characterToUpdate, core::aabbox3d<f32> &trespassingBox);
		bool isCharacterInsideVanillaStageWallbox(FK_Character *characterToUpdate, core::aabbox3d<f32> &trespassingBox);
		bool isCharacterInsideComplexStageWallbox(FK_Character *characterToUpdate, core::aabbox3d<f32> &trespassingBox);
		void classifyStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
			core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
			bool &playerIsCornered);
		void classifyVanillaStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
			core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
			bool &playerIsCornered);
		void classifyComplexStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
			core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
			bool &playerIsCornered);
		bool getIfCharacterIsCornered(FK_Character* character);
		bool isCharacterInsideRingoutZone(FK_Character *characterToUpdate);
		bool isCharacterInsideVanillaRingoutZone(FK_Character *characterToUpdate);
		bool isCharacterInsideComplexRingoutZone(FK_Character *characterToUpdate);
		void updateCharacterPositions(f32 delta_t_s, FK_Character* leftPlayer, FK_Character* rightPlayer, 
			core::vector3df& shavedMovementStage, core::vector3df& additionalMovement);
		bool getCharacterCollisionCondition(FK_Character *characterToUpdate, FK_Character *sparringCharacter);
		void updateThrow();
		void updateCinematic(FK_Character* attackingPlayer, FK_Character* targetPlayer);
	private:
		ISceneManager* smgr;
		FK_Character* player1;
		FK_Character* player2;
		FK_Stage* stage;
		ICameraSceneNode* camera;
		float baseCameraYPosition;
		float baseCameraDistance;
		float baseCameraTargetOffsetY;
		float nearestCameraDistance;
		float farthestCameraDistance;
		float distanceScaleFactorXZ;
		float lastTargetY;
		f32 timeCounter;
		s32 oldDirectionDeterminant;
		f32 intersectionDistance;
		bool throwStart;
		bool isUpdatingCinematic;
		f32 defaultFOVAngle;
		f32 battleFOVAngle;
		core::vector3df lastNonZeroCharacterPositionDelta;
		core::vector3df lastCameraPosition;
		core::vector3df lastCameraTarget;
		core::vector3df lastCameraAngle;
	};
};

#endif