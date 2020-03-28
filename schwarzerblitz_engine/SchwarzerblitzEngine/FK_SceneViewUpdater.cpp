#include<cmath>
#include<iostream>
#include"FK_SceneViewUpdater.h"


namespace fk_engine{
	/* cosntructor of the class
	- new_player1: pointer to the player1 object
	- new_player2: pointer to the player2 object
	- new_camera: pointer to the scene camera
	- cameraYposition: Y position of the camera
	- cameraXZdistance: normal distance between the camera and the line which connects the players
	- cameraTargetOffsetY: basic distance between the Y target point and the camera Y [DEFAULT: -8.0]
	- cameraNearField: minimum camera distance from the players (XZ direction) [DEFAULT: 80.0]
	- cameraFarField: maximum camera distance from the players (XZ direction)  [DEFAULT: 240.0]
	- new_distanceScaleFactorXZ: determines how fast does the camera go away form the players [DEFAULT: 320.0]*/
	FK_SceneViewUpdater::FK_SceneViewUpdater(ISceneManager* new_smgr,
		FK_Character* new_player1, FK_Character* new_player2,
		FK_Stage* new_stage, ICameraSceneNode * new_camera, float cameraYposition, float cameraXZdistance, float cameraTargetOffsetY, 
		float cameraNearField, float cameraFarField, float new_distanceScaleFactorXZ){
		smgr = new_smgr;
		player1 = new_player1;
		player2 = new_player2;
		stage = new_stage;
		camera = new_camera;
		baseCameraYPosition = cameraYposition;
		baseCameraDistance  = cameraXZdistance;
		baseCameraTargetOffsetY = cameraTargetOffsetY;
		nearestCameraDistance = cameraNearField;
		farthestCameraDistance = cameraFarField;
		distanceScaleFactorXZ = new_distanceScaleFactorXZ;
		lastCameraPosition = core::vector3df(0.f, 0.f, 0.f);
		lastCameraTarget = core::vector3df(0.f, 0.f, 0.f);
		lastCameraAngle = core::vector3df(0.f, -1.f, 0.f);
		timeCounter = 0;
		oldDirectionDeterminant = 1;
		intersectionDistance = 35.0f; //A.K.A minimum distance between characters
		defaultFOVAngle = core::PI / 2.5;
		battleFOVAngle = core::PI / 2.8;
		throwStart = false;
		lastNonZeroCharacterPositionDelta = core::vector3df(0.f, 0.f, 0.f);
		isUpdatingCinematic = false;
		// call the first update to position the camera properly
		update(0.f);
	};

	/* declare empty destructor*/
	FK_SceneViewUpdater::~FK_SceneViewUpdater(){

	};

	/* this function returns 1 if player 1 is at the left of the camera, -1 if player 2 is on the left side or 0 if they
	are superimposed*/
	int FK_SceneViewUpdater::getDirectionDeterminant(){
		/*if ((player1->isJumping() || player2->isJumping())){
			return oldDirectionDeterminant;
		}*/
		if (isUpdatingCinematic) {
			return oldDirectionDeterminant;
		}
		core::vector3df p1position = player1->getPosition();
		core::vector3df p2position = player2->getPosition();
		core::vector3df fightersMidPoint;
		core::vector3df cameraPosition = camera->getPosition();
		fightersMidPoint.X = (p1position.X + p2position.X) / 2;
		fightersMidPoint.Z = (p1position.Z + p2position.Z) / 2;
		f32 discriminant = ((fightersMidPoint.X - cameraPosition.X) * (p1position.Z - cameraPosition.Z) -
			(fightersMidPoint.Z - cameraPosition.Z) * (p1position.X - cameraPosition.X));
		return (discriminant > 0) ? 1 : ((discriminant < 0) ? -1 : oldDirectionDeterminant);
	}

	/* get the player positioned on the left side of the screen */
	FK_Character* FK_SceneViewUpdater::getLeftSidePlayer(){
		oldDirectionDeterminant = getDirectionDeterminant();
		if (oldDirectionDeterminant > 0){
			return player1;
		}
		else{
			return player2;
		}
	};
	/* get the player positioned on the right side of the screen */
	FK_Character* FK_SceneViewUpdater::getRightSidePlayer(){
		oldDirectionDeterminant = getDirectionDeterminant();
		if (oldDirectionDeterminant <= 0){
			return player1;
		}
		else{
			return player2;
		}
	};

	bool FK_SceneViewUpdater::getIfVerticalDistanceIsIgnoredForCalculatingCollisions(FK_Character *one, FK_Character *two) {
		bool flag = false;
		//if ((one->getStance() == FK_Stance_Type::CrouchedStance) && two->isAirborne()) {
		//	return false;
		//}
		//if ((one->isJumping() && one->getCurrentMove() == NULL) && two->getStance() == FK_Stance_Type::GroundStance) {
		//	return true;
		//}
		/*if ((one->isRunning() && one->getCurrentMove() == NULL) && two->isHitStun()) {
			return true;
		}*/
		if ((one->isHitStun() && one->getVelocityPerSecond().Z != 0) || 
			(two->isHitStun() && two->getVelocityPerSecond().Z != 0)) {
			return true;
		}
		if ((one->getStance() == FK_Stance_Type::AnyStandardStance && one->getCurrentMove() == NULL &&
			!one->isJumping()) &&
			(two->getStance() == FK_Stance_Type::AnyStandardStance && two->getCurrentMove() == NULL &&
				!two->isJumping())) {
			return true;
		}
		return flag;
	}

	/* check collisions between characters (avoid compenetration) */
	bool FK_SceneViewUpdater::areCharactersColliding(FK_Character *one, FK_Character *two, bool ignoreVerticalDirection) {
		if (player1->isBeingThrown() || player2->isBeingThrown()){
			return false;
		}
		core::aabbox3d<f32> b1, b2;
		bool distanceFlag = false;
		core::vector3df deltaPosition = two->getPosition() - one->getPosition();
		/*if ((one->getCurrentMove() == NULL && two->isHitStun() && two->getVelocityPerSecond().Z != 0) ||
			(two->getCurrentMove() == NULL && one->isHitStun() && one->getVelocityPerSecond().Z != 0)) {
			deltaPosition.Y = 0;
		}*/
		if (ignoreVerticalDirection ||
			getIfVerticalDistanceIsIgnoredForCalculatingCollisions(one, two)/* ||
			getIfVerticalDistanceIsIgnoredForCalculatingCollisions(two, one)*/) {
			deltaPosition.Y = 0;
		}
		distanceFlag = deltaPosition.getLength() <= intersectionDistance;
		return distanceFlag;
	}

	/* update the interaction of bullets and stage box*/
	void FK_SceneViewUpdater::updateBulletStageInteractions(){
		if (stage->getWallbox().isEmpty()){
			return;
		}
		/* check collision for bullets for both players */
		for each(auto bullet in player1->getBulletsCollection()){
			// only low hitting bullets are stopped by wallboxes
			if (bullet->isDisposeable()){
				continue;
			}
			if ((bullet->getHitbox().getAttackType() & FK_Attack_Type::LowAtks) == 0){
				continue;
			}
			auto b1 = bullet->getHitboxNode()->getTransformedBoundingBox();
			bool intersectFlag = b1.isFullInside(stage->getWallbox());
			if (!intersectFlag){
				intersectFlag = b1.intersectsWithBox(stage->getWallbox());
			}
			if (!intersectFlag){
				bullet->getHitbox().setHit(true);
				bullet->setRange(1.0f);
				if (bullet->getParticleEmitter() == NULL) {
					player1->activateParticleEffect(bullet->getPosition());
				}
			}
		}
		/* check collision for bullets for both players */
		for each(auto bullet in player2->getBulletsCollection()){
			// only low hitting bullets are stopped by wallboxes
			if (bullet->isDisposeable()){
				continue;
			}
			if ((bullet->getHitbox().getAttackType() & FK_Attack_Type::LowAtks) == 0){
				continue;
			}
			auto b1 = bullet->getHitboxNode()->getTransformedBoundingBox();
			bool intersectFlag = b1.isFullInside(stage->getWallbox());
			if (!intersectFlag){
				intersectFlag = b1.intersectsWithBox(stage->getWallbox());
			}
			if (!intersectFlag){
				bullet->getHitbox().setHit(true);
				bullet->setRange(1.0f);
				if (bullet->getParticleEmitter() == NULL) {
					player2->activateParticleEffect(bullet->getPosition());
				}
			}
		}
	}

	/* check collisions with stage wallbox */
	bool FK_SceneViewUpdater::isCharacterInsideStageWallbox(FK_Character* playerToUpdate, 
		core::aabbox3d<f32> &trespassingBox){
		/* "zero" the trespassing box */
		trespassingBox = core::aabbox3d<f32>(0, 0, 0, 0, 0, 0);
		/* apply proper method*/
		if (stage->hasComplexShape()) {
			return isCharacterInsideComplexStageWallbox(playerToUpdate, trespassingBox);
		}
		else {
			return isCharacterInsideVanillaStageWallbox(playerToUpdate, trespassingBox);
		}
	}

	/* check collisions with stage wallbox */
	bool FK_SceneViewUpdater::isCharacterInsideVanillaStageWallbox(FK_Character* playerToUpdate,
		core::aabbox3d<f32> &trespassingBox) {
		/* "zero" the trespassing box */
		trespassingBox = core::aabbox3d<f32>(0, 0, 0, 0, 0, 0);
		/* in case there is no wallbox, return true */
		if (stage->getWallbox().isEmpty()) {
			return true;
		}
		/* check collision between each pair of hurtboxes */
		core::aabbox3d<f32> b1;
		bool intersectFlag = true;
		for (auto iterator = playerToUpdate->getHurtboxCollection().begin();
			iterator != playerToUpdate->getHurtboxCollection().end(); ++iterator) {
			auto mesh = iterator->second;
			auto id = iterator->first;
			b1 = mesh->getTransformedBoundingBox();
			intersectFlag &= b1.isFullInside(stage->getWallbox());
			if (!intersectFlag) {
				trespassingBox = b1;
				return false;
			}
		}
		return intersectFlag;
	}

	/* check collisions with stage wallbox */
	bool FK_SceneViewUpdater::isCharacterInsideComplexStageWallbox(FK_Character* playerToUpdate,
		core::aabbox3d<f32> &trespassingBox) {
		/* "zero" the trespassing box */
		trespassingBox = core::aabbox3d<f32>(0, 0, 0, 0, 0, 0);
		/* check collision between each pair of hurtboxes */
		core::aabbox3d<f32> b1;
		bool intersectFlag = true;
		u32 numberOfPlanes = stage->getNumberofPlanesOfComplexWallBox();
		for (u32 i = 0; i < numberOfPlanes; ++i) {
			core::plane3df testPlane = stage->getComplexWallBoxPlane(i);
			for (auto iterator = playerToUpdate->getHurtboxCollection().begin();
				iterator != playerToUpdate->getHurtboxCollection().end(); ++iterator) {
				auto mesh = iterator->second;
				auto id = iterator->first;
				b1 = mesh->getTransformedBoundingBox();
				core::EIntersectionRelation3D testVal = b1.classifyPlaneRelation(testPlane);
				intersectFlag &= testVal == core::EIntersectionRelation3D::ISREL3D_FRONT;
				if (!intersectFlag) {
					trespassingBox = b1;
					return false;
				}
			}
		}
		return intersectFlag;
	}

	/* check if the player is inside the ringout zone 
	(i.e. the zone in which a strong attack can send the player out of the ring) */
	bool FK_SceneViewUpdater::isCharacterInsideRingoutZone(FK_Character* playerToUpdate){
		if (stage->hasComplexShape()) {
			return isCharacterInsideComplexRingoutZone(playerToUpdate);
		}
		else {
			return isCharacterInsideVanillaRingoutZone(playerToUpdate);
		}
	}

	bool FK_SceneViewUpdater::isCharacterInsideComplexRingoutZone(FK_Character* playerToUpdate) {
		/* check collision between each hurtbox and the rignout zone */
		core::aabbox3d<f32> b1;
		bool intersectFlag = true;
		u32 numberOfPlanes = stage->getNumberofPlanesOfComplexRingoutBox();
		if (numberOfPlanes == 0) {
			return false;
		}
		for (u32 i = 0; i < numberOfPlanes; ++i) {
			core::plane3df testPlane = stage->getComplexRingoutBoxPlane(i);
			for (auto iterator = playerToUpdate->getHurtboxCollection().begin();
				iterator != playerToUpdate->getHurtboxCollection().end(); ++iterator) {
				auto mesh = iterator->second;
				auto id = iterator->first;
				b1 = mesh->getTransformedBoundingBox();
				core::EIntersectionRelation3D testVal = b1.classifyPlaneRelation(testPlane);
				intersectFlag = testVal != core::EIntersectionRelation3D::ISREL3D_FRONT;
				if (intersectFlag) {
					return true;
				}
			}
		}
		return intersectFlag;
	}

	bool FK_SceneViewUpdater::isCharacterInsideVanillaRingoutZone(FK_Character* playerToUpdate) {
		/* in case there is no wallbox, return true */
		if (stage->getRingoutbox().isEmpty()) {
			return false;
		}
		/* check collision between each pair of hurtboxes */
		core::aabbox3d<f32> b1;
		core::aabbox3d<f32> b2 = stage->getRingoutbox();
		bool insideRingoutBoxFlag = true;
		for (auto iterator = playerToUpdate->getHurtboxCollection().begin();
			iterator != playerToUpdate->getHurtboxCollection().end(); ++iterator) {
			auto mesh = iterator->second;
			auto id = iterator->first;
			b1 = mesh->getTransformedBoundingBox();
			insideRingoutBoxFlag &= b1.isFullInside(stage->getRingoutbox());
			// if at least one hurtbox is outside the inner box, start the second round of checks
			if (!insideRingoutBoxFlag)
				break;
		}
		// if the player is outside the ringout box, then is inside the ringout zone
		return !insideRingoutBoxFlag;
	}

	// classify intersection with stage box
	void FK_SceneViewUpdater::classifyStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
		core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
		bool &playerIsCornered) {

		if (stage->hasComplexShape()) {
			classifyComplexStageBoxTrespassing(trespassingBox,
				collisionNormal,ringoutCollisionNormal,collisionDirection,
				playerIsCornered);
		}
		else {
			classifyVanillaStageBoxTrespassing(trespassingBox,
				collisionNormal, ringoutCollisionNormal, collisionDirection,
				playerIsCornered);
		}
	}

	// classify intersection with stage box
	void FK_SceneViewUpdater::classifyVanillaStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
		core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
		bool &playerIsCornered) {
		core::EIntersectionRelation3D northIntersect =
			trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneNorth));
		core::EIntersectionRelation3D southIntersect =
			trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneSouth));
		core::EIntersectionRelation3D eastIntersect =
			trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneEast));
		core::EIntersectionRelation3D westIntersect =
			trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneWest));
		u32 ringoutThroughWallsCount = 0;
		if (northIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT) {
			collisionDirection |= (u32)FK_PlaneDirections::PlaneNorth;
			collisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneNorth).Normal;
			if (!stage->allowsRingout((u32)FK_PlaneDirections::PlaneNorth)) {
				ringoutCollisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneNorth).Normal;
				ringoutThroughWallsCount += 1;
			}
		}
		else if (southIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT) {
			collisionDirection |= (u32)FK_PlaneDirections::PlaneSouth;
			collisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneSouth).Normal;
			if (!stage->allowsRingout((u32)FK_PlaneDirections::PlaneSouth)) {
				ringoutCollisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneSouth).Normal;
				ringoutThroughWallsCount += 1;
			}
		}
		if (westIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT) {
			playerIsCornered = !(player1->isBeingThrown() || player2->isBeingThrown()) && collisionDirection > 0;
			collisionDirection |= (u32)FK_PlaneDirections::PlaneWest;
			collisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneWest).Normal;
			if (!stage->allowsRingout((u32)FK_PlaneDirections::PlaneWest)) {
				ringoutCollisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneWest).Normal;
				ringoutThroughWallsCount += 1;
			}
		}
		else if (eastIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT) {
			playerIsCornered = !(player1->isBeingThrown() || player2->isBeingThrown()) && collisionDirection > 0;
			collisionDirection |= (u32)FK_PlaneDirections::PlaneEast;
			collisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneEast).Normal;
			if (!stage->allowsRingout((u32)FK_PlaneDirections::PlaneEast)) {
				ringoutCollisionNormal += stage->getWallboxPlane(FK_PlaneDirections::PlaneEast).Normal;
				ringoutThroughWallsCount += 1;
			}
		}
		if (playerIsCornered && ringoutThroughWallsCount > 0) {
			collisionDirection = (u32)FK_PlaneDirections::PlaneNone;
		}
	}

	// classify intersection with stage box
	void FK_SceneViewUpdater::classifyComplexStageBoxTrespassing(core::aabbox3d<f32> &trespassingBox,
		core::vector3df &collisionNormal, core::vector3df &ringoutCollisionNormal, u32 &collisionDirection,
		bool &playerIsCornered) {
		bool intersectFlag = true;
		collisionNormal = core::vector3df(0.f, 0.f, 0.f);
		collisionDirection = 0;
		u32 numberOfPlanes = stage->getNumberofPlanesOfComplexWallBox();
		for (u32 i = 0; i < numberOfPlanes; ++i) {
			core::plane3df testPlane = stage->getComplexWallBoxPlane(i);
			core::EIntersectionRelation3D testVal = trespassingBox.classifyPlaneRelation(testPlane);
			intersectFlag = testVal != core::EIntersectionRelation3D::ISREL3D_FRONT;
			if (intersectFlag) {
				u32 collisionDirectionIndex = i;
				if (collisionNormal.getLength() > 0) {
					playerIsCornered = true;
					collisionDirectionIndex *= numberOfPlanes;
				}
				collisionDirection += collisionDirectionIndex;
				if (stage->allowsRingout(collisionDirection)) {
					ringoutCollisionNormal += testPlane.Normal;
				}
				collisionNormal += testPlane.Normal;
			}
		}
		playerIsCornered &= !(player1->isBeingThrown() || player2->isBeingThrown());
	}

	// check if character is cornered
	bool FK_SceneViewUpdater::getIfCharacterIsCornered(FK_Character* character) {
		core::aabbox3d<f32> trespassingBox;
		if (isCharacterInsideStageWallbox(character, trespassingBox)) {
			return false;
		}
		if (player1->isBeingThrown() || player2->isBeingThrown()) {
			return false;
		}
		bool playerIsCornered = false;
		if (stage->hasComplexShape()) {
			u32 numberOfPlanes = stage->getNumberofPlanesOfComplexWallBox();
			bool intersectFlag = false;
			bool multipleIntersection = false;
			for (u32 i = 0; i < numberOfPlanes; ++i) {
				core::plane3df testPlane = stage->getComplexWallBoxPlane(i);
				core::EIntersectionRelation3D testVal = trespassingBox.classifyPlaneRelation(testPlane);
				intersectFlag = testVal != core::EIntersectionRelation3D::ISREL3D_FRONT;
				if (intersectFlag) {
					if (multipleIntersection) {
						playerIsCornered = true;
					}
					else {
						multipleIntersection = true;
					}
				}
			}
		}
		else {
			core::EIntersectionRelation3D northIntersect =
				trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneNorth));
			core::EIntersectionRelation3D southIntersect =
				trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneSouth));
			core::EIntersectionRelation3D eastIntersect =
				trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneEast));
			core::EIntersectionRelation3D westIntersect =
				trespassingBox.classifyPlaneRelation(stage->getWallboxPlane(FK_PlaneDirections::PlaneWest));
			if ((northIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT ||
				southIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT) &&
				(westIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT ||
					eastIntersect != core::EIntersectionRelation3D::ISREL3D_FRONT)) {
				playerIsCornered = true;
			}
		}
		return playerIsCornered;
	}

	/* get if players can collide*/
	// this function was used to try implementing jumps to overcome standing / crouching players, but ended up being a failure :'(
	// keeping it because there is a slim chance that I can adapt it later
	bool FK_SceneViewUpdater::getCharacterCollisionCondition(FK_Character *playerToUpdate, FK_Character *sparringPlayer) {
		return true;
		//return !(playerToUpdate->isJumping() && !sparringPlayer->isAirborne());
	}

	/* update player position*/
	void FK_SceneViewUpdater::updateCharacterPositions(f32 delta_t_s, FK_Character* playerToUpdate, FK_Character* sparringPlayer,
		core::vector3df& shavedMovementStage, 
		core::vector3df& additionalMovement){
		bool updateRightPlayer = !playerToUpdate->isOnLeftSide();
		core::vector3df nodePosition = playerToUpdate->getPosition();
		core::vector3df backupPosition = playerToUpdate->getPosition();
		core::vector3df backupRotation = playerToUpdate->getRotation();
		f32 maximumDistance = 240.0f;
		// stop parallel movement if the distance is already too big
		core::vector3df nodeDistance = (sparringPlayer->getPosition() - nodePosition);
		nodeDistance.Y = 0;
		if (nodeDistance.getLength() >= maximumDistance && !playerToUpdate->getRingoutFlag()){
			if (playerToUpdate->getNextMovement().X < 0){
				core::vector3df nextMovementReplacement = playerToUpdate->getNextMovement();
				nextMovementReplacement.X = 0;
				playerToUpdate->setNextMovement(nextMovementReplacement);
			}
		}
		core::vector3df movementVector(0.f, 0.f, 0.f);
		if (playerToUpdate->getNextMovement().X != 0 || playerToUpdate->getNextMovement().Y != 0 ||
			additionalMovement.X != 0 || additionalMovement.Z != 0){
			core::vector3df nodePosition1 = playerToUpdate->getPosition();
			core::vector3df nodePosition2 = sparringPlayer->getPosition();
			core::vector3df normalizedDelta = (nodePosition2 - nodePosition1);
			normalizedDelta.Y = 0;
			if (normalizedDelta.getLengthSQ() < 1.f) {
				if (!player1->isBeingThrown() || player2->isBeingThrown()) {
					f32 newAngle = playerToUpdate->getRotation().Y;
					normalizedDelta.X = sin(newAngle*core::DEGTORAD);
					normalizedDelta.Z = cos(newAngle*core::DEGTORAD);
					if (!playerToUpdate->isDirectionLocked()) {
						playerToUpdate->setDirectionLock(true, true);
						playerToUpdate->setLockedDirection(normalizedDelta);
						playerToUpdate->setRotation(core::vector3df(0, newAngle, 0));
					}
				}
			}else{
				playerToUpdate->revertDirectionLock();
			}
			//core::vector3df delta_rotation = normalizedDelta.getHorizontalAngle();
			core::vector3df backupRotation = playerToUpdate->getRotation();
			normalizedDelta.normalize();
			if (playerToUpdate->isDirectionLocked()){
				if (playerToUpdate->hasToSetLockedDirection()){
					core::vector3df oldDirection = backupRotation;
					oldDirection.X = 0;
					oldDirection.Z = 0;
					// in case of need, limit the maximum horizontal angle a player can rotate during a move
					if(!(player1->isBeingThrown() || player2->isBeingThrown()) &&
						playerToUpdate->isCurrentMovePartOfAChain() || 
						(playerToUpdate->getCurrentMove() != NULL && playerToUpdate->isJumping())){
						f32 oldAngle = oldDirection.Y;
						f32 newAngle = normalizedDelta.getHorizontalAngle().Y;
						if (newAngle > 180.f) {
							newAngle = newAngle - 360.f;
						}
						if (oldAngle > 180.f) {
							oldAngle = oldAngle - 360.f;
						}
						f32 angularDelta = newAngle - oldAngle;
						if (angularDelta > 180) {
							angularDelta -= 360;
						}
						if (angularDelta < -180) {
							angularDelta += 360;
						}
						f32 angularThreshold = playerToUpdate->getCurrentMove()->getMaximumTrackingAngleOnStartup();
						if (playerToUpdate->isJumping()) {
							angularThreshold = 5.0f;
						}
						if (abs(angularDelta) > angularThreshold) {
							nodePosition2 = nodePosition1;
							newAngle = oldAngle + angularThreshold * angularDelta / abs(angularDelta);
							if (newAngle < 0.f) {
								newAngle += 360.f;
							}
							normalizedDelta.X = sin(newAngle*core::DEGTORAD);
							normalizedDelta.Z = cos(newAngle*core::DEGTORAD);
						}
					}
					playerToUpdate->setLockedDirection(normalizedDelta);
					if (!playerToUpdate->isHitStun()){
						core::vector3df delta_rotation = normalizedDelta.getHorizontalAngle();
						delta_rotation.X = 0;
						delta_rotation.Z = 0;
						playerToUpdate->setRotation(delta_rotation);
					}
				}
				normalizedDelta = playerToUpdate->getLockedDirection();
				//std::cout << "direction lock: " << normalizedDelta.X << " " << normalizedDelta.Z << std::endl;
			}
			else{
				// in case of need, limit the maximum horizontal angle a player can rotate during a move
				if (!(player1->isBeingThrown() || player2->isBeingThrown()) &&
					playerToUpdate->getCurrentMove() != NULL &&
					((playerToUpdate->getCurrentFrame() > playerToUpdate->getCurrentMove()->getStartingFrame() + 1 &&
					playerToUpdate->getCurrentFrame() <= playerToUpdate->getCurrentMove()->getEndingFrame()) ||
					playerToUpdate->isCurrentMovePartOfAChain())) {
					f32 oldAngle = backupRotation.Y;
					f32 newAngle = normalizedDelta.getHorizontalAngle().Y;
					if (newAngle > 180.f) {
						newAngle = newAngle - 360.f;
					}
					if (oldAngle > 180.f) {
						oldAngle = oldAngle - 360.f;
					}
					f32 angularDelta = newAngle - oldAngle;
					if (angularDelta > 180) {
						angularDelta -= 360;
					}
					if (angularDelta < -180) {
						angularDelta += 360;
					}
					f32 angularThreshold = playerToUpdate->getCurrentMove()->getMaximumTrackingAngleDuringMovement() * delta_t_s;
					//std::cout << newAngle << " " << oldAngle << " " << angularDelta << std::endl;
					if (abs(angularDelta) > angularThreshold) {
						nodePosition2 = nodePosition1;
						newAngle = oldAngle + angularThreshold * angularDelta / abs(angularDelta);
						if (newAngle < 0.f) {
							newAngle += 360.f;
						}
						normalizedDelta.X = sin(newAngle*core::DEGTORAD);
						normalizedDelta.Z = cos(newAngle*core::DEGTORAD);
					}
				}

				if (!playerToUpdate->isAirborne()) {
					core::vector3df delta_rotation = normalizedDelta.getHorizontalAngle();
					delta_rotation.X = 0;
					delta_rotation.Z = 0;
					//std::cout << playerToUpdate->getDisplayName() << " " << delta_rotation.Y << std::endl;
					playerToUpdate->setRotation(delta_rotation);
				}
			}
			nodePosition = playerToUpdate->getPosition();
			// Make parallel movement
			nodePosition.X = nodePosition.X + playerToUpdate->getNextMovement().X * normalizedDelta.X;
			nodePosition.Z = nodePosition.Z + playerToUpdate->getNextMovement().X * normalizedDelta.Z;
			// Perform side movement (notice that buttons are arranged such that UP and DOWN keep on working in the same way)
			core::vector3df normal_direction;
			normal_direction.X = normalizedDelta.Z;
			normal_direction.Z = -normalizedDelta.X;
			if (updateRightPlayer && !(player1->isBeingThrown() || player2->isBeingThrown()) && 
				(playerToUpdate->getCurrentMove() == NULL || !playerToUpdate->getCurrentMove()->cantBeTransversalMirrored())){
				normal_direction *= -1;
			}
			nodePosition.X = nodePosition.X + playerToUpdate->getNextMovement().Y * normal_direction.X;
			nodePosition.Z = nodePosition.Z + playerToUpdate->getNextMovement().Y * normal_direction.Z;
			// add additional movement
			nodePosition.X += additionalMovement.X;
			nodePosition.Z += additionalMovement.Z;
			// calculate movement vector
			movementVector = nodePosition - playerToUpdate->getPosition();
		}
		playerToUpdate->setPosition(nodePosition);
		//playerToUpdate->getAnimatedMesh()->updateAbsolutePosition();
		/* roll back if the player intersect*/
		core::vector3df movementVectorForRingout = movementVector;
		bool ignoreVerticalDirection = getIfCharacterIsCornered(sparringPlayer);
		bool characterCollisionCondition = getCharacterCollisionCondition(playerToUpdate, sparringPlayer);
		if (characterCollisionCondition) {
			if (areCharactersColliding(playerToUpdate, sparringPlayer, ignoreVerticalDirection)) {
				core::vector3df currentPosition = playerToUpdate->getPosition();
				core::vector3df newDistance = sparringPlayer->getPosition() - playerToUpdate->getPosition();
				core::vector3df approachingDirection = sparringPlayer->getPosition() - backupPosition;
				newDistance.Y = 0;
				f32 absoluteDistance = newDistance.getLength();
				approachingDirection.Y = 0;
				approachingDirection.normalize();
				f32 deltaToApply = intersectionDistance - absoluteDistance;
				nodePosition.X = nodePosition.X - deltaToApply * approachingDirection.X;
				nodePosition.Z = nodePosition.Z - deltaToApply * approachingDirection.Z;
				movementVector.X -= deltaToApply * approachingDirection.X;
				movementVector.Z -= deltaToApply * approachingDirection.Z;
				playerToUpdate->setPosition(nodePosition);
				//playerToUpdate->setRotation(backupRotation);
				//if (!playerToUpdate->isJumping()) {
				//	playerToUpdate->setNextMovement(core::vector3df(0, 0, playerToUpdate->getNextMovement().Z));
				//}
				backupPosition = nodePosition;
				backupRotation = playerToUpdate->getRotation();
				//nodePosition = backupPosition;
				//std::cout << playerToUpdate->getDisplayName() << " has collided" << std::endl;
				if (playerToUpdate->isRunning() && !playerToUpdate->isCancelingRun()) {
					if (!(playerToUpdate->isImpactRunning() && sparringPlayer->isHitStun())) {
						playerToUpdate->cancelRunning(fk_constants::FK_BufferTimeBeforeRunningEndsMs);
					}
				}
			}
		}
		/* rollback if the player is going out of the wallbox */
		core::aabbox3d<f32> trespassingBox;
		bool insideStageBox = isCharacterInsideStageWallbox(playerToUpdate, trespassingBox);
		bool playerCanSufferRingout = playerToUpdate->canSufferRingOut() && (
			((sparringPlayer->getPosition() - playerToUpdate->getPosition()).getLength() < maximumDistance) ||
			sparringPlayer->getRingoutFlag());
		//playerCanSufferRingout &= playerToUpdate->getPosition().Y > 18;
		
		core::vector3d<f32> collisionNormal(0,0,0);
		core::vector3d<f32> ringoutCollisionNormal(0, 0, 0);
		u32 collisionDirection = (u32)FK_PlaneDirections::PlaneNone;
		bool playerIsCornered = false;
		core::vector3df playerPosition = playerToUpdate->getPosition();
		if (!insideStageBox){
			classifyStageBoxTrespassing(
				trespassingBox, collisionNormal, ringoutCollisionNormal, 
				collisionDirection, playerIsCornered);
		}
		playerToUpdate->setCorneredFlag(playerIsCornered);
		// prepare arrays for computations (ringout & wallbox)
		core::vector3df nodePosition1 = playerToUpdate->getPosition();
		core::vector3df nodePosition2 = sparringPlayer->getPosition();
		core::vector3df normalizedDelta = (nodePosition2 - nodePosition1);
		normalizedDelta.Y = 0;
		normalizedDelta.normalize();
		core::vector3df normal_direction;
		normal_direction.X = normalizedDelta.Z;
		normal_direction.Z = -normalizedDelta.X;
		if (updateRightPlayer){
			normal_direction *= -1;
		}
		// compute X and Z movement
		core::vector3d<f32> movementVectorNormal = movementVector;
		/* compare the movement direction with the position of the walls (that is, find where the wall has been crossed) */
		movementVectorNormal.normalize();
		// make the dot product with the collision normal
		f32 movementDirection = collisionNormal.dotProduct(movementVectorForRingout);
		// if the movement is all in the direction of the wall, set flag that player is hitting the walls instead
		if (movementDirection < -0.95) {
			playerToUpdate->setRingWallCollisionFlag(true);
		}else{
			playerToUpdate->setRingWallCollisionFlag(false);
		}
		// check for ringout and shave movement
		f32 ringoutDirectionThreshold = 0.87;
		core::vector3d<f32> deltaToSubtract = movementVector.dotProduct(collisionNormal)*collisionNormal;
		// if the dot product is smaller than 0, then keep with the usual checks, otherwise stop the procedure
		if (!insideStageBox && movementDirection < 0){
			if (playerCanSufferRingout && !playerToUpdate->getRingoutFlag() && 
				stage->allowsRingout(collisionDirection) &&
				abs(movementDirection) > ringoutDirectionThreshold){
				playerToUpdate->setRingoutFlag(true);
				// remove animator and create a new one, without collisions with objects
				ISceneNodeAnimator* pAnim1 = *(playerToUpdate->getAnimatedMesh()->getAnimators().begin());
				if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE){
					((scene::ISceneNodeAnimatorCollisionResponse*)pAnim1)->setWorld(
						stage->getSkybox()->getTriangleSelector());
				}
				/*if (currentPlayerCamera != NULL){
					smgr->setActiveCamera(currentPlayerCamera);
				}*/
				// temporary - give the "sent-out-player" a "small" velocity boost to "help" the ringout process
				core::vector3d<f32> velocity = playerToUpdate->getVelocityPerSecond();
				velocity.Z += 100;
				/* and add a small boost during jumps to make it easier and more pleasant to see */
				if (!playerToUpdate->isHitStun()) {
					velocity.X += 100 * velocity.X / abs(velocity.X);
				}
				playerToUpdate->setVelocityPerSecond(velocity);
			}
			else if (playerToUpdate->getRingoutFlag()){
				deltaToSubtract = movementVector.dotProduct(ringoutCollisionNormal)*ringoutCollisionNormal;
				playerToUpdate->setPosition(playerToUpdate->getPosition() - deltaToSubtract);
				playerToUpdate->setNextMovement(core::vector3df(0, 0, playerToUpdate->getNextMovement().Z));
				nodePosition = playerToUpdate->getPosition();
			}else{
				if (playerIsCornered){
					nodePosition = backupPosition;
					nodePosition.Y = playerToUpdate->getPosition().Y;
					backupPosition = nodePosition;
					playerToUpdate->setPosition(backupPosition);	
				}
				else{
					playerToUpdate->setPosition(playerToUpdate->getPosition() - deltaToSubtract);
					shavedMovementStage = deltaToSubtract;
					//playerToUpdate->setRotation(backupRotation);
					playerToUpdate->setNextMovement(core::vector3df(0, 0, playerToUpdate->getNextMovement().Z));
					nodePosition = playerToUpdate->getPosition();
				}
			}
		}

		/* perform vertical movement */
		if (playerToUpdate->getNextMovement().Z != 0){
			float newZ = playerToUpdate->getNextMovement().Z;
			nodePosition.Y = nodePosition.Y + playerToUpdate->getNextMovement().Z;
			playerToUpdate->setPosition(nodePosition);
			backupPosition.Y = nodePosition.Y;
			//std::cout << playerToUpdate->getDisplayName() << " vertical position: "<< nodePosition.Y << std::endl;
		}
		/* check again if player intersects and roll back in case of need*/
		bool restoreBackupPosition = false;
		if (characterCollisionCondition) {
			ignoreVerticalDirection = getIfCharacterIsCornered(sparringPlayer);
			if (playerToUpdate->getNextMovement().X > 0 &&
				areCharactersColliding(playerToUpdate, sparringPlayer, ignoreVerticalDirection)) {
				playerToUpdate->setPosition(backupPosition);
				restoreBackupPosition = true;
				//std::cout << playerToUpdate->getDisplayName() << " position reset" << std::endl;
				//playerToUpdate->setRotation(backupRotation);
				//playerToUpdate->setNextMovement(core::vector3df(0, 0, 0));
			}
		}
		if (restoreBackupPosition){
			//playerToUpdate->setVelocityPerSecond(core::vector3df(0,0,0));
			core::vector3d<f32> velocity = playerToUpdate->getVelocityPerSecond();
			velocity.X = velocity.X > 0 ? 0 : velocity.X;
			playerToUpdate->setVelocityPerSecond(velocity);
		}
		playerToUpdate->setNextMovement(core::vector3df(0, 0, 0));
	};

	/* reset camera variables */
	void FK_SceneViewUpdater::resetCameraPosition() {
		lastCameraPosition = core::vector3df(0.f, 0.f, 0.f);
		lastCameraAngle = core::vector3df(0.f, -1.f, 0.f);
	}

	/* update the position of the camera */
	void FK_SceneViewUpdater::update(f32 delta_t_s, f32 additionalCameraZoom){
		camera->setFOV(battleFOVAngle);
		//updatePlayerCameras();
		/* update camera position */
		//s32 testDirectionDeterminant = oldDirectionDeterminant;
		FK_Character *leftPlayer = getLeftSidePlayer();
		FK_Character *rightPlayer = getRightSidePlayer();
		/* update bullet intersection with stage box */
		updateBulletStageInteractions();
		// check if one of the player is being thrown
		if (!throwStart && (player1->isBeingThrown() || player2->isBeingThrown())){
			throwStart = true;
			setupThrow();
		}
		else if (throwStart && !(player1->isBeingThrown() || player2->isBeingThrown())){
			throwStart = false;
		}
		// check for cinematic attacks
		isUpdatingCinematic = false;
		if (leftPlayer->getCurrentMove() != NULL &&
			leftPlayer->getCurrentMove()->hasActiveMoveCinematic((u32)leftPlayer->getCurrentFrame())) {
			updateCinematic(leftPlayer, rightPlayer);
			isUpdatingCinematic = true;
		}
		else if (rightPlayer->getCurrentMove() != NULL &&
			rightPlayer->getCurrentMove()->hasActiveMoveCinematic((u32)rightPlayer->getCurrentFrame())) {
			updateCinematic(rightPlayer, leftPlayer);
			isUpdatingCinematic = true;
		}
		// check for throws
		else if (player1->isBeingThrown() || player2->isBeingThrown()) {
			updateThrow();
		}
		else{
			//// set the proper boolean for each player
			//if (!(player1->isAirborne() || player2->isAirborne())){
			//	leftPlayer->setOnLeftSide(true);
			//	rightPlayer->setOnLeftSide(false);
			//}
			// set the proper boolean for each player
			if (!leftPlayer->isAirborne()) {
				if (!(rightPlayer->isAirborne() && leftPlayer->getCurrentMove() != NULL)) {
					leftPlayer->setOnLeftSide(true);
				}
			}
			if (!rightPlayer->isAirborne()) {
				if (!(leftPlayer->isAirborne() && rightPlayer->getCurrentMove() != NULL)) {
					rightPlayer->setOnLeftSide(false);
				}
			}
			core::vector3df nodePosition1 = leftPlayer->getPosition();
			core::vector3df nodePosition2 = rightPlayer->getPosition();
			core::vector3df cameraPosition = camera->getPosition();
			core::vector3df fightersMidPoint;
			core::vector3df fightersDelta = nodePosition1 - nodePosition2;
			if (fightersDelta.getLengthSQ() < 1.f) {
				fightersMidPoint = nodePosition1;
				cameraPosition = lastCameraPosition;
				camera->setTarget(fightersMidPoint);
				camera->setPosition(lastCameraPosition);
			}
			else {
				fightersMidPoint.X = (nodePosition2.X + nodePosition1.X) / 2;
				fightersMidPoint.Z = (nodePosition2.Z + nodePosition1.Z) / 2;
				/* the Y position of the camera is not influenced by the position of the fighters */
				fightersMidPoint.Y = (nodePosition2.Y + nodePosition1.Y) / 2;
				fightersMidPoint.Y = fightersMidPoint.Y < 0 ? 0 : fightersMidPoint.Y;
				camera->setTarget(fightersMidPoint);
				core::vector3df normal_direction;
				//if (fightersDelta.getLengthSQ() > 1e-3) {
				//	lastNonZeroCharacterPositionDelta = fightersDelta;
				//}
				//else {
				//	fightersDelta = lastNonZeroCharacterPositionDelta;
				//}
				/*copy the content of the distance between the fighters to normalize it without changing the original array*/
				core::vector3df normalizedDelta = fightersDelta;
				/* get only the planar distance for calculating the position of the camera */
				normalizedDelta.Y = 0;
				normalizedDelta.normalize();
				normal_direction.X = -normalizedDelta.Z;
				normal_direction.Z = normalizedDelta.X;
				float fighter_distance = sqrt(fightersDelta.X*fightersDelta.X + fightersDelta.Z*fightersDelta.Z);
				float cameraDistance = fighter_distance / distanceScaleFactorXZ * baseCameraDistance;
				if (cameraDistance < nearestCameraDistance)
					cameraDistance = nearestCameraDistance;
				if (cameraDistance > farthestCameraDistance)
					cameraDistance = farthestCameraDistance;
				cameraDistance -= additionalCameraZoom;
				cameraPosition.X = fightersMidPoint.X + cameraDistance * normal_direction.X;
				cameraPosition.Z = fightersMidPoint.Z + cameraDistance * normal_direction.Z;
				cameraPosition.Y = baseCameraYPosition + fightersMidPoint.Y - baseCameraTargetOffsetY;
				lastTargetY = fightersMidPoint.Y;
				f32 cameraMovementThreshold = 40.0f;
				core::vector3df currentCameraAngle = (fightersMidPoint - cameraPosition).getHorizontalAngle();
				currentCameraAngle.X = 0.f;
				currentCameraAngle.Z = 0.f;
				bool angularCondition = false;
				lastCameraAngle = currentCameraAngle;
				bool distanceCondition = lastCameraPosition != core::vector3df(0, 0, 0) &&
					cameraPosition.getDistanceFrom(lastCameraPosition) > cameraMovementThreshold;
				if (lastCameraPosition != core::vector3df(0, 0, 0) && (angularCondition || distanceCondition)) {
					cameraPosition.X = lastCameraPosition.X;
					cameraPosition.Z = lastCameraPosition.Z;
					lastCameraPosition = core::vector3df(0, 0, 0);
				}
				else {
					lastCameraPosition = cameraPosition;
				}
				lastCameraAngle = currentCameraAngle;
				camera->setPosition(cameraPosition);
			}
		}
		/* now, update nodes according to the last camera position */
		/* update left player first */
		core::vector3df shavedMovementStage(0, 0, 0);
		core::vector3df additionalMovement(0, 0, 0);
		bool basicUpdatePerformed = false;
		u32 maximumNumberOfIterations = 32;
		u32 iterationCounter = 0;
		/*bool charactersColliding = areCharactersColliding(leftPlayer, rightPlayer);
		bool collisionCondition = getCharacterCollisionCondition(leftPlayer, rightPlayer) && getCharacterCollisionCondition(rightPlayer, leftPlayer);*/
		while (!basicUpdatePerformed){
			if (player1->getNextMovement() != core::vector3df(0, 0, 0) || additionalMovement != core::vector3df(0, 0, 0)){
				//currentPlayerCamera = player1->isOnLeftSide() ? cameraPlayer2 : cameraPlayer1;
				updateCharacterPositions(delta_t_s, player1, player2, 
					shavedMovementStage, additionalMovement);
			};
			if (player1->isBeingThrown() || player2->isBeingThrown()){
				additionalMovement = -shavedMovementStage;
			}
			shavedMovementStage = core::vector3df(0, 0, 0);
			/* then, update right player*/
			if (player2->getNextMovement() != core::vector3df(0, 0, 0) || additionalMovement != core::vector3df(0, 0, 0)){
				//currentPlayerCamera = player2->isOnLeftSide() ? cameraPlayer2 : cameraPlayer1;
				updateCharacterPositions(delta_t_s, player2, player1, 
					shavedMovementStage, additionalMovement);
			};
			if (player1->isBeingThrown() || player2->isBeingThrown()){
				additionalMovement = -shavedMovementStage;
			}
			shavedMovementStage = core::vector3df(0, 0, 0);
			if (iterationCounter >= maximumNumberOfIterations || (additionalMovement.X == 0 && additionalMovement.Z == 0)){
				basicUpdatePerformed = true;
			}
			iterationCounter += 1;
			//std::cout << "iteration no. " << iterationCounter << std::endl;
		}
		// update position as a reference to the ringout zone
		// left player
		bool ringoutZoneFlag = isCharacterInsideRingoutZone(leftPlayer);
		leftPlayer->setInStageRingoutZoneFlag(ringoutZoneFlag);
		// right player
		ringoutZoneFlag = isCharacterInsideRingoutZone(rightPlayer);
		rightPlayer->setInStageRingoutZoneFlag(ringoutZoneFlag);
		/* reset the movement array */
		leftPlayer->resetNextMovement();
		rightPlayer->resetNextMovement();
	};

	/* update camera position for continue*/
	void FK_SceneViewUpdater::updateContinue(int playerId){
		smgr->setActiveCamera(camera);
		if (playerId != 0){
			camera->setFOV(defaultFOVAngle);
			bool player1IsOnFocus = playerId == -1;
			FK_Character *targetPlayer = player1IsOnFocus ? player1 : player2;
			core::vector3df cameraPosition = targetPlayer->getPosition();
			core::vector3df playerDirection = targetPlayer->getRotation();
			f32 cameraDistanceFromTarget = 20.0;
			f32 offsetAngleForContinueCamera = 0.0;
			f32 angle = playerDirection.Y + offsetAngleForContinueCamera;
			f32 cameraOffsetYPositionFromTarget = 45.0f;
			cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromTarget;
			cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromTarget;
			cameraPosition.Y += cameraOffsetYPositionFromTarget;
			//cameraPosition.Z 
			camera->setPosition(cameraPosition);
			camera->setTarget(targetPlayer->getPosition());
		}
	}
	/* update camera position for player victory*/
	void FK_SceneViewUpdater::updateVictory(int winnerId){
		smgr->setActiveCamera(camera);
		if (winnerId != 0){
			camera->setFOV(defaultFOVAngle);
			bool player1HasWon = winnerId == -1;
			FK_Character *targetPlayer = player1HasWon ? player1 : player2;
			core::vector3df cameraPosition = targetPlayer->getPosition();
			core::vector3df playerDirection = targetPlayer->getRotation();
			f32 cameraDistanceFromWinner = 60.0;
			f32 offsetAngleForVictoryCamera = 30.0;
			f32 angle = playerDirection.Y + offsetAngleForVictoryCamera;
			f32 cameraOffsetYPositionFromWinner = 5.0f;
			cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromWinner;
			cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromWinner;
			cameraPosition.Y += cameraOffsetYPositionFromWinner;
			//cameraPosition.Z 
			camera->setPosition(cameraPosition);
			camera->setTarget(targetPlayer->getPosition());
		}
	}

	/* align characters */
	void FK_SceneViewUpdater::alignCharacters(FK_Character* targetPlayer){
		FK_Character *leftPlayer = getLeftSidePlayer();
		FK_Character *rightPlayer = getRightSidePlayer();
		core::vector3df normalizedDelta = rightPlayer->getPosition() - leftPlayer->getPosition();
		if (!targetPlayer->isOnLeftSide()){
			normalizedDelta = leftPlayer->getPosition() - rightPlayer->getPosition();
		}
		normalizedDelta.Y = 0;
		normalizedDelta.normalize();
		core::vector3df angle = normalizedDelta.getHorizontalAngle();
		targetPlayer->setRotation(normalizedDelta.getHorizontalAngle());
	}

	void FK_SceneViewUpdater::alignPlayerTowardsPlayer(FK_Character* playerToAlign, FK_Character* referencePlayer){
		core::vector3df normalizedDelta = referencePlayer->getPosition() - playerToAlign->getPosition();
		normalizedDelta.Y = 0;
		normalizedDelta.normalize();
		core::vector3df angle = normalizedDelta.getHorizontalAngle();
		playerToAlign->setRotation(normalizedDelta.getHorizontalAngle());
	}

	/* setup throw total distance */
	void FK_SceneViewUpdater::setupThrowTotalDistance(FK_Character* throwingPlayer, f32& basicDistance, 
		core::vector3df& totalDistanceThrowingPlayer, core::vector3df& totalDistanceThrownPlayer){
		basicDistance = throwingPlayer->getCurrentMove()->getThrowStartingDistance();
		u32 startFrame = throwingPlayer->getCurrentMove()->getThrowReactionAnimationPose().getStartingFrame();
		u32 endFrame = throwingPlayer->getCurrentMove()->getThrowReactionAnimationPose().getEndingFrame();
		for (u32 i = startFrame; i < endFrame; ++i){
			totalDistanceThrownPlayer += throwingPlayer->getCurrentMove()->getThrowTargetMovementAtFrame(i);
		}
		startFrame = throwingPlayer->getCurrentMove()->getStartingFrame();
		endFrame = throwingPlayer->getCurrentMove()->getEndingFrame();
		for (u32 i = startFrame; i < endFrame; ++i){
			totalDistanceThrowingPlayer += throwingPlayer->getCurrentMove()->getMovementAtFrame(i);
		}
	}

	/* setup throw starting position */
	void FK_SceneViewUpdater::setupThrow(){
		/* get distance to apply */
		f32 startingDistance = 0.f;
		core::vector3d<f32> totalDistanceMovementPl1(0, 0, 0);
		core::vector3d<f32> totalDistanceMovementPl2(0, 0, 0);
		player1->getAnimatedMesh()->updateAbsolutePosition();
		player2->getAnimatedMesh()->updateAbsolutePosition();
		if (player1->isThrowingOpponent()){
			setupThrowTotalDistance(player1, startingDistance, totalDistanceMovementPl1, totalDistanceMovementPl2);
		}
		else{
			setupThrowTotalDistance(player2, startingDistance, totalDistanceMovementPl2, totalDistanceMovementPl1);
		}
		if (startingDistance < 0.f){
			return;
		}
		/* get distance between characters */
		FK_Character *leftPlayer = getLeftSidePlayer();
		FK_Character *rightPlayer = getRightSidePlayer();
		leftPlayer->resetNextMovement();
		rightPlayer->resetNextMovement();
		leftPlayer->setVelocityPerSecond(core::vector3df(0, 0, 0));
		rightPlayer->setVelocityPerSecond(core::vector3df(0, 0, 0));
		core::vector3df normalizedDelta = rightPlayer->getPosition() - leftPlayer->getPosition();
		normalizedDelta.Y = 0;
		f32 distance = normalizedDelta.getLength();
		f32 deltaDistance = startingDistance - distance;
		f32 distanceToApply = deltaDistance / 2;
		normalizedDelta.normalize();
		// Make parallel movement
		core::vector3df nodePosition1 = leftPlayer->getPosition();
		nodePosition1.X = nodePosition1.X - distanceToApply * normalizedDelta.X;
		nodePosition1.Z = nodePosition1.Z - distanceToApply * normalizedDelta.Z;
		core::vector3df nodePosition2 = rightPlayer->getPosition();
		nodePosition2.X = nodePosition2.X + distanceToApply * normalizedDelta.X;
		nodePosition2.Z = nodePosition2.Z + distanceToApply * normalizedDelta.Z;
		leftPlayer->setPosition(nodePosition1);
		rightPlayer->setPosition(nodePosition2);
		alignPlayerTowardsPlayer(leftPlayer, rightPlayer);
		alignPlayerTowardsPlayer(rightPlayer, leftPlayer);
		if (leftPlayer->isBeingThrown()){
			leftPlayer->setDirectionLock(true);
		}
		else if (rightPlayer->isBeingThrown()){
			rightPlayer->setDirectionLock(true);
		}
	}
	/* update camera position for cinematic move*/
	void FK_SceneViewUpdater::updateCinematic(FK_Character* attackingPlayer, FK_Character* targetPlayer){
		camera->setFOV(defaultFOVAngle);
		core::vector3df cameraPosition = attackingPlayer->getPosition();
		core::vector3df playerDirection = attackingPlayer->getRotation() + 180.f;
		/*f32 cameraDistanceFromPlayer = 75.f;
		f32 cameraYAngle = 60.f;
		f32 offsetAngleForThrowCamera = !leftPlayerIsPerforming ? cameraYAngle : -cameraYAngle;
		f32 angle = playerDirection.Y + offsetAngleForThrowCamera;
		f32 cameraOffsetYPositionFromThrown = 12.f;*/
		f32 currentFrame = attackingPlayer->getAnimatedMesh()->getFrameNr();
		s32 firstFrame = (s32)std::floor(currentFrame);
		s32 lastFrame = (s32)std::ceil(currentFrame);
		f32 delta = currentFrame - (f32)firstFrame;
		FK_Move::FK_MoveCinematic cinematic = attackingPlayer->getCurrentMove()->getCinematic();
		FK_Move::FK_ThrowCameraSettings previousFrame = 
			cinematic.getCurrentCameraSetting(firstFrame);
		FK_Move::FK_ThrowCameraSettings nextFrame =
			cinematic.getCurrentCameraSetting(lastFrame);
		FK_Move::FK_ThrowCameraSettings cameraSettings;
		// use character from current frame as a target
		if (nextFrame.directCut) {
			cameraSettings = cinematic.getCurrentCameraSetting(attackingPlayer->getCurrentFrame());
		}
		else {
			cameraSettings.targetCharacter = cinematic.getCurrentCameraSetting(attackingPlayer->getCurrentFrame()).targetCharacter;
			// interpolate all other quantities from the last two frames		
			cameraSettings.planarDistanceFromMiddlePoint = previousFrame.planarDistanceFromMiddlePoint +
				(nextFrame.planarDistanceFromMiddlePoint - previousFrame.planarDistanceFromMiddlePoint) *
				delta;
			cameraSettings.offsetPositionYFromMiddlePoint = previousFrame.offsetPositionYFromMiddlePoint +
				(nextFrame.offsetPositionYFromMiddlePoint - previousFrame.offsetPositionYFromMiddlePoint) *
				delta;
			cameraSettings.rotationAngleYFromCameraPlane = previousFrame.rotationAngleYFromCameraPlane +
				(nextFrame.rotationAngleYFromCameraPlane - previousFrame.rotationAngleYFromCameraPlane) *
				delta;
			cameraSettings.targetYOffset = previousFrame.targetYOffset +
				(nextFrame.targetYOffset - previousFrame.targetYOffset) *
				delta;
		}
		FK_Character* cameraTarget = cameraSettings.targetCharacter > 0 ? attackingPlayer : targetPlayer;
		f32 cameraDistanceFromPlayer = cameraSettings.planarDistanceFromMiddlePoint;
		f32 cameraYAngle = cameraSettings.rotationAngleYFromCameraPlane;
		f32 offsetAngleForThrowCamera = !attackingPlayer->isOnLeftSide() ? cameraYAngle : -cameraYAngle;
		f32 angle = playerDirection.Y + offsetAngleForThrowCamera;
		f32 cameraOffsetYPositionFromThrown = cameraSettings.offsetPositionYFromMiddlePoint;
		cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Y += cameraOffsetYPositionFromThrown;
		//cameraPosition.Z 
		camera->setPosition(cameraPosition);
		camera->setTarget(cameraTarget->getPosition() + core::vector3df(0.f,
			cameraSettings.targetYOffset,
			0.f)
		);
		resetCameraPosition();
	}

	/* update camera position for throws*/
	void FK_SceneViewUpdater::updateThrow(){
		camera->setFOV(defaultFOVAngle);
		FK_Character* leftPlayer = player1->isOnLeftSide() ? player1 : player2;
		FK_Character* rightPlayer = player1->isOnLeftSide() ? player2 : player1;
		bool leftPlayerIsThrown = leftPlayer->isBeingThrown();
		FK_Character *targetPlayer = leftPlayerIsThrown ? leftPlayer : rightPlayer;
		FK_Character *attackingPlayer = leftPlayerIsThrown ? rightPlayer : leftPlayer;
		core::vector3df cameraPosition = targetPlayer->getPosition();
		// TO FIX!
		core::vector3df playerDirection = attackingPlayer->getRotation() + 180.0f;// (attackingPlayer->getPosition() - targetPlayer->getPosition()).getHorizontalAngle();
		//std::cout << "TG: " << targetPlayer->getRotation().X << "-" << targetPlayer->getRotation().Y << "-" << targetPlayer->getRotation().Z << "-"<<std::endl;
		//std::cout << "AT: " << attackingPlayer->getRotation().X << "-" << attackingPlayer->getRotation().Y << "-" << attackingPlayer->getRotation().Z << "-" << std::endl;
		FK_Move tmove;
		f32 cameraDistanceFromPlayer = tmove.getThrowCameraSettings().planarDistanceFromMiddlePoint;
		f32 cameraYAngle = tmove.getThrowCameraSettings().rotationAngleYFromCameraPlane;
		f32 offsetAngleForThrowCamera = leftPlayerIsThrown ? cameraYAngle : -cameraYAngle;
		f32 angle = playerDirection.Y + offsetAngleForThrowCamera;
		f32 cameraOffsetYPositionFromThrown = tmove.getThrowCameraSettings().offsetPositionYFromMiddlePoint;
		if (attackingPlayer->getCurrentMove() != NULL){
			cameraDistanceFromPlayer = attackingPlayer->getCurrentMove()->getThrowCameraSettings().planarDistanceFromMiddlePoint;
			cameraYAngle = attackingPlayer->getCurrentMove()->getThrowCameraSettings().rotationAngleYFromCameraPlane;
			offsetAngleForThrowCamera = leftPlayerIsThrown ? cameraYAngle : -cameraYAngle;
			angle = playerDirection.Y + offsetAngleForThrowCamera;
			cameraOffsetYPositionFromThrown = attackingPlayer->getCurrentMove()->getThrowCameraSettings().offsetPositionYFromMiddlePoint;
		}
		cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Y += cameraOffsetYPositionFromThrown;
		//cameraPosition.Z 
		camera->setPosition(cameraPosition);
		camera->setTarget(targetPlayer->getPosition());
		resetCameraPosition();
	}
	/* update camera position for player intro*/
	void FK_SceneViewUpdater::updateIntro(int playerId){
		smgr->setActiveCamera(camera);
		camera->setFOV(defaultFOVAngle);
		bool player1Intro = playerId == -1;
		FK_Character *targetPlayer = player1Intro ? player1 : player2;
		core::vector3df cameraPosition = targetPlayer->getPosition();
		core::vector3df playerDirection = targetPlayer->getRotation();
		f32 cameraDistanceFromPlayer = 70.0;
		f32 offsetAngleForVictoryCamera = player1Intro? 30.0 : -30.0;
		f32 angle = playerDirection.Y + offsetAngleForVictoryCamera;
		f32 cameraOffsetYPositionFromWinner = 5.0f;
		cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Y += cameraOffsetYPositionFromWinner;
		//cameraPosition.Z 
		camera->setPosition(cameraPosition);
		camera->setTarget(targetPlayer->getPosition());
	}
	/* update camera position for arena introduction - return false if the screen pan has been finished */
	bool FK_SceneViewUpdater::updateStageViewIntro(f32 delta_t_s){
		smgr->setActiveCamera(camera);
		camera->setFOV(defaultFOVAngle);
		core::vector3df cameraPosition(0, 60, 0);
		camera->setPosition(cameraPosition);
		f32 totalPanTime = 6.0f;
		f32 extraTime = 2.0f;
		f32 totalAngularPan = 360.0;
		f32 targetDistance = 150.0f;
		f32 cameraAngle = totalAngularPan * timeCounter / totalPanTime;
		if (cameraAngle > totalAngularPan) cameraAngle = totalAngularPan;
		timeCounter += delta_t_s;
		core::vector3df target = camera->getPosition();
		target.X += targetDistance * cos(cameraAngle * core::DEGTORAD);
		target.Z -= targetDistance * sin(cameraAngle * core::DEGTORAD);
		f32 firstPanoramicRound = 45.0;
		if (cameraAngle > firstPanoramicRound) {
			target.Y = target.Y - cameraPosition.Y * 
				(cameraAngle - firstPanoramicRound) / (totalAngularPan - firstPanoramicRound);
			if (timeCounter > totalPanTime){
				target.Y = 0;
				cameraPosition.Y += targetDistance * (timeCounter - totalPanTime) / extraTime;
				camera->setPosition(cameraPosition);
			}

		}
		//std::cout << target.X << " " << target.Y << " " << target.Z << std::endl;
		camera->setTarget(target);
		if (timeCounter > totalPanTime + extraTime){
			timeCounter = 0;
			return false;
		}
		return true;
	}
};
