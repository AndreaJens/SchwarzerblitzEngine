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

#include<algorithm>
#include"FK_Character.h"
#include"FK_MoveFileParser.h"
#include"FK_BoneFileParser.h"
#include"ExternalAddons.h"
#include<iostream>
#include<sstream>

namespace fk_engine{

	// Setup
	FK_Character::FK_Character(){
		mesh = 0;
		animatedMesh = 0;
		hurtboxCollection.clear();
		hitboxCollection.clear();
		life = 1;
		maxLife = life;
		damageScalingMultiplier = 1.f;
		lifeScalingFactor = 1.f;
		walkingSpeed = 80.0f;
		runningSpeed = 200.0f;
		meshScale = 1.0f;
		jumpSpeed = 10;
		weight = 100;
		animationTransitionTime = 0.1f;
		animationFrameRate = fk_constants::FK_BasicAnimationRate;
		lastFrame = -1; //this variable is used to check for hitbox activation and movement
		lastFrameForObjectUpdate = -1; // this variable is instead used for updating object toggles
		name = std::string();
		displayName = std::string();
		wdisplayName = std::wstring();
		isLeftPlayer = true;
		basicAnimations.clear();
		currentMove = NULL;
		storageMove = NULL;
		//nextMove = NULL;
		nextMove.clear();
		lastMoveNoHitboxFlag = false;
		directionLock = false;
		originalDirectionLock = false;
		safetyDirectionLock = false;
		lockedDirection = core::vector3df(0, 0, 0);
		moveIsPartOfAChain = false;
		hitStunFlag = false;
		hitStunReferenceTime = 0;
		hitStunDuration = 0;
		testGravity = -550.0f;
		jumpGravity = -200.0f;
		guardFlag = false;
		delayAfterMoveTimeMs = 0; //set 300 ms of delay before a new move can be inputed
		delayMovementAfterMoveTimeMs = 0;
		delayAfterMoveTimeCounterMs = 0; // set the time counter to zero
		//reset trigger counters
		triggerCounters = fk_constants::FK_MaxTriggerCounterNumber;
		triggerComboedFlag = false;
		triggerFlag = true;
		triggerGuardDurationS = -1;
		currentStance = FK_Stance_Type::GroundStance;
		hasRotatedOnTheGround = false;
		groundRotationDirection = FK_GroundRotationDirection::NoRotation;
		setBackfaceCulling = false;
		refreshCollisionEllipsoid = false;
		wasCollisionEllipsoidRefreshed = false;
		introQuotes = std::map<std::string, std::vector<std::wstring> >();
		roundWinQuotes = std::map<std::string, std::vector<std::wstring> >();
		matchWinQuotes = std::map<std::string, std::vector<std::wstring> >();
		// reset mesh array
		meshFileNames.clear();
		// reset move array
		movesCollection.clear();
		//currentMoveFollowupCollection.clear();
		m_thread_characterLoaded = false;
		isThrown = false;
		isThrowing = false;
		positionAtPreviousFrame = core::vector3d<f32>(0, 0, 0);
		canBeSentOutOfRing = false;
		// ring out flag
		isRingOut = false;
		isInStageRingoutZone = false;
		// maximum roll counter (when grounded)
		maximumRollingTimeMs = 500;
		rollingTimeCounterMs = 0;
		hitWhileGrounded = false;
		// jump
		jumpingFlag = false;
		storedJumpingAnimation = FK_BasicPose_Type::JumpingAnimation;
		collisionFlag = false;
		// special stance dictionary
		specialStanceDictionary[FK_Stance_Type::SpecialStance1] = "Special Stance 1";
		specialStanceDictionary[FK_Stance_Type::SpecialStance2] = "Special Stance 2";
		specialStanceDictionary[FK_Stance_Type::SpecialStance3] = "Special Stance 3";
		specialStanceDictionary[FK_Stance_Type::SpecialStance4] = "Special Stance 4";
		specialStanceDictionary[FK_Stance_Type::SpecialStance5] = "Special Stance 5";
		// throw reactions
		targetThrowToApply.clear();
		// AI archetype
		AIarchetype = FK_AIArchetypes::Aggressor;
		// impact point for ringout
		positionWhenHitByImpactAttack = core::vector3df(0, 0, 0);
		runningBufferTimerMs = -1;
		runningBufferTimerMsTarget = -1;
		/* impact cancel check */
		canCheckForImpactCancelInput = false;
		hasUsedImpactCancelInCurrentCombo = false;
		impactCancelTimerCounter = -1;
		impactCancelShaderEffectTimer = -1;
		/* air attack */
		isPerformingAirAttack = false;
		hasTouchedGround = false;
		/* damage multipliers */
		receivedPhysicalDamageMultiplier = 1.f;
		receivedBulletDamageMultiplier = 1.f;
		/* tracking flag */
		opponentTrackingFlag = false;
		movesetTag = std::string();
		/* pickable object id*/
		pickableObjectNameId = std::string();
		isPickingObjectFlag = false;
		/* sound timer */
		hitSoundTimer = 0;
		/* check if transforming*/
		isTransforming = false;
		resetTransitionTime = false;
		throwProcessingFlag = false;
		isInBaseForm = true;
		/* special effects */
		specialEffectArmor = false;
		specialEffectArmorTimeCounterMS = 0;
		isMovingAgainstWalls = false;
		corneredFlag = false;
		// pushback
		pushbackDuration = -1;
		lastMoveReceivedDamageId = 0;
		pushbackVelocity = core::vector3df(-350.f, 0.f, 0.f);
		// trigger guard condition
		triggerButtonPressedBeforeHitstun = false;
		isSidestepping = false;
		sidestepCounter = 0;
	};
	// initializer with values 
	FK_Character::FK_Character(FK_DatabaseAccessor newDatabaseAccessor, std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr,	core::vector3df startingPosition, core::vector3df startingRotation, 
		int outfit_id, f32 maxLifeScale, std::string newMovesetTag, std::string newAdditionalObjectTag,
		bool loadAllAnimations) : FK_Character(){
		setup(newDatabaseAccessor, newFileName, newDirectory, newCommonDirectory,
			smgr, startingPosition, startingRotation,
			outfit_id, maxLifeScale, newMovesetTag, newAdditionalObjectTag, loadAllAnimations);
	}
	// independent method for setting up a character (used in multi-thread)
	void FK_Character::setup(FK_DatabaseAccessor newDatabaseAccessor, std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr, core::vector3df startingPosition, core::vector3df startingRotation,
		std::string outfitDirectory, f32 maxLifeScale, 
		std::string newMovesetTag, std::string newAdditionalObjectsTag,
		bool loadAllAnimations){
		loadBasicVariables(newDatabaseAccessor, newFileName, newDirectory, newCommonDirectory, smgr, 
			outfitDirectory, maxLifeScale, newMovesetTag,
			true, true);
		loadMeshsAndEffects(startingPosition, startingRotation,
			newAdditionalObjectsTag,
			true, true, loadAllAnimations, loadAllAnimations);
		if (loadAllAnimations){
			loadTextureVariablesAndTextures();
		}
	}

	// independent method for setting up a character (used in multi-thread)
	void FK_Character::setup(FK_DatabaseAccessor newDatabaseAccessor, 
		std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr, core::vector3df startingPosition, core::vector3df startingRotation,
		s32 outfitId, f32 maxLifeScale, 
		std::string newMovesetTag, std::string newAdditionalObjectsTag,
		bool loadAllAnimations){
		loadBasicVariables(newDatabaseAccessor, newFileName, newDirectory, 
			newCommonDirectory, smgr, outfitId, maxLifeScale, newMovesetTag,
			true, true);
		loadMeshsAndEffects(startingPosition, startingRotation, 
			newAdditionalObjectsTag,
			true, true, loadAllAnimations, loadAllAnimations);
		if (loadAllAnimations){
			loadTextureVariablesAndTextures();
		}
	}
	// independent method for setting up a character (used in multi-thread)
	void FK_Character::loadBasicVariables(FK_DatabaseAccessor newDatabaseAccessor, 
		std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr, int outfit_id, f32 maxLifeScale, 
		std::string newMovesetTag, 
		bool loadMoves, bool loadQuotes){
		databaseAccessor = newDatabaseAccessor;
		setDatabaseDrivenVariables();
		characterFileName = newFileName;
		characterDirectory = newDirectory;
		lifeScalingFactor = maxLifeScale;
		baseForm = FK_Move::FK_TransformationMove();
		/* parse the character file*/
		parseCharacterFile(characterDirectory + characterFileName);
		/* setup life */
		setLifeToMax();
		/* load all available costumes */
		availableCharacterOutfits.clear();
		parseMeshFiles();
		/* load mesh */
		if ((u32)outfit_id >= availableCharacterOutfits.size()){
			outfit_id = availableCharacterOutfits.size() - 1;
		}
		outfitId = outfit_id;
		smgr_reference = smgr;
		/* set animation directory */
		commonDirectory = newCommonDirectory;
		animationDirectory = characterDirectory + "animations\\";
		sharedAnimationDirectory = commonDirectory + "animations\\";
		if (loadQuotes){
			/* load quotes */
			loadIntroQuotes();
			loadWinQuotes();
		}
		movesetTag = newMovesetTag;
		if (loadMoves){
			/* add tag to movefilename, if any*/
			if (!newMovesetTag.empty()){
				moveFileName = newMovesetTag + moveFileName;
			}
			/* parse the move file list after having determined the filename*/
			parseMoves();
			/* add common moves to the set */
			loadCommonMoves();
			/* sort moves so that the longest is the first */
			std::sort(movesCollection.begin(), movesCollection.end());
			std::reverse(movesCollection.begin(), movesCollection.end());
		}
		
	}

	// independent method for setting up a character (used in multi-thread)
	void FK_Character::loadBasicVariables(FK_DatabaseAccessor newDatabaseAccessor, std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr, std::string outfitDirectory, f32 maxLifeScale, std::string newMovesetTag,
		bool loadMoves, bool loadQuotes){
		databaseAccessor = newDatabaseAccessor;
		setDatabaseDrivenVariables();
		characterFileName = newFileName;
		characterDirectory = newDirectory;
		lifeScalingFactor = maxLifeScale;
		/* parse the character file*/
		parseCharacterFile(characterDirectory + characterFileName);
		/* setup life */
		setLifeToMax();
		/* load all available costumes */
		availableCharacterOutfits.clear();
		parseMeshFiles(outfitDirectory);
		/* load mesh */
		s32 outfit_id = 0;
		for each(FK_CharacterOutfit outfit in availableCharacterOutfits){
			if (outfitDirectory == outfit.outfitDirectory){
				outfit_id = outfit.outfitId;
			}
		}
		if ((u32)outfit_id >= availableCharacterOutfits.size()){
			outfit_id = availableCharacterOutfits.size() - 1;
		}
		outfitId = outfit_id;
		smgr_reference = smgr;
		/* set animation directory */
		commonDirectory = newCommonDirectory;
		animationDirectory = characterDirectory + "animations\\";
		sharedAnimationDirectory = commonDirectory + "animations\\";
		if (loadQuotes){
			/* load quotes */
			loadIntroQuotes();
			loadWinQuotes();
		}
		movesetTag = newMovesetTag;
		if (loadMoves){
			/* add tag to movefilename, if any*/
			if (!newMovesetTag.empty()){
				moveFileName = newMovesetTag + moveFileName;
			}
			/* parse the move file list after having determined the filename*/
			parseMoves();
			/* add common moves to the set */
			loadCommonMoves();
			/* sort moves so that the longest is the first */
			std::sort(movesCollection.begin(), movesCollection.end());
			std::reverse(movesCollection.begin(), movesCollection.end());
		}

	}

	// load basic variables for selectio screen's sake
	void FK_Character::loadVariablesForSelectionScreen(FK_DatabaseAccessor newDatabaseAccessor, std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr) {
		databaseAccessor = newDatabaseAccessor;
		setDatabaseDrivenVariables();
		characterFileName = newFileName;
		characterDirectory = newDirectory;
		setDatabaseDrivenVariables();
		parseCharacterFileForSelectionScreen(characterDirectory + characterFileName);
		/* set animation directory */
		commonDirectory = newCommonDirectory;
		animationDirectory = characterDirectory + "animations\\";
		sharedAnimationDirectory = commonDirectory + "animations\\";
		/* load all available costumes */
		availableCharacterOutfits.clear();
		parseMeshFiles();
		outfitId = 0;
		smgr_reference = smgr;
	}

	// load basic variables for selectio screen's sake
	void FK_Character::loadVariablesForSelectionScreenExtra(FK_DatabaseAccessor newDatabaseAccessor, std::string newFileName, std::string newDirectory, std::string newCommonDirectory,
		scene::ISceneManager* smgr) {
		databaseAccessor = newDatabaseAccessor;
		setDatabaseDrivenVariables();
		characterFileName = newFileName;
		characterDirectory = newDirectory;
		setDatabaseDrivenVariables();
		parseCharacterFileForSelectionScreen(characterDirectory + characterFileName);
		/* set animation directory */
		commonDirectory = newCommonDirectory;
		animationDirectory = characterDirectory + "animations\\";
		sharedAnimationDirectory = commonDirectory + "animations\\";
		/* load all available costumes */
		availableCharacterOutfits.clear();
		parseMeshFilesExtra();
		outfitId = 0;
		smgr_reference = smgr;
	}

	void FK_Character::loadMeshsAndEffects(core::vector3df startingPosition, core::vector3df startingRotation,
		std::string newAdditionalObjectsTag, 
		bool createHitbox, bool loadSFX, bool loadAllAnimations, bool loadAllProjectileMeshs){
		std::string meshfile = (characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory +
			availableCharacterOutfits[outfitId].outfitFilename);
		baseForm.newMeshName = availableCharacterOutfits[outfitId].outfitFilename;
		//m_threadMutex.lock();
		mesh = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
		//m_threadMutex.unlock();
		mesh->convertMeshToTangents();
		animatedMesh = smgr_reference->addAnimatedMeshSceneNode(mesh);
		animatedMesh->setPosition(startingPosition);
		animatedMesh->setRotation(startingRotation);
		f32 scale = meshScale;
		if (availableCharacterOutfits[outfitId].outfitMeshScale > 0.f){
			scale = availableCharacterOutfits[outfitId].outfitMeshScale;
		}
		if (availableCharacterOutfits[outfitId].backfaceCulling != setBackfaceCulling){
			setBackfaceCulling = availableCharacterOutfits[outfitId].backfaceCulling;
		}
		animatedMesh->setScale(core::vector3df(scale, scale, scale));
		animatedMesh->setAnimationSpeed(animationFrameRate);
		animatedMesh->setTransitionTime(animationTransitionTime);
		animatedMesh->setJointMode(irr::scene::EJUOR_CONTROL);

		int mcount = animatedMesh->getMaterialCount();
		for (int i = 0; i < mcount; i++){
			animatedMesh->getMaterial(i).BackfaceCulling = setBackfaceCulling;
			animatedMesh->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
			animatedMesh->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		}
		/* parse bones after having determined the filename*/
		parseBones();
		/* load additional objects */
		if (createHitbox){
			loadHitboxHurtboxCollections();
		}
		loadAdditionalObjects(newAdditionalObjectsTag);
		setAdditionalObjectsObjectsDependencies();
		/* if moves have been loaded, setup the correct flags for each additional object*/
		setAdditionalObjectsMovesDependencies();
		/* load special effects (if any)*/
		if (loadSFX){
			/*create particle system for hit special effects */
			createHitParticleSystem();
			createGuardParticleSystem();
		}
		/* create animation callback with a pointer to this */
		animationCallback = new FK_AnimationEndCallback(this);
		/* set animation callback to the animated mesh */
		animatedMesh->setAnimationEndCallback(animationCallback);
		/* set character into idle pose */
		setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
		/* load all the used animations (stances + move list)*/
		if (loadAllAnimations){
			loadMoveAnimations();
		}
		// load all projectile meshs in cache
		if (loadAllProjectileMeshs){
			loadProjectileMeshs();
		}
		/* set loaded flag to true */
		m_thread_characterLoaded = true;
	}

	void FK_Character::loadHitboxHurtboxCollections() {
		/* fill hitbox collection */
		fillHitboxCollection();
		/* add non-standard hitboxes */
		loadAdditionalHitboxes();
		/* create standard hurtbox */
		fillHurtboxCollection();
	}

	void FK_Character::loadMoveAnimations() {
		for (u32 i = 0; i < movesCollection.size(); ++i) {
			//m_threadMutex.lock();
			std::string animation_name = movesCollection[i].getAnimationName(true).c_str();
			bool loadedFromCharacterDirectory = addAnimationFromDirectXFile(animation_name.c_str(), animationDirectory + animation_name + ".x");
			if (!loadedFromCharacterDirectory) {
				addAnimationFromDirectXFile(animation_name.c_str(), sharedAnimationDirectory + animation_name + ".x");
			}
			animation_name = movesCollection[i].getAnimationName(false).c_str();
			loadedFromCharacterDirectory = addAnimationFromDirectXFile(animation_name.c_str(), animationDirectory + animation_name + ".x");
			if (!loadedFromCharacterDirectory) {
				addAnimationFromDirectXFile(animation_name.c_str(), sharedAnimationDirectory + animation_name + ".x");
			}
			// check also for meshes and transformations
			if (movesCollection[i].hasTransformation()) {
				FK_Move::FK_TransformationMove transformation = movesCollection[i].getTransformation();
				if (transformation.newMeshName != std::string()) {
					std::string meshfile = (characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory +
						transformation.newMeshName);
					scene::ISkinnedMesh* tempMesh = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
					if (!tempMesh) {
						meshfile = (characterDirectory + transformation.newMeshName);
						tempMesh = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
					}
				}
				// check for new stances animations after transformation
				for (auto& stancePair : transformation.newStances) {
					if (stancePair.second.getAnimationName() != std::string()) {
						animation_name = stancePair.second.getAnimationName();
						addAnimationFromDirectXFile(animation_name.c_str(), sharedAnimationDirectory + animation_name + ".x");
					}
				}
			}
			//m_threadMutex.unlock();
		}
		for (auto iterator = basicAnimations.begin(); iterator != basicAnimations.end(); ++iterator) {
			//m_threadMutex.lock();
			std::string animation_name = iterator->second.getAnimationName();
			bool loadedFromCharacterDirectory = addAnimationFromDirectXFile(animation_name.c_str(),
				animationDirectory + animation_name + ".x");
			if (!loadedFromCharacterDirectory) {
				addAnimationFromDirectXFile(animation_name.c_str(), sharedAnimationDirectory + animation_name + ".x");
			}
			//m_threadMutex.unlock();
		}
	}

	void FK_Character::loadProjectileMeshs() {
		for (u32 i = 0; i < movesCollection.size(); ++i) {
			//m_threadMutex.lock();
			u32 bulletArraySize = movesCollection[i].getBulletCollection().size();
			for (u32 k = 0; k < bulletArraySize; ++k) {
				auto bullet = movesCollection[i].getBulletCollection()[k];
				if (bullet.getBillboardFlag()) {
					std::string meshName = bullet.getMeshPath();
					bool success = 
						smgr_reference->getVideoDriver()->getTexture(
						(getCharacterDirectory() + getOutfitPath() + meshName).c_str());
					if (!success) {
						smgr_reference->getVideoDriver()->getTexture((getOutfitPath() + meshName).c_str());
					}
					else {
						movesCollection[i].getBulletCollection()[k].setMeshPath(getCharacterDirectory() + meshName);
					}
					if (bullet.getParticleEmitterTextureName() != std::string()) {
						success = smgr_reference->getVideoDriver()->getTexture((getCharacterDirectory() +
							getOutfitPath() + bullet.getParticleEmitterTextureName()).c_str());
						if (!success) {
							smgr_reference->getVideoDriver()->getTexture(
								(getCharacterDirectory() + bullet.getParticleEmitterTextureName()).c_str());
						}
						else {
							movesCollection[i].getBulletCollection()[k].setParticleEmitter((
								getOutfitPath() + bullet.getParticleEmitterTextureName()).c_str());
						}
					}
				}
				else {
					std::string meshName = bullet.getMeshPath();
					std::string meshpath = getCharacterDirectory() + getOutfitPath() + meshName;
					bool success = smgr_reference->getMesh(
						meshpath.c_str());
					if (!success) {
						smgr_reference->getMesh((getCharacterDirectory() + meshName).c_str());
					}
					else {
						movesCollection[i].getBulletCollection()[k].setMeshPath(getOutfitPath() + meshName);
					}
					if (bullet.getParticleEmitterTextureName() != std::string()) {
						success = smgr_reference->getVideoDriver()->getTexture((getCharacterDirectory() + 
							getOutfitPath() + bullet.getParticleEmitterTextureName()).c_str());
						if (!success) {
							smgr_reference->getVideoDriver()->getTexture(
								(getCharacterDirectory() + bullet.getParticleEmitterTextureName()).c_str());
						}
						else {
							movesCollection[i].getBulletCollection()[k].setParticleEmitter((
								getOutfitPath() + bullet.getParticleEmitterTextureName()).c_str());
						}
					}
				}
			}
		}
	}

	/* class destructor */
	FK_Character::~FK_Character(){
		for (auto iterator = hurtboxCollection.begin(); iterator != hurtboxCollection.end(); ++iterator){
			if (iterator->second != NULL && iterator->second->getReferenceCount() > 0){
				//smgr_reference->getMeshCache()->removeMesh(((scene::IAnimatedMeshSceneNode*) iterator->second)->getMesh());
				iterator->second->remove();
			}
		};
		hurtboxCollection.clear();
		for (auto iterator = hitboxCollection.begin(); iterator != hitboxCollection.end(); ++iterator){
			if (iterator->second != NULL  && iterator->second->getReferenceCount() > 0){
				//smgr_reference->getMeshCache()->removeMesh(((scene::IAnimatedMeshSceneNode*) iterator->second)->getMesh());
				iterator->second->remove();
			}
		};
		hitboxCollection.clear();
		// clear bullets
		for each (FK_Bullet* bullet in bulletsCollection){
			bullet->dispose();
		}
		bulletsCollection.clear();
		// clear objects
		for each (auto node in characterAdditionalObjectsMeshs){
			if (node != NULL && node->getReferenceCount() > 0){
				node->remove();
			}
		}
		characterAdditionalObjectsMeshs.clear();
		characterAdditionalObjects.clear();
		namedAdditionalObjectsReferences.clear();
		if (guardSFX != NULL && guardSFX->getReferenceCount() > 0){
			guardSFX->remove();
		}
		if (hitSFX != NULL && hitSFX->getReferenceCount() > 0){
			hitSFX->remove();
		}
		movesCollection.clear();
		if (animatedMesh != NULL && animatedMesh->getReferenceCount() > 0){
			animatedMesh->remove();
		}
		
		for (auto iterator = animationKeymap.begin(); iterator != animationKeymap.end(); ++iterator) {
			delete iterator->second;
		}
		
		delete animationCallback;
		characterOutfits.clear();
		availableCharacterOutfits.clear();
		nextMove.clear();
		animationKeymap.clear();
		basicAnimations.clear();
		characterFileKeyMap.clear();
		availableTransformations.clear();
		availableTransformationsNames.clear();
	}

	/* copy constructor */
	void FK_Character::setCharacterSelectionVariablesFromCharacter(
		const FK_Character &character, std::string additionalPathString) {
		databaseAccessor = character.databaseAccessor;
		setDatabaseDrivenVariables();
		characterFileName = character.characterFileName;
		characterDirectory = character.characterDirectory;
		if (!additionalPathString.empty()) {
			characterDirectory += additionalPathString;
		}
		lifeScalingFactor = character.lifeScalingFactor;
		name = character.name;
		wname = character.wname;
		basicAnimations = character.basicAnimations;
		meshScale = character.meshScale;
		/* load all available costumes */
		availableCharacterOutfits = character.availableCharacterOutfits;
		characterOutfits = character.characterOutfits;
		bonesFileName = character.bonesFileName;
		smgr_reference = character.smgr_reference;
		/* set animation directory */
		commonDirectory = character.commonDirectory;
		animationDirectory = characterDirectory + "animations\\";
		sharedAnimationDirectory = commonDirectory + "animations\\";
	}

	// check if character has been loaded
	bool FK_Character::hasBeenLoaded(){
		return m_thread_characterLoaded;
	}

	//get the number of alternative outfits
	int FK_Character::getNumberOfOutfits(){
		return availableCharacterOutfits.size();
	}

	std::string FK_Character::getAnimationDirectory()
	{
		return animationDirectory;
	}

	// get current outfit path
	std::string FK_Character::getOutfitPath(){
		return characterOutfits[outfitId].outfitDirectory;
	}

	// get current outfit
	FK_Character::FK_CharacterOutfit FK_Character::getOutfit(s32 currentOutfitId){
		if (currentOutfitId < 0) {
			currentOutfitId = outfitId;
		}
		else if (currentOutfitId >= characterOutfits.size()) {
			currentOutfitId %= (s32)characterOutfits.size();
		}
		return characterOutfits[currentOutfitId];
	}

	void FK_Character::parseSingleCostume(std::map<FK_OutfitFileKeys, std::string>& outfitKeysMap, std::string costumeDirectory) {
		std::string additionalMeshConfigFilename = "config.txt";
		std::string path = characterDirectory + costumeDirectory;
		std::ifstream configurationFile((path + additionalMeshConfigFilename).c_str());
		if (!configurationFile) {
			return;
		}
		std::string temp;
		FK_Character::FK_CharacterOutfit newOutfit;
		while (configurationFile >> temp) {
			if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitFilename]) {
				configurationFile >> temp;
				newOutfit.outfitFilename = temp;
				newOutfit.outfitDirectory = costumeDirectory;
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitName]) {
				configurationFile >> temp;
				std::replace(temp.begin(), temp.end(), '_', ' ');
				newOutfit.outfitName = temp;
				newOutfit.outfitWName = fk_addons::convertNameToNonASCIIWstring(temp);
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeCharacterName]) {
				configurationFile >> temp;
				std::replace(temp.begin(), temp.end(), '_', ' ');
				newOutfit.outfitCharacterName = temp;
				newOutfit.outfitWCharacterName = fk_addons::convertNameToNonASCIIWstring(temp);
				newOutfit.outfitDisplayName = temp;
				newOutfit.outfitWDisplayName = fk_addons::convertNameToNonASCIIWstring(temp);
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitAvailable]) {
				u32 availabilityFlag;
				configurationFile >> availabilityFlag;
				newOutfit.isAvailableFromBeginning = (bool)availabilityFlag;
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitMeshScale]) {
				f32 tempScale = 1.0f;
				configurationFile >> tempScale;
				newOutfit.outfitMeshScale = max(0.0f, tempScale);
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitBackfaceCulling]) {
				newOutfit.backfaceCulling = true;
			}
			else if (temp == outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeDisplayName]) {
				configurationFile >> temp;
				std::replace(temp.begin(), temp.end(), '_', ' ');
				newOutfit.outfitDisplayName = temp;
				newOutfit.outfitWDisplayName = fk_addons::convertNameToNonASCIIWstring(temp);
			}
		}
		//if (newOutfit.isAvailableFromBeginning){
		newOutfit.outfitId = (s32)availableCharacterOutfits.size();
		availableCharacterOutfits.push_back(newOutfit);
		//}
		characterOutfits.push_back(newOutfit);
		configurationFile.clear();
		configurationFile.close();
	}

	void FK_Character::parseAllMeshFilesDirectorySearch(const std::string& additionalFolder) {
		u32 maximumNumberOfMeshes = 100;
		std::map<FK_OutfitFileKeys, std::string> outfitKeysMap;
		outfitKeysMap[FK_OutfitFileKeys::OutfitFilename] = "#MESH_FILENAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitName] = "#OUTFIT_NAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAvailable] = "#AVAILABLE_FROM_START";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeCharacterName] = "#CHARACTER_NAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeDisplayName] = "#CHARACTER_SHORTNAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitMeshScale] = "#OUTFIT_MESH_SCALE";
		outfitKeysMap[FK_OutfitFileKeys::OutfitBackfaceCulling] = "#NO_BACKFACE";
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::AdditionalCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			parseSingleCostume(outfitKeysMap, subfolder);
		}
		// addons
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::DLCCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			parseSingleCostume(outfitKeysMap, subfolder);
		}
		// if the additional folder is not empty, parse also that one
		if (!additionalFolder.empty()) {
			parseSingleCostume(outfitKeysMap, additionalFolder);
		}
	}

	void FK_Character::parseAllMeshFilesDirectorySearchExtra(const std::string& additionalFolder) {
		u32 maximumNumberOfMeshes = 100;
		std::map<FK_OutfitFileKeys, std::string> outfitKeysMap;
		outfitKeysMap[FK_OutfitFileKeys::OutfitFilename] = "#MESH_FILENAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitName] = "#OUTFIT_NAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAvailable] = "#AVAILABLE_FROM_START";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeCharacterName] = "#CHARACTER_NAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeDisplayName] = "#CHARACTER_SHORTNAME";
		outfitKeysMap[FK_OutfitFileKeys::OutfitMeshScale] = "#OUTFIT_MESH_SCALE";
		outfitKeysMap[FK_OutfitFileKeys::OutfitBackfaceCulling] = "#NO_BACKFACE";
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::AdditionalCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			parseSingleCostume(outfitKeysMap, subfolder);
		}
		// addons
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::DLCCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			parseSingleCostume(outfitKeysMap, subfolder);
		}
		// EXTRA
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::ExtraCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			parseSingleCostume(outfitKeysMap, subfolder);
		}
		// if the additional folder is not empty, parse also that one
		if (!additionalFolder.empty()) {
			parseSingleCostume(outfitKeysMap, additionalFolder);
		}
	}

	// load additional mesh names
	void FK_Character::parseMeshFiles(const std::string& additionalFolder){
		// TO BE IMPLEMENTED
		// browse through all the subfolders with a certain root, up to 100
		// first check if there is any file which contains available meshs
		std::string masterFileMeshConfig = "costumes.txt";
		std::ifstream masterFile((characterDirectory + masterFileMeshConfig).c_str());
		std::vector<std::string> folders;
		bool standardSearch = true;
		if(masterFile){
			std::string temp;
			while (masterFile >> temp) {
				folders.push_back(temp + "\\");
			}
			if (!additionalFolder.empty()) {
				folders.push_back(additionalFolder);
			}
			standardSearch = folders.empty();
		}
		if(standardSearch){
			parseAllMeshFilesDirectorySearch(additionalFolder);
			return;
		}
		else {
			std::map<FK_OutfitFileKeys, std::string> outfitKeysMap;
			outfitKeysMap[FK_OutfitFileKeys::OutfitFilename] = "#MESH_FILENAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitName] = "#OUTFIT_NAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAvailable] = "#AVAILABLE_FROM_START";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeCharacterName] = "#CHARACTER_NAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeDisplayName] = "#CHARACTER_SHORTNAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitMeshScale] = "#OUTFIT_MESH_SCALE";
			outfitKeysMap[FK_OutfitFileKeys::OutfitBackfaceCulling] = "#NO_BACKFACE";
			for each(std::string subfolder in folders) {
				parseSingleCostume(outfitKeysMap, subfolder);
			}
		}
	};

	// load additional mesh names
	void FK_Character::parseMeshFilesExtra(const std::string& additionalFolder) {
		// TO BE IMPLEMENTED
		// browse through all the subfolders with a certain root, up to 100
		// first check if there is any file which contains available meshs
		std::string masterFileMeshConfig = "costumes.txt";
		std::ifstream masterFile((characterDirectory + masterFileMeshConfig).c_str());
		std::vector<std::string> folders;
		bool standardSearch = true;
		if (masterFile) {
			std::string temp;
			while (masterFile >> temp) {
				folders.push_back(temp + "\\");
			}
			if (!additionalFolder.empty()) {
				folders.push_back(additionalFolder);
			}
			standardSearch = folders.empty();
		}
		if (standardSearch) {
			parseAllMeshFilesDirectorySearchExtra(additionalFolder);
			return;
		}
		else {
			std::map<FK_OutfitFileKeys, std::string> outfitKeysMap;
			outfitKeysMap[FK_OutfitFileKeys::OutfitFilename] = "#MESH_FILENAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitName] = "#OUTFIT_NAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAvailable] = "#AVAILABLE_FROM_START";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeCharacterName] = "#CHARACTER_NAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitAlternativeDisplayName] = "#CHARACTER_SHORTNAME";
			outfitKeysMap[FK_OutfitFileKeys::OutfitMeshScale] = "#OUTFIT_MESH_SCALE";
			outfitKeysMap[FK_OutfitFileKeys::OutfitBackfaceCulling] = "#NO_BACKFACE";
			for each (std::string subfolder in folders) {
				parseSingleCostume(outfitKeysMap, subfolder);
			}
		}
	};

	void FK_Character::parseCharacterFile(std::string filename){
		/* create parsing map */
		characterFileKeyMap[FK_CharacterFileKeys::CharacterName] = "#NAME";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterDisplayName] = "#SHORTNAME";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitSFXFile] = "#HIT_SFX";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitSFXFileTrigger] = "#HIT_SFX_TRIGGER";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterMovesFile] = "#MOVESFILE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterBonesFile] = "#BONESFILE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitboxFile] = "#HITBOXFILE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHurtboxFile] = "#HURTBOXFILE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterLife] = "#LIFE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWeight] = "#WEIGHT";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWalkSpeed] = "#WALKSPEED";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterJumpSpeed] = "#JUMPSPEED";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterStanceFrames] = "#STANCE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWalkFrames] = "#WALKING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterSidestepFrames] = "#SIDESTEP";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterStandGuardFrames] = "#STANDGUARD";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingFrames] = "#CROUCHING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingStanceFrames] = "#LOWSTANCE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingGuardFrames] = "#CROUCHGUARD";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterRunningFrames] = "#RUNNING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakHighFrontFrames] = "#HITHIGHWEAKFRONT";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakMediumFrontFrames] = "#HITMEDWEAKFRONT";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakLowFrontFrames] = "#HITLOWWEAKFRONT";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakHighBackFrames] = "#HITHIGHWEAKBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakMediumBackFrames] = "#HITMEDWEAKBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakLowBackFrames] = "#HITLOWWEAKBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongHighFrames] = "#HITHIGHSTRONG";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongHighBackFrames] = "#HITHIGHSTRONGBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongMediumFrames] = "#HITMEDSTRONG";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongMediumBackFrames] = "#HITMEDSTRONGBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongLowFrames] = "#HITLOWSTRONG";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongLowBackFrames] = "#HITLOWSTRONGBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongFlightFrames] = "#HITFLIGHTSTRONG";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongFlightBackFrames] = "#HITFLIGHTSTRONGBACK";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterSupineLandingFrames] = "#SUPINELANDING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedSupineFrames] = "#GROUNDEDSUPINE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedProneFrames] = "#GROUNDEDPRONE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedSupineRotationFrames] = "#GROUNDEDSUPINEROTATION";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedProneRotationFrames] = "#GROUNDEDPRONEROTATION";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWinAnimationFrames] = "#WINPOSE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterIntroAnimationFrames] = "#INTRO";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterMeshScale] = "#MESH_SCALE";
		characterFileKeyMap[FK_CharacterFileKeys::SpecialStance1AnimationFrames] = "#SPECIALSTANCE1";
		characterFileKeyMap[FK_CharacterFileKeys::SpecialStance2AnimationFrames] = "#SPECIALSTANCE2";
		characterFileKeyMap[FK_CharacterFileKeys::SpecialStance3AnimationFrames] = "#SPECIALSTANCE3";
		characterFileKeyMap[FK_CharacterFileKeys::SpecialStance4AnimationFrames] = "#SPECIALSTANCE4";
		characterFileKeyMap[FK_CharacterFileKeys::SpecialStance5AnimationFrames] = "#SPECIALSTANCE5";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWakeUpFrames] = "#WAKEUP";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterJumpingFrames] = "#JUMPING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterAIArchetype] = "#AI_TYPE";
		/* additional animations */
		characterFileKeyMap[FK_CharacterFileKeys::CharacterBackWalkFrames] = "#BACKWALKING";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterBackwardJumpFrames] = "#BACKWARDJUMP";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterForwardJumpFrames] = "#FORWARDJUMP";
		std::map<std::string, FK_AIArchetypes> AI_keysMap;
		AI_keysMap["aggressor"] = FK_AIArchetypes::Aggressor;
		AI_keysMap["tactician"] = FK_AIArchetypes::Tactician;
		AI_keysMap["balanced"] = FK_AIArchetypes::Balanced;
		AI_keysMap["luchador"] = FK_AIArchetypes::Luchador;
		AI_keysMap["complete"] = FK_AIArchetypes::JackOfAllTrades;
		/* open file and start parsing it */
		std::ifstream inputFile(filename.c_str());
		while (!inputFile.eof()){
			std::string temp;
			inputFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterName]){
				inputFile >> temp;
				name = temp;
				displayName = temp;
				// replace all underscores with white spaces in the names
				std::replace(name.begin(), name.end(), '_', ' ');
				std::replace(displayName.begin(), displayName.end(), '_', ' ');
				wname = fk_addons::convertNameToNonASCIIWstring(name);
				wdisplayName = fk_addons::convertNameToNonASCIIWstring(displayName);
			}
			/* display name (optional)*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterDisplayName]){
				inputFile >> temp;
				displayName = temp;
				std::replace(displayName.begin(), displayName.end(), '_', ' ');
				wdisplayName = fk_addons::convertNameToNonASCIIWstring(displayName);
			}
			/* mesh scale*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterMeshScale]){
				f32 scale;
				inputFile >> scale;
				meshScale = max(scale, 0.0f);
			}
			/* AI type*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterAIArchetype]){
				inputFile >> temp;
				if (AI_keysMap.count(temp) > 0){
					AIarchetype = AI_keysMap[temp];
				}
			}
			/* sfx file name*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitSFXFile]){
				inputFile >> temp;
				particleSystemTextureFileName = temp;
			}
			/* trigger sfx file name*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitSFXFileTrigger]){
				inputFile >> temp;
				particleSystemTextureFileNameTrigger = temp;
			}
			/* moves file name */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterMovesFile]){
				inputFile >> temp;
				moveFileName = temp;
			}
			/* bones file name */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterBonesFile]){
				inputFile >> temp;
				bonesFileName = temp;
			}
			/* hitbox file name */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitboxFile]){
				inputFile >> temp;
				hitboxFileName = temp;
			}
			/* hurtbox file name */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHurtboxFile]){
				inputFile >> temp;
				hurtboxFileName = temp;
			}
			/* lelse ife */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterLife]){
				inputFile >> life;
				maxLife = life;
			}
			/* walk speed */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterWalkSpeed]){
				/*inputFile >> walkingSpeed;
				walkingSpeed = 80.0f;
				runningSpeed = walkingSpeed * 2.5f;*/
			}
			/* check for basic poses */
			/* idle */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterStanceFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::IdleAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterWalkFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::WalkingAnimation] = temp;
				if (basicAnimations.find(FK_BasicPose_Type::BackWalkingAnimation) == basicAnimations.end()) {
					basicAnimations[FK_BasicPose_Type::BackWalkingAnimation] = temp;
				}
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterBackWalkFrames]) {
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::BackWalkingAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterRunningFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::RunningAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterSidestepFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SidestepAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterStandGuardFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::GuardingAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingGuardFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::CrouchingGuardAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::CrouchingAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterCrouchingStanceFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::CrouchingIdleAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterJumpingFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::JumpingAnimation] = temp;
				if (basicAnimations.find(FK_BasicPose_Type::ForwardJumpAnimation) == basicAnimations.end()) {
					basicAnimations[FK_BasicPose_Type::ForwardJumpAnimation] = temp;
				}
				if (basicAnimations.find(FK_BasicPose_Type::BackwardJumpAnimation) == basicAnimations.end()) {
					basicAnimations[FK_BasicPose_Type::BackwardJumpAnimation] = temp;
				}
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterForwardJumpFrames]) {
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::ForwardJumpAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterBackwardJumpFrames]) {
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::BackwardJumpAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance1AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance1Animation] = temp;
				std::string stanceName;
				inputFile >> stanceName;
				std::replace(stanceName.begin(), stanceName.end(), '_', ' ');
				specialStanceDictionary[FK_Stance_Type::SpecialStance1] = stanceName;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance2AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance2Animation] = temp;
				std::string stanceName;
				inputFile >> stanceName;
				std::replace(stanceName.begin(), stanceName.end(), '_', ' ');
				specialStanceDictionary[FK_Stance_Type::SpecialStance2] = stanceName;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance3AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance3Animation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance3AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance3Animation] = temp;
				std::string stanceName;
				inputFile >> stanceName;
				std::replace(stanceName.begin(), stanceName.end(), '_', ' ');
				specialStanceDictionary[FK_Stance_Type::SpecialStance3] = stanceName;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance4AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance4Animation] = temp;
				std::string stanceName;
				inputFile >> stanceName;
				std::replace(stanceName.begin(), stanceName.end(), '_', ' ');
				specialStanceDictionary[FK_Stance_Type::SpecialStance4] = stanceName;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::SpecialStance5AnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::SpecialStance5Animation] = temp;
				std::string stanceName;
				inputFile >> stanceName;
				std::replace(stanceName.begin(), stanceName.end(), '_', ' ');
				specialStanceDictionary[FK_Stance_Type::SpecialStance5] = stanceName;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterWakeUpFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::SupineWakeUpAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakHighFrontFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakHighAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakMediumFrontFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakMediumAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakLowFrontFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakLowAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakHighBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakHighBackAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakMediumBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakMediumBackAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitWeakLowBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitWeakLowBackAnimation] = temp;
			}
			// strong hits (front and back)
			// HIGH
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongHighFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongHighAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongHighBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongHighBackAnimation] = temp;
			}
			// MID
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongMediumFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongMediumAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongMediumBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongMediumBackAnimation] = temp;
			}
			// LOW
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongLowFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongLowAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongLowBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::HitStrongLowBackAnimation] = temp;
			}
			// FLIGHT
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongFlightFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::HitStrongFlightAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterHitStrongFlightBackFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for strong hits (i.e. hits which send you flying) the animation is looped
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::HitStrongFlightBackAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterSupineLandingFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				// for landing after a strong hit
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::SupineLandingAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedSupineFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::GroundedSupineAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedProneFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::GroundedProneAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedSupineRotationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::GroundedSupineRotationAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterGroundedProneRotationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::GroundedProneRotationAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterWinAnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::WinAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterIntroAnimationFrames]){
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::IntroAnimation] = temp;
			}
		}
		baseForm.newStances = basicAnimations;
		baseForm.isBasicForm = true;
		currentForm = baseForm;
	}

	// parse character file for selection screen
	void FK_Character::parseCharacterFileForSelectionScreen(std::string filename) {
		/* create parsing map */
		characterFileKeyMap[FK_CharacterFileKeys::CharacterName] = "#NAME";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterStanceFrames] = "#STANCE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterWinAnimationFrames] = "#WINPOSE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterMeshScale] = "#MESH_SCALE";
		characterFileKeyMap[FK_CharacterFileKeys::CharacterBonesFile] = "#BONESFILE";
		/* open file and start parsing it */
		std::ifstream inputFile(filename.c_str());
		while (!inputFile.eof()) {
			std::string temp;
			inputFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterName]) {
				inputFile >> temp;
				name = temp;
				displayName = temp;
				// replace all underscores with white spaces in the names
				std::replace(name.begin(), name.end(), '_', ' ');
				std::replace(displayName.begin(), displayName.end(), '_', ' ');
				wname = fk_addons::convertNameToNonASCIIWstring(name);
				wdisplayName = fk_addons::convertNameToNonASCIIWstring(displayName);
			}
			/* mesh scale*/
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterMeshScale]) {
				f32 scale;
				inputFile >> scale;
				meshScale = max(scale, 0.0f);
			}
			/* idle */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterStanceFrames]) {
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, true);
				basicAnimations[FK_BasicPose_Type::IdleAnimation] = temp;
			}
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterWinAnimationFrames]) {
				int startFrame, stopFrame;
				std::string animationName;
				inputFile >> animationName >> startFrame >> stopFrame;
				FK_Pose temp(animationName, startFrame, stopFrame, false);
				basicAnimations[FK_BasicPose_Type::WinAnimation] = temp;
			}
			/* bones file name */
			else if (temp == characterFileKeyMap[FK_CharacterFileKeys::CharacterBonesFile]) {
				inputFile >> temp;
				bonesFileName = temp;
			}
		}
		baseForm.newStances = basicAnimations;
		baseForm.isBasicForm = true;
	}

	FK_CharacterStats FK_Character::getCharacterStats()
	{
		return stats;
	}

	// set database connected variables
	void FK_Character::setDatabaseDrivenVariables()
	{
		triggerCounters = databaseAccessor.getStartingTriggers();
		animationFrameRate = databaseAccessor.getAnimationFramerateFPS();
		walkingSpeed = databaseAccessor.getBasicCharacterWalkSpeed();
		sidestepSpeed = databaseAccessor.getBasicCharacterSidestepSpeed();
		mixedstepSpeed = databaseAccessor.getBasicCharacterMixedStepSpeed();
		runningSpeed = databaseAccessor.getBasicCharacterRunningSpeed();
	}

	void FK_Character::loadCommonMoves()
	{
		std::string commonMovesFilePath = commonDirectory + "..\\characters\\common\\moves.txt";
		std::ifstream ifile(commonMovesFilePath.c_str());
		if (!ifile) {
			return;
		}
		FK_MoveFileParser* moveParser = new FK_MoveFileParser(commonMovesFilePath);
		auto commonMovesCollection = moveParser->getParsedMoves();
		u32 startingId = movesCollection.size() + 1;
		auto previousMoveCollection = movesCollection;
		for each (FK_Move move in commonMovesCollection) {
			bool moveOverwritten = false;
			for each (FK_Move standard_move in previousMoveCollection) {
				if (standard_move.getName() == move.getName()) {
					moveOverwritten = true;
					break;
				}
			}
			if (!moveOverwritten) {
				move.setMoveId(startingId);
				movesCollection.push_back(move);
				startingId += 1;
				if (move.hasTransformation() && move.getTransformation().transformationNameTag != std::string()) {
					if (availableTransformations.count(move.getTransformation().transformationNameTag) == 0) {
						availableTransformations[move.getTransformation().transformationNameTag] = move.getTransformation();
						availableTransformationsNames.push_back(move.getTransformation().transformationNameTag);
					}
				}
			}
		}
		moveParser->setFollowupsIds(movesCollection);
		delete moveParser;
	}

	void FK_Character::parseMoves(){
		FK_MoveFileParser* moveParser = new FK_MoveFileParser(characterDirectory + moveFileName);
		movesCollection = moveParser->getParsedMoves();
		// store transformations
		for each (FK_Move move in movesCollection) {
			if (move.hasTransformation() && move.getTransformation().transformationNameTag != std::string()) {
				if (availableTransformations.count(move.getTransformation().transformationNameTag) == 0) {
					availableTransformations[move.getTransformation().transformationNameTag] = move.getTransformation();
					availableTransformationsNames.push_back(move.getTransformation().transformationNameTag);
				}
			}
		}
		delete moveParser;
	};

	void FK_Character::parseBones(){
		FK_BoneFileParser* boneParser = new FK_BoneFileParser(characterDirectory + bonesFileName);
		armatureBonesCollection = boneParser->getBonesMap();
		delete boneParser;
	};
	void FK_Character::fillHitboxCollection(){
		/*	// file will be probably used later, but now...
		std::ifstream file(hitboxFileName.c_str());
		FK_MoveFileParser* moveParser = new FK_MoveFileParser();
		FK_BoneFileParser* boneParser = new FK_BoneFileParser();
		moveParser->getHitboxParsingMap();
		boneParser->getParsingMap();
		delete moveParser;
		delete boneParser;
		*/
		core::vector3df hitboxDefaultScale = core::vector3df(0.06f, 0.06f, 0.06f);
		if (databaseAccessor.extendedJugglesAllowed()) {
			core::vector3df hitboxDefaultScale = core::vector3df(0.08f, 0.08f, 0.06f);
		}
		/* create torso variables */
		core::vector3df hitboxDefaultTorsoScale(0.2f, 0.2f, 0.3f);
		core::vector3df hitboxDefaultTorsoOffset(0, 0.1f, 0);
		/* create head variables */
		core::vector3df hitboxDefaultHeadScale(0.07f, 0.1f, 0.07f);
		core::vector3df hitboxDefaultHeadOffset(0, 0.5f, 0);
		hitboxCollection.clear();
		std::map<FK_Hitbox_Type, FK_Bones_Type> standard_hitboxes;
		standard_hitboxes[FK_Hitbox_Type::RightPunch] = FK_Bones_Type::RightFingersArmatureMedium;
		standard_hitboxes[FK_Hitbox_Type::RightElbow] = FK_Bones_Type::RightForearmArmature;
		standard_hitboxes[FK_Hitbox_Type::RightKnee] = FK_Bones_Type::RightKneeArmature;
		standard_hitboxes[FK_Hitbox_Type::RightFoot] = FK_Bones_Type::RightFootArmature;
		standard_hitboxes[FK_Hitbox_Type::LeftPunch] = FK_Bones_Type::LeftFingersArmatureMedium;
		standard_hitboxes[FK_Hitbox_Type::LeftElbow] = FK_Bones_Type::LeftForearmArmature;
		standard_hitboxes[FK_Hitbox_Type::LeftKnee] = FK_Bones_Type::LeftKneeArmature;
		standard_hitboxes[FK_Hitbox_Type::LeftFoot] = FK_Bones_Type::LeftFootArmature;
		standard_hitboxes[FK_Hitbox_Type::Torso] = FK_Bones_Type::TorsoArmature;
		standard_hitboxes[FK_Hitbox_Type::Head] = FK_Bones_Type::HeadArmature;
		/* browse through vector and assign bones to the proper hitbox */
		for (auto iterator = standard_hitboxes.begin(); iterator != standard_hitboxes.end(); ++iterator){
			std::string boneName = armatureBonesCollection[iterator->second];
			scene::IBoneSceneNode* bone = animatedMesh->getJointNode(boneName.c_str());
			if (bone == NULL){
				continue;
			}
			scene::ISceneNode * hitbox = smgr_reference->addSphereSceneNode();
			hitbox->setMaterialFlag(video::EMF_LIGHTING, false);
			hitbox->setMaterialFlag(video::EMF_WIREFRAME, true);
			hitbox->setParent(bone);
			//hitbox->setMaterialType(video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL);
			//hitbox->getMaterial(0).DiffuseColor = video::SColor(128, 0, 255, 0);
			if (iterator->first == FK_Hitbox_Type::Torso){
				hitbox->setScale(hitboxDefaultTorsoScale);
				hitbox->setPosition(hitboxDefaultTorsoOffset);
			}
			else if (iterator->first == FK_Hitbox_Type::Head){
				hitbox->setScale(hitboxDefaultHeadScale);
				hitbox->setPosition(hitboxDefaultHeadOffset);
			}
			else{
				hitbox->setScale(hitboxDefaultScale);
			}
			//hitbox->setDebugDataVisible(scene::EDS_BBOX);
			hitbox->setVisible(false);
			hitboxCollection[iterator->first] = hitbox;
			hitbox->updateAbsolutePosition();
		}
	};
	/* create standard hurtbox for the character */
	void FK_Character::fillHurtboxCollection(){
		/*create torso*/
		core::vector3df size(0.2f, 0.1f, 0.25f);
		core::vector3df offset(0, 0.1f, 0);
		if (databaseAccessor.extendedJugglesAllowed()) {
			size = core::vector3df(0.2f, 0.1f, 0.2f);
			offset = core::vector3df(0, 0.1f, 0.f);
		}
		FK_Bones_Type bone = FK_Bones_Type::TorsoArmature;
		FK_Hurtbox_Type type = FK_Hurtbox_Type::TorsoHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/* create head*/
		size = core::vector3df(0.1f, 0.1f, 0.1f);
		offset = core::vector3df(0, 0.5f, 0);
		bone = FK_Bones_Type::HeadArmature;
		type = FK_Hurtbox_Type::HeadHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/*create spine*/
		size = core::vector3df(0.2f, 0.1f, 0.2f);
		offset = core::vector3df(0, -0.1f, 0);
		if (databaseAccessor.extendedJugglesAllowed()) {
			size = core::vector3df(0.12f, 0.1f, 0.12f);
			offset = core::vector3df(0.f, -0.1f, 0);
		}
		bone = FK_Bones_Type::SpineArmature;
		type = FK_Hurtbox_Type::SpineHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/*create hips*/
		size = core::vector3df(0.2f, 0.05f, 0.2f);
		offset = core::vector3df(0, -0.15f, 0);
		if (databaseAccessor.extendedJugglesAllowed()) {
			size = core::vector3df(0.12f, 0.05f, 0.12f);
			offset = core::vector3df(0, -0.15f, 0);
		}
		bone = FK_Bones_Type::HipsArmature;
		type = FK_Hurtbox_Type::HipsHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/*create waists*/
		size = core::vector3df(0.06f, 0.2f, 0.06f);
		offset = core::vector3df(0, 1, 0);
		bone = FK_Bones_Type::RightLegArmature;
		type = FK_Hurtbox_Type::RightWaistHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		bone = FK_Bones_Type::LeftLegArmature;
		type = FK_Hurtbox_Type::LeftWaistHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/* create legs*/
		bone = FK_Bones_Type::RightKneeArmature;
		type = FK_Hurtbox_Type::RightLegHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		bone = FK_Bones_Type::LeftKneeArmature;
		type = FK_Hurtbox_Type::LeftLegHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		bone = FK_Bones_Type::LeftKneeArmature;
		type = FK_Hurtbox_Type::LeftLegHurtbox;
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
		/* create fullbody hurtbox for more on-spot collisions*/
		bone = FK_Bones_Type::RootArmature;
		type = FK_Hurtbox_Type::FullBodyHurtbox;
		size = core::vector3df(3.0f, 4.f, 3.0f);
		offset = core::vector3df(0, -25, 0);
		hurtboxCollection[type] = createHurtbox(bone, size, offset);
	}
	/* create a single hurtbox and place it properly - if parent is NULL, no parent is set*/
	scene::ISceneNode *FK_Character::createHurtbox(FK_Bones_Type parent, core::vector3df size, core::vector3df offset){
		std::string boneName = armatureBonesCollection[parent];
		scene::IBoneSceneNode* bone = animatedMesh->getJointNode(boneName.c_str());
		scene::ISceneNode * hurtbox = smgr_reference->addSphereSceneNode();
		hurtbox->setMaterialFlag(video::EMF_LIGHTING, false);
		hurtbox->setMaterialFlag(video::EMF_WIREFRAME, true);
		hurtbox->setScale(size);
		if (bone != NULL){
			hurtbox->setParent(bone);
		}
		else{
			hurtbox->setParent(animatedMesh);
		}
		hurtbox->setPosition(offset);
		//hurtbox->setDebugDataVisible(scene::EDS_BBOX);
		hurtbox->setVisible(false);
		hurtbox->updateAbsolutePosition();
		return hurtbox;
	}
	/* toggle hitbox visibility */
	void FK_Character::toggleHitboxVisibility(bool newVisibility){
		/* browse through vector and assign bones to the proper hitbox */
		for (auto iterator = hitboxCollection.begin(); iterator != hitboxCollection.end(); ++iterator){
			iterator->second->setVisible(newVisibility);
		}
	};
	/* toggle hurtbox visibility */
	void FK_Character::toggleHurtboxVisibility(bool newVisibility){
		/* browse through vector and assign bones to the proper hitbox */
		for (auto iterator = hurtboxCollection.begin(); iterator != hurtboxCollection.end(); ++iterator){
			iterator->second->setVisible(newVisibility);
		}
	};
	/* check if hitboxes/hurtboxes are visible*/
	bool FK_Character::checkHitboxVisibility() {
		/* browse through vector and check visibility of each hitbox */
		bool flag = false;
		for (auto iterator = hitboxCollection.begin(); iterator != hitboxCollection.end(); ++iterator) {
			flag = iterator->second->isVisible();
			if (flag) {
				return true;
			}
		}
		return false;
	}
	bool FK_Character::checkHurtboxVisibility() {
		bool flag = false;
		for (auto iterator = hurtboxCollection.begin(); iterator != hurtboxCollection.end(); ++iterator) {
			flag = iterator->second->isVisible();
			if (flag) {
				return true;
			}
		}
		return false;
	}
	// Graphics section
	scene::IAnimatedMeshSceneNode* FK_Character::getAnimatedMesh(){
		return animatedMesh;
	};
	void FK_Character::toggleLighting(bool lightingActive){
		animatedMesh->setMaterialFlag(video::EMF_LIGHTING, lightingActive);
		for (u32 i = 0; i < characterAdditionalObjectsMeshs.size(); ++i){
			if (!characterAdditionalObjects[i].hasDummyMesh()){
				characterAdditionalObjectsMeshs[i]->setMaterialFlag(video::EMF_LIGHTING, lightingActive);
			}
		}
	};
	void FK_Character::toggleWireframe(bool wireframeActive){
		animatedMesh->setMaterialFlag(video::EMF_WIREFRAME, wireframeActive);
		for (u32 i = 0; i < characterAdditionalObjectsMeshs.size(); ++i){
			if (!characterAdditionalObjects[i].hasDummyMesh()){
				characterAdditionalObjectsMeshs[i]->setMaterialFlag(video::EMF_WIREFRAME, wireframeActive);
			}
		}
	};
	// particle system
	void FK_Character::createHitParticleSystem(){
		hitSFX = smgr_reference->addParticleSystemSceneNode(false);
		std::string path = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory + particleSystemTextureFileName;
		hitSFXTexture = smgr_reference->getVideoDriver()->getTexture(path.c_str());
		if (!hitSFXTexture){
			path = characterDirectory + particleSystemTextureFileName;
			hitSFXTexture = smgr_reference->getVideoDriver()->getTexture(path.c_str());
		}
		path = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory + particleSystemTextureFileNameTrigger;
		hitSFXTextureTrigger = smgr_reference->getVideoDriver()->getTexture(path.c_str());
		if (!hitSFXTextureTrigger){
			path = characterDirectory + particleSystemTextureFileNameTrigger;
			hitSFXTextureTrigger = smgr_reference->getVideoDriver()->getTexture(path.c_str());
		}
		//m_threadMutex.unlock();
		scene::IParticleEmitter* em = hitSFX->createBoxEmitter(
			core::aabbox3d<f32>(-4, 0, -4, 4, 1, 4), // emitter size
			core::vector3df(0.0f, 0.1f, 0.0f),   // initial direction
			0, 0,                             // emit rate (0 assures that no particles are created until a hit)
			video::SColor(0, 255, 255, 255),       // darkest color
			video::SColor(0, 255, 255, 255),       // brightest color
			100, 120, 360,                         // min and max age, angle
			core::dimension2df(8.0f, 8.0f),         // min size
			core::dimension2df(22.0f, 22.0f));        // max size

		hitSFX->setEmitter(em); // this grabs the emitter
		em->drop(); // so we can drop it here without deleting it
		hitSFX->setScale(core::vector3df(2, 2, 2));
		hitSFX->setMaterialFlag(video::EMF_LIGHTING, false);
		hitSFX->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		
		hitSFX->setMaterialTexture(0, hitSFXTexture);
		hitSFX->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		// set particle expiration time to 200 ms.
		particleExpirationTime = 200.0;
	};
	void FK_Character::createGuardParticleSystem(){
		guardSFX = smgr_reference->addParticleSystemSceneNode(false);

		scene::IParticleEmitter* em = hitSFX->createBoxEmitter(
			core::aabbox3d<f32>(-2, 0, -2, 2, 1, 2), // emitter size
			core::vector3df(0.0f, 0.1f, 0.0f),   // initial direction
			0, 0,                             // emit rate (0 assures that no particles are created until a hit)
			video::SColor(0, 255, 255, 255),       // darkest color
			video::SColor(0, 255, 255, 255),       // brightest color
			50, 150, 360,                         // min and max age, angle
			core::dimension2df(8.0f, 8.0f),         // min size
			core::dimension2df(26.0f, 26.0f));        // max size

		guardSFX->setEmitter(em); // this grabs the emitter
		em->drop(); // so we can drop it here without deleting it
		guardSFX->setScale(core::vector3df(2, 2, 2));
		guardSFX->setMaterialFlag(video::EMF_LIGHTING, false);
		guardSFX->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		std::string path = commonDirectory + fk_constants::FK_GuardSFXFileName;
		//m_threadMutex.lock();
		guardSFX->setMaterialTexture(0, smgr_reference->getVideoDriver()->getTexture(path.c_str()));
		//m_threadMutex.unlock();
		guardSFX->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		// make it invisible until consition are met
		//guardSFX->setVisible(false);
		// set particle expiration time to 200 ms.
		//particleExpirationTime = 300.0;
	};
	/* activate particle effect visibility */
	void FK_Character::activateParticleEffect(FK_Hitbox_Type hitboxId, bool guard){
		activateParticleEffect(hitboxCollection[hitboxId]->getAbsolutePosition(), guard);
	};

	/* activate particle effect visibility (absolute position)*/
	void FK_Character::activateParticleEffect(core::vector3df position, bool guard){
		if (guard){
			guardSFX->setPosition(position);
			hitSFX->getEmitter()->setMaxParticlesPerSecond(0);
			hitSFX->getEmitter()->setMinParticlesPerSecond(0);
			guardSFX->getEmitter()->setMaxParticlesPerSecond(60);
			guardSFX->getEmitter()->setMinParticlesPerSecond(50);
			particleReferenceTime = 0;
		}
		else{
			if (isTriggerModeActive()){
				hitSFX->setMaterialTexture(0, hitSFXTextureTrigger);
			}
			else{
				hitSFX->setMaterialTexture(0, hitSFXTexture);
			}
			hitSFX->setPosition(position);
			guardSFX->getEmitter()->setMaxParticlesPerSecond(0);
			guardSFX->getEmitter()->setMinParticlesPerSecond(0);
			hitSFX->getEmitter()->setMaxParticlesPerSecond(60);
			hitSFX->getEmitter()->setMinParticlesPerSecond(50);
			particleReferenceTime = 0;
		}
	};

	scene::IParticleSystemSceneNode* FK_Character::getHitParticleEffect(){
		return hitSFX;
	}
	scene::IParticleSystemSceneNode* FK_Character::getGuardParticleEffect(){
		return guardSFX;
	}

	/* update particle effect visibility */
	void FK_Character::updateParticleEffect(f32 timeFrame_s){
		/* the time frame has to be converted in milliseconds*/
		particleReferenceTime += timeFrame_s * 1000.0f;
		if (hitSFX != NULL && guardSFX != NULL){
			if (hitSFX->getEmitter()->getMaxParticlesPerSecond() > 0){
				if (particleReferenceTime > particleExpirationTime){
					hitSFX->getEmitter()->setMaxParticlesPerSecond(0);
					hitSFX->getEmitter()->setMinParticlesPerSecond(0);
				}
			}
			else if (guardSFX->getEmitter()->getMaxParticlesPerSecond() > 0){
				if (particleReferenceTime > particleExpirationTime){
					guardSFX->getEmitter()->setMaxParticlesPerSecond(0);
					guardSFX->getEmitter()->setMinParticlesPerSecond(0);
				}
			}
		}
	}
	// Game logic section
	// get name
	std::string FK_Character::getName(){
		if (!availableCharacterOutfits[outfitId].outfitCharacterName.empty()){
			return availableCharacterOutfits[outfitId].outfitCharacterName;
		}
		return getOriginalName();
	};
	std::wstring FK_Character::getWName(){
		if (!availableCharacterOutfits[outfitId].outfitCharacterName.empty()){
			return availableCharacterOutfits[outfitId].outfitWCharacterName;
		}
		return getWOriginalName();
	};
	std::string FK_Character::getOriginalName(){
		return name;
	};
	std::wstring FK_Character::getWOriginalName(){
		return wname;
	};
	// get display name
	std::string FK_Character::getDisplayName(){
		if (!availableCharacterOutfits[outfitId].outfitDisplayName.empty()){
			return availableCharacterOutfits[outfitId].outfitDisplayName;
		}
		return displayName;
	};
	std::wstring FK_Character::getWDisplayName(){
		if (!availableCharacterOutfits[outfitId].outfitDisplayName.empty()){
			return availableCharacterOutfits[outfitId].outfitWDisplayName;
		}
		return wdisplayName;
	};
	/* get outfit id*/
	int FK_Character::getOutfitId(){
		return outfitId;
	};

	void FK_Character::setOutfitId(s32 newOutfitId){
		outfitId = newOutfitId;
	};

	std::deque<FK_Move> & FK_Character::getMovesCollection(){
		return movesCollection;
	}
	
	std::map<FK_Hurtbox_Type,scene::ISceneNode*> &FK_Character::getHurtboxCollection(){
		return hurtboxCollection;
	};
	std::map<FK_Hitbox_Type, scene::ISceneNode*> &FK_Character::getHitboxCollection(){
		return hitboxCollection;
	};
	/* get single bone from armature, if that bone exist for the selected character*/
	std::string FK_Character::getArmatureBone(FK_Bones_Type boneType){
		auto iterator = armatureBonesCollection.find(boneType);
		if (iterator == armatureBonesCollection.end()) {
			return std::string();
		}
		else {
			return iterator->second;
		}
	};
	std::string FK_Character::getMeshName(){
		return animatedMesh->getName();
	};
	core::vector3df FK_Character::getPosition(){
		return animatedMesh->getPosition();
	};
	core::vector3df FK_Character::getRotation(){
		return animatedMesh->getRotation();
	};
	void FK_Character::setPosition(core::vector3df newPosition){
		animatedMesh->setPosition(newPosition);
	};
	void FK_Character::setRotation(core::vector3df newRotation){
		animatedMesh->setRotation(newRotation);
	};
	/* "next movement" is a vector which contains the relative movement the character should be subjected to.
	the coordinates are: X = parallel movement (on-axis with enemy), Y = side movement (perpendicular to enemy) and
	Z = vertical movement. This array is read and processed by the SceneViewUpdater class, which handles the defintions
	of parallel and perpendicular direction*/
	void FK_Character::setNextMovement(core::vector3df new_movement){
		nextMovement = new_movement;
	}
	core::vector3df FK_Character::getNextMovement(){
		if (isImpactRunning() && currentMove != NULL){
			return core::vector3df(nextMovement.X * databaseAccessor.getRunCancelSpeedMultiplier(),
				nextMovement.Y, nextMovement.Z);
		}
		return nextMovement;
	}
	void FK_Character::resetNextMovement(){
		nextMovement = core::vector3df(0,0,0);
	}
	/* get and set velocity variables, used to determine next move */
	core::vector3df FK_Character::getVelocityPerSecond(){
		return velocityPerSecond;
	};
	void FK_Character::setVelocityPerSecond(core::vector3df new_velocity){
		velocityPerSecond = new_velocity;
	};
	/* get currently performed move*/
	FK_Move* FK_Character::getCurrentMove(){
		return currentMove;
	};
	/* get currently performed move*/
	FK_Move* FK_Character::getNextMove(){
		if (nextMove.size() > 0){
			return nextMove[0];
		}
		else{
			return NULL;
		}
	};
	/* get last move in the stack*/
	FK_Move* FK_Character::getLastStackedMove(){
		if (nextMove.size() > 0){
			return nextMove[nextMove.size()-1];
		}
		else{
			return currentMove;
		}
	}

	/* check if character has one move with active hitboxes */
	bool FK_Character::hasActiveMove(bool afterStartup)
	{
		if (currentMove == NULL) {
			return false;
		}
		if (afterStartup) {
			return currentMove->isActive(getCurrentFrame());
		}
		return currentMove->isStillActive(getCurrentFrame());
	}

	/* get current attack flag */
	FK_Attack_Type FK_Character::getCurrentAttackType() {
		if (currentMove == NULL) {
			return FK_Attack_Type::NoType;
		}else{
			s32 currentFrame = getCurrentFrame();
			FK_Attack_Type result = currentMove->getAttackTypeAtFrame(currentFrame);
			return result;
		}
	}

	
	/* get the currently displayed animation frame*/
	int FK_Character::getCurrentFrame(){
		return (int)animatedMesh->getFrameNr();
	};
	/* get the damage multiplier*/
	float FK_Character::getDamageScalingMultiplier(){
		return damageScalingMultiplier;
	};
	/* set new life */
	void FK_Character::setLife(float newLifeValue){
		life = newLifeValue;
		if (life <= 0) life = 0;
	};
	/* apply damage*/
	void FK_Character::receiveDamage(float damage, bool toObjects){
		setLife(life - damage);
		if (damage >= 0 && toObjects){
			applyDamageToObjects(damage);
		}
	}
	/* get current life */
	float FK_Character::getLife(){
		return life;
	};
	/* get maximum life*/
	float FK_Character::getMaxLife(){
		return max(1.0f, maxLife * lifeScalingFactor);
	};
	/* get life ration*/
	float FK_Character::getLifeRatio(){
		return life / getMaxLife();
	};
	/* get life percentage (ratio * 100) */
	float FK_Character::getLifePercentage(){
		return 100.0f*getLifeRatio();
	};
	/* restore life to maximum */
	void FK_Character::setLifeToMax(){
		life = getMaxLife();
	};
	/* check if the character is KO (life < 0)*/
	bool FK_Character::isKO(){
		return life <= 0;
	}
	/* check if the character has full life*/
	bool FK_Character::hasFullLife(){
		return life >= getMaxLife();
	}
	/* trigger section */
	/* check if can trigger combo */
	bool FK_Character::canTriggerCombo(){
		if (!databaseAccessor.triggerCombosAllowed()) {
			return false;
		}
		return canAcceptInput() && getTriggerCounters() >= databaseAccessor.getTriggerCountersPerActivation();
	};
	/* check if can trigger a revenge counter */
	bool FK_Character::canRevengeTrigger(){
		if (!databaseAccessor.triggerGuardAllowed()) {
			return false;
		}
		bool trigger_flag = true;
		trigger_flag &= getTriggerCounters() >= databaseAccessor.getTriggerCountersPerActivation();
		trigger_flag &= !isBeingThrown();
		trigger_flag &= isHitStun();
		trigger_flag &= getStance() == FK_Stance_Type::GroundStance ||
			getStance() == FK_Stance_Type::CrouchedStance ||
			getStance() == FK_Stance_Type::WakeUpStance;
		trigger_flag &= velocityPerSecond.Z == 0;
		trigger_flag &= !isGuarding();
		return !triggerButtonPressedBeforeHitstun && trigger_flag;
	};
	/* get number of triggers left */
	int FK_Character::getTriggerCounters(){
		return triggerCounters;
	};

	/* set number of triggers left */
	void FK_Character::setTriggerCounters(s32 numberOfCounters){
		triggerCounters = numberOfCounters;
	};

	/* check if player is grounded */
	bool FK_Character::isGrounded(){
		return (getStance() == FK_Stance_Type::SupineStance ||
			getStance() == FK_Stance_Type::ProneStance);
	}

	/* check if the player is triggering the attack */
	bool FK_Character::isTriggering(){
		return triggerFlag;
	};
	/* check if the player is suffering a trigger combo*/
	bool FK_Character::isTriggerComboed(){
		return triggerComboedFlag;
	}
	/* set trigger comboed flag*/
	void FK_Character::setTriggerComboedFlag(){
		triggerComboedFlag = true;
	};
	/* cancel trigger comboed flag*/
	void FK_Character::cancelTriggerComboedFlag(){
		triggerComboedFlag = false;
	}
	/* check if trigger mode is active */
	bool  FK_Character::isTriggerModeActive(){
		return triggerModeFlag || (/*currentMove != NULL &&*/ impactCancelRun_flag);
	};
	/* check if trigger guard is active */
	bool FK_Character::isTriggerGuardActive(){
		return isGuarding() && isTriggerModeActive();
	};
	void FK_Character::updateTriggerGuard(f32 delta_t_s){
		if (triggerGuardDurationS >= 0 && isTriggerModeActive()){
			triggerGuardDurationS += delta_t_s;
			if (currentMove != NULL || 
				triggerGuardDurationS > (f32)(databaseAccessor.getTriggerGuardDurationMs())/1000.f){
				cancelTriggerMode();
				triggerGuardDurationS = -1;
			}
		}
	};
	/* activate trigger mode*/
	void FK_Character::activateTriggerMode(){
		u32 numberOfTriggerCountersUsed = databaseAccessor.getTriggerCountersPerActivation();
		if ((u32)getTriggerCounters() >= numberOfTriggerCountersUsed){
			triggerModeFlag = true;
			useTriggerCounters(numberOfTriggerCountersUsed);
			stats.numberOfTriggerCombos += 1;
		}
	}

	/* set trigger flag */
	void FK_Character::setTriggerModeFlag() {
		triggerModeFlag = true;
	}
	/* abort trigger mode*/
	void FK_Character::cancelTriggerMode(){
		triggerModeFlag = false;
		impactCancelRun_flag = false;
		triggerGuardDurationS = -1;
	}
	/* increase trigger counters*/
	void FK_Character::addTriggerCounters(int value){
		triggerCounters += value;
		// clamp value to maximum
		if (triggerCounters > databaseAccessor.getMaxTriggers()){
			triggerCounters = databaseAccessor.getMaxTriggers();
		}
	};
	/* use trigger counters */
	void FK_Character::useTriggerCounters(int value){
		triggerCounters -= value;
		stats.numberOfUsedBullets += value;
		// clamp value to zero
		if (triggerCounters < 0){
			triggerCounters = 0;
		}
	};

	/* mark player for impact cancel check */
	void FK_Character::allowForImpactCancelCheck(){
		impactCancelTimerCounter = FK_Character::MaximumTimeImpactCancel;
		canCheckForImpactCancelInput = true;
	}

	/* check if the character is impact-running */
	bool FK_Character::isImpactRunning(){
		return isRunning() && impactCancelRun_flag;
	}

	/* check if the character is impact-running */
	bool FK_Character::isInImpactCancelState(){
		return impactCancelRun_flag;
	}

	/* update checks for impact cancel */
	void FK_Character::updateImpactCancelCheck(u32 delta_t_ms){
		if (!databaseAccessor.runCancelAllowed()) {
			canCheckForImpactCancelInput = false;
			return;
		}
		// update impact cancel timer
		if (canCheckForImpactCancelInput){
			impactCancelTimerCounter -= (s32)delta_t_ms;
			if (impactCancelTimerCounter < 0){
				canCheckForImpactCancelInput = false;
			}
		}
	}

	/* check for Impact Cancel availability*/
	void FK_Character::checkForImpactCancel(u32 lastButtonPressed){
		// impact cancel is aborted, in the current release (as of 19.08.2017)
		//canCheckForImpactCancelInput = false;
		if (!databaseAccessor.runCancelAllowed()) {
			canCheckForImpactCancelInput = false;
			return;
		}
		if (hasUsedImpactCancelInCurrentCombo){
			return;
		}
		if (isHitStun()){
			return;
		}
		if (currentMove != NULL &&
			currentMove->getEndStance() == FK_Stance_Type::AirStance){
			return;
		}
		if (getNextMove() != NULL){
			return;
		}
		if (triggerCounters < fk_constants::FK_CountersPerImpactCancelActivation){
			return;
		}
		if (!isTriggering()){
			return;
		}
		u32 inputButtonsPressed = lastButtonPressed & FK_Input_Buttons::Any_NonDirectionButton;
		u32 directionToCheck = isOnLeftSide() ? FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
		lastButtonPressed &= FK_Input_Buttons::Any_Direction_Plus_Held;
		lastButtonPressed &= ~FK_Input_Buttons::Down_Direction;
		if ((inputButtonsPressed & FK_Input_Buttons::Guard_Button) != 0 && (lastButtonPressed == directionToCheck)){
			performImpactCancel();
			impactCancelShaderEffectTimer = ArmorSpecialEffectDurationMS;
		}
		else if ((inputButtonsPressed & FK_Input_Buttons::Guard_Button) != 0) {
			performStandingCancel();
			impactCancelShaderEffectTimer = ArmorSpecialEffectDurationMS;
		}
	}

	/* check perform Impact Cancel*/
	void FK_Character::performImpactCancel(){
		canCheckForImpactCancelInput = false;
		useTriggerCounters(databaseAccessor.getTriggerCountersPerRunCancelActivation());
		resetMove();
		triggerFlag = false;
		setStance(FK_Stance_Type::RunningStance);
		impactCancelRun_flag = true;
		hasUsedImpactCancelInCurrentCombo = true;
		delayAfterMoveTimeMs = 100;
		delayMovementAfterMoveTimeMs = 0;
		stats.numberOfTriggerCancels += 1;
	}

	/* check perform Impact Cancel*/
	void FK_Character::performStandingCancel() {
		canCheckForImpactCancelInput = false;
		useTriggerCounters(databaseAccessor.getTriggerCountersPerRunCancelActivation());
		resetMove();
		triggerFlag = false;
		setStance(FK_Stance_Type::GroundStance);
		impactCancelRun_flag = false;
		hasUsedImpactCancelInCurrentCombo = true;
		delayAfterMoveTimeMs = 0;
		delayMovementAfterMoveTimeMs = 0;
		stats.numberOfTriggerCancels += 1;
	}
	/* manage combo counter*/
	void FK_Character::increaseComboCounter(f32 damage, s32 moveId, s32 hitboxId){
		comboCounter += 1;
		comboDamage += damage;
		stats.totalDamage += damage;
		if (comboDamage > stats.maxComboDamage) {
			stats.maxComboDamage = comboDamage;
		}
		if (comboCounter > stats.maxComboLength) {
			stats.maxComboLength = comboCounter;
		}
		if (moveId >= 0 && hitboxId >= 0) {
			FKComboUnit unit(moveId, hitboxId);
			// create if not there
			currentComboLog[unit] += 1;
		}
	};
	void FK_Character::resetComboCounter(){
		comboCounter = 0;
		comboDamage = 0;
		currentComboLog.clear();
	};

	u32 FK_Character::getComboCounter() const{
		return comboCounter;
	};

	s32 FK_Character::getComboDamage() const{
		return (s32)std::ceil(comboDamage);
	}

	// repetition number, used in the infinite prevention system
	u32 FK_Character::getNumberOfRepetitionOfSameMoveInCombo(s32 moveId, s32 hitboxId) const
	{
		FKComboUnit unit(moveId, hitboxId);
		if (currentComboLog.find(unit) != currentComboLog.end()) {
			return currentComboLog.at(unit);
		}
		return 0;
	}
	


	/* set/get stance */
	void FK_Character::setStance(FK_Stance_Type stance){
		currentStance = stance;
	}
	FK_Stance_Type FK_Character::getStance(){
		return currentStance;
	};
	/* reset current move */
	void FK_Character::resetMove(bool resetTrigger){
		isSidestepping = false;
		sidestepCounter = 0;
		resetMoveDelayCounter();
		disableArmorSpecialEffect();
		isTransforming = false;
		moveIsPartOfAChain = false;
		if (resetTransitionTime){
			animatedMesh->setTransitionTime(animationTransitionTime);
			resetTransitionTime = false;
		}
		isPerformingAirAttack = false;
		hasUsedImpactCancelInCurrentCombo = false;
		impactCancelRun_flag = false;
		currentMove = NULL;
		storageMove = NULL;
		nextMove.clear();
		setDirectionLock(false);
		setLockedDirection(core::vector3df(0, 0, 0));
		resetNextMovement();
		setGroundCollisionFlag(false);
		/* abort trigger mode if no new move is inputed */
		if (resetTrigger && !throwProcessingFlag){
			movesPerformedInCurrentCombo.clear();
		}
		if (resetTrigger && isTriggerModeActive()){
			cancelTriggerMode();
		}
	}

	void FK_Character::applyNewMove(FK_Move* newMove){
		if (newMove == NULL){
			return;
		}
		if (newMove->getNumberOfRequiredBulletCounters() > 0){
			useTriggerCounters(newMove->getNumberOfRequiredBulletCounters());
		}
		if (newMove->getTriggerModeActivationFlag() && databaseAccessor.triggerCombosAllowed()) {
			triggerModeFlag = true;
		}
		runningBufferTimerMs = -1;
		runningBufferTimerMsTarget = -1;
		lastMoveNoHitboxFlag = false;
		plannedMove.reset();
		disableArmorSpecialEffect();
		canCheckForImpactCancelInput = false;
		recoverFromGrounding(false);
		moveIsPartOfAChain = currentMove != NULL && !currentMove->canTrackAfterAnimation();
		currentMove = newMove;
		/*if (jumpingFlag && newMove->getStance() != FK_Stance_Type::AirStance) {
			jumpingFlag = false;
			velocityPerSecond.Z = 0;
		}*/
		if (newMove->getStance() == FK_Stance_Type::AirStance){
			isPerformingAirAttack = true;
		}
		else{
			isPerformingAirAttack = false;
		}
		movesPerformedInCurrentCombo.push_back(currentMove->getName());
		setLockedDirection(core::vector3df(0, 0, 0));
		setDirectionLock(newMove->getDirectionLockFlag());
		currentMove->resetHitboxesHitFlag();
		currentMove->resetSoundsPlayed();
		resetNextMovement();
		setGroundCollisionFlag(false);
		if (newMove->canTrackAtStartup()){
			setTrackingFlag(true);
		}
		//nextMove = NULL;
		if (nextMove.size() > 0){
			nextMove.pop_front();
		}
		core::stringw newAnimationName = core::stringw(newMove->getAnimationName(isOnLeftSide()).c_str());
		std::string newAnimationNameString = newMove->getAnimationName(isOnLeftSide());
		core::stringw tempCurrentAnimationName = getCurrentAnimationName();
		bool newAnimationFlag = newAnimationName != tempCurrentAnimationName;
		/* call new animation from database */
		if (newAnimationFlag){
			bool loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), animationDirectory + newAnimationNameString + ".x");
			//bool loaded = false;
			if (!loaded){
				loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), sharedAnimationDirectory + newAnimationNameString + ".x");
			}
			if (loaded){
				setAnimation(newAnimationNameString.c_str());
			}
			else{
				currentAnimationName = newAnimationName;
			}
		}
		// set correct stance
		setStance(currentMove->getEndStance());
		animatedMesh->setTransitionTime(animationTransitionTime);
		lastFrame = -1;
		lastFrameForObjectUpdate = -1;
		animatedMesh->setFrameLoop(currentMove->getStartingFrame(), currentMove->getEndingFrame());
		for (int k = 0; k < characterAdditionalObjects.size(); ++k){
			if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
				characterAdditionalObjectsMeshs[k]->setFrameLoop(currentMove->getStartingFrame(), currentMove->getEndingFrame());
				characterAdditionalObjectsMeshs[k]->setLoopMode(false);
			}
		}
	}

	/* check if move is already in current combo */
	bool FK_Character::isMoveInCurrentCombo(FK_Move* moveToTest){
		if (moveToTest->canBeUsedMultipleTimesInChain()) {
			return false;
		}
		if (std::find(movesPerformedInCurrentCombo.begin(),
			movesPerformedInCurrentCombo.end(), moveToTest->getName()) ==
			movesPerformedInCurrentCombo.end()){
			return false;
		}
		return true;
	}

	/* perform new move, if move is not NULL*/
	bool FK_Character::performMove(FK_Move* newMove, bool forceExecution){
		/* check if the new move is a followup. If so, put it in queue*/
		bool movePerformed = false;
		if (newMove != NULL && newMove->getNumberOfRequiredBulletCounters() > getTriggerCounters()){
			return movePerformed;
		}
		if (canPerformNewMove(newMove) || forceExecution){
			if (newMove != NULL && !forceExecution && delayAfterMoveTimeCounterMs < delayAfterMoveTimeMs &&
				!lastMoveNoHitboxFlag) {
				// running buffer condition
				bool runningMoveFlag =
					isRunning() &&
					newMove->getStance() == FK_Stance_Type::RunningStance &&
					newMove->getEndStance() != FK_Stance_Type::RunningStance;
				if (!runningMoveFlag) {
					return false;
				}
				plannedMove.reset();
				plannedMove.activationFrame = 0;
				plannedMove.move = newMove;
				movesPerformedInCurrentCombo.push_back(newMove->getName());
				return false;
			}
			else if (!forceExecution){
				float currentFrame = (f32)getCurrentFrame();
				// check if the new move can cancel the old move
				if (getCurrentMove() != NULL && newMove != NULL) {
					//std::cout << "current move (cancel)" << getLastStackedMove()->getName() << " frame:" << currentFrame << std::endl;
					int size = getCurrentMove()->getCancelIntoMovesSet().size();
					for (int i = 0; i < size; i++) {
						FK_FollowupMove tempMove = getCurrentMove()->getCancelIntoMovesSet()[i];
						/*std::cout << i + 1 << "/" << size << " " << tempMove.getName() << " " <<newMove->getName()<<
							" frame:" << tempMove.getInputWindowStartingFrame() << " " << tempMove.getInputWindowEndingFrame() << std::endl;*/
							//std::cout << "moves: " << movesPerformedInCurrentCombo.size() << std::endl;
						if (newMove->getMoveId() == tempMove.getMoveId() &&
							currentFrame >= tempMove.getInputWindowStartingFrame() &&
							currentFrame <= tempMove.getInputWindowEndingFrame()) {
							if (!isMoveInCurrentCombo(newMove)) {
								// reset trigger if needed
								u32 hitboxSize = currentMove->getHitboxCollection().size();
								if (isTriggerModeActive()) {
									bool abortTrigger = hitboxSize > 0 && !currentMove->hasConnectedWithTarget();
									bool noActiveHitbox = true;
									if (abortTrigger) {
										for (int i = 0; i < hitboxSize; i++) {
											if (noActiveHitbox) {
												if (currentMove->getHitboxCollection()[i].getEndingFrame() <= currentFrame) {
													noActiveHitbox = false;
												}
											}
											if (currentMove->getHitboxCollection()[i].hasHit()) {
												abortTrigger = false;
												break;
											}
										}
									}
									if (abortTrigger && !noActiveHitbox) {
										cancelTriggerMode();
									}
								}
								//if (newMove != NULL) std::cout << "followUp: " << newMove->getName() << std::endl;
								applyNewMove(newMove);
								movePerformed = true;
								break;
							}
						}
						// ease of use buffer for planned moves
						else if (newMove->getMoveId() == tempMove.getMoveId() &&
							(currentFrame >= tempMove.getInputWindowStartingFrame() - 
								MaximumNumberOfBufferFramesForCancel) &&
							currentFrame <= tempMove.getInputWindowEndingFrame()) 
						{
							if (!isMoveInCurrentCombo(newMove)) {
								plannedMove.reset();
								plannedMove.activationFrame = tempMove.getInputWindowStartingFrame();
								plannedMove.move = newMove;
								movesPerformedInCurrentCombo.push_back(tempMove.getName());
							}
						}
					}
				}
				if (!movePerformed) {
					// check for followup
					if (getLastStackedMove() != NULL && newMove != NULL) {
						//std::cout << "current move " << getLastStackedMove()->getName() << std::endl;
						int size = getLastStackedMove()->getFollowupMovesSet().size();
						for (int i = 0; i < size; i++) {
							FK_FollowupMove tempMove = getLastStackedMove()->getFollowupMovesSet()[i];
							//std::cout << i + 1 << "/" << size << " " << tempMove.getName() << " " << newMove->getName() << std::endl;
							if (newMove->getMoveId() == tempMove.getMoveId() &&
								((currentFrame >= tempMove.getInputWindowStartingFrame() &&
									currentFrame <= tempMove.getInputWindowEndingFrame()) ||
									tempMove.getInputWindowStartingFrame() == getLastStackedMove()->getStartingFrame())) {
								// if the same move is already part of the move chain, don't add it.
								//std::cout << "(FWP) moves: " << movesPerformedInCurrentCombo.size() << std::endl;
								if (!isMoveInCurrentCombo(newMove)) {
									movesPerformedInCurrentCombo.push_back(tempMove.getName());
									nextMove.push_back(newMove);
									//if (newMove != NULL) std::cout << "followUp: " << newMove->getName() << std::endl;
								}
								return false;
							}
						}
					}
				}
			}
			/* else, do the usual checks*/
			if (forceExecution || (currentMove == NULL && newMove != NULL)){// ||
				//((currentMove != NULL && newMove != NULL) && currentMove->getName() != newMove->getName())){
				/* exit the process if the required stance does not match with the player's*/
				FK_Stance_Type stanceToCheck = getStance();
				if (currentMove != NULL){
					stanceToCheck = currentMove->getEndStance();
				}
				if (!forceExecution && newMove->getStance() != stanceToCheck){
					resetMove();
					//nextMove = NULL;
					nextMove.clear();
					return false;
				}
				// make an exception for air moves
				if (stanceToCheck == FK_Stance_Type::AirStance && newMove->getStance() != FK_Stance_Type::AirStance) {
					if (currentMove != NULL && forceExecution) {
						isPerformingAirAttack = true;
						storageMove = currentMove;
						currentMove = NULL;
						setBasicAnimation(FK_BasicPose_Type::JumpingAnimation, true, false, false);
					}
					//resetMove();
					//nextMove = NULL;
					//nextMove.clear();
					return false;
				}
				if (newMove->getStance() == FK_Stance_Type::AirStance && jumpingFlag){
					if (!canPerformJumpAttack()){
						return false;
					}
				}
				//bool mergeFlag = false;
				if (currentMove != NULL && !forceExecution){
					movePerformed = false;
				}
				else{
					// assign new move
					applyNewMove(newMove);
					movePerformed = true;
				}
				animatedMesh->setLoopMode(false);
				animatedMesh->animateJoints();
				//resetNextMovement();
			};
		}
		/* if the trigger button is pressed and a move is started, use one indicator and enter trigger mode */
		if (movePerformed && isTriggering() && !isTriggerModeActive() && currentMove->canBeTriggered()){
			activateTriggerMode();
		}
		return movePerformed;
	};
	// this method is invoked in the FK_SceneViewUpdater object, in order to make less calculations
	void FK_Character::setOnLeftSide(bool onLeft){
		isLeftPlayer = onLeft;
	};
	/* return true if player is on the left side of the screen */
	bool FK_Character::isOnLeftSide(){
		return isLeftPlayer;
	};
	/* this function is still to be defined properly*/
	bool FK_Character::canAcceptInput(){
		return !isKO() && !isBeingThrown() && !isHitStun() && !getRingoutFlag();
	};
	/* this function checks if the player can move*/
	bool FK_Character::canMove(){
		if (getStance() == FK_Stance_Type::WakeUpStance){
			return false;
		}
		return canAcceptInput() && delayAfterMoveTimeCounterMs >= delayMovementAfterMoveTimeMs;
	};
	/* this function states if the player can perform a new move*/
	bool FK_Character::canPerformNewMove(FK_Move* newMove){
		if (newMove == NULL) {
			return false;
		}
		else {
			bool guardFlag = !isGuarding() || newMove->canCancelGuard();
			bool runningMoveFlag =
				isRunning() &&
				newMove->getStance() == FK_Stance_Type::RunningStance &&
				newMove->getEndStance() != FK_Stance_Type::RunningStance;
			bool damagingMoveAfterNonTriggerMove =
				lastMoveNoHitboxFlag &&
				(newMove->canBeTriggered() || newMove->getTotalBulletDamage() > 0);
			return guardFlag && canMove() && (damagingMoveAfterNonTriggerMove || 
				runningMoveFlag || delayAfterMoveTimeCounterMs >= delayAfterMoveTimeMs);
		}
		return false;
	};
	/* this function manages the delay counter to introduce some time delay between various unchained moves */
	void FK_Character::updateMoveDelayCounter(f32 delta_t_s){
		delayAfterMoveTimeCounterMs += (u32)(delta_t_s * 1000.0f);
		//if (delayAfterMoveTimeMs > 0) {
		//	std::cout << delayAfterMoveTimeCounterMs << "/" << delayAfterMoveTimeMs << " (" << delayMovementAfterMoveTimeMs << ")" << std::endl;
		//}
		if (delayAfterMoveTimeCounterMs >= delayAfterMoveTimeMs) {
			delayAfterMoveTimeCounterMs = 0;
			delayMovementAfterMoveTimeMs = 0;
			delayAfterMoveTimeMs = 0;
		}
	}
	/* reset move delay counter after a move is completed */
	void FK_Character::resetMoveDelayCounter(){
		//std::cout << "move end - delay start" << std::endl;
		delayAfterMoveTimeMs = 0;
		delayMovementAfterMoveTimeMs = 0;
		delayAfterMoveTimeCounterMs = 0;
		delayAfterMoveTimeMs = databaseAccessor.getCharacterMoveCooldownDurationMs();
		//if (currentMove != NULL && currentMove->isThrow()){
		//	delayAfterMoveTimeMs += 50;
		//}
		if (isTriggerModeActive()){
			delayAfterMoveTimeMs = (u32)floor((f32)delayAfterMoveTimeMs * 1.5f);
		}
		if (currentMove != NULL){
			if (!(currentMove->getDelayAfterMoveFlag())){
				delayAfterMoveTimeMs = 0;
			}
			else if(currentMove->getDelayAfterMoveTime() >= 0){
				delayAfterMoveTimeMs = currentMove->getDelayAfterMoveTime();
			}
			if (currentMove->getStance() != FK_Stance_Type::RunningStance &&
				currentMove->getEndStance() == FK_Stance_Type::RunningStance){
				delayAfterMoveTimeMs = (u32)std::ceil((f32)delayAfterMoveTimeMs * 2.0f / 3);
			}
			if (isTriggerModeActive() && !currentMove->isThrow()){
				delayAfterMoveTimeMs = (u32)floor((f32)delayAfterMoveTimeMs * 1.5f);
			}
		}
		delayMovementAfterMoveTimeMs = delayAfterMoveTimeMs;
		if (currentMove != NULL && (!currentMove->canBeTriggered() && !currentMove->isThrow()) && currentMove->getBulletCollection().empty()){
			if (currentMove->getEndStance() != FK_Stance_Type::RunningStance) {
				delayAfterMoveTimeMs = 0;
			}
			delayMovementAfterMoveTimeMs = 0;
		}
		delayAfterMoveTimeCounterMs = 0;
	}
	/* check and set direction lock */
	bool FK_Character::isDirectionLocked(){
		return directionLock;
	};
	void FK_Character::setDirectionLock(bool new_directionLock, bool safetyLock){
		if (safetyLock && directionLock != new_directionLock) {
			safetyDirectionLock = true;
			originalDirectionLock = directionLock;
		}
		directionLock = new_directionLock;
	};

	void FK_Character::revertDirectionLock() {
		if (safetyDirectionLock) {
			safetyDirectionLock = false;
			directionLock = originalDirectionLock;
			if (!directionLock) {
				lockedDirection = core::vector3df(0, 0, 0);
			}
		}
	}

	core::vector3df FK_Character::getLockedDirection(){
		return lockedDirection;
	};
	void FK_Character::setLockedDirection(core::vector3df new_Direction){
		lockedDirection = new_Direction;
	};
	bool FK_Character::hasToSetLockedDirection(){
		return (lockedDirection == core::vector3df(0, 0, 0));
	};

	bool FK_Character::isCurrentMovePartOfAChain() {
		return moveIsPartOfAChain;
	}

	/* Reactions to hit */
	bool FK_Character::isHitStun(){
		//if (isGrounded() || (getVelocityPerSecond().Z == 0 && isKO())){
		//	return false;
		//}
		return (hitStunFlag);// || getStance() == FK_Stance_Type::LandingStance);
	};
	/* reset variables */
	void FK_Character::resetVariables(bool resetObjects){
		// restore proper mesh visibility
		getAnimatedMesh()->setVisible(true);
		for (int i = 0; i < characterAdditionalObjects.size(); ++i){
			if (characterAdditionalObjects[i].canBeRestored ||
				characterAdditionalObjects[i].resetBetweenRounds ||
				resetObjects){
				characterAdditionalObjects[i].resetFlags();
			}
			if (characterAdditionalObjects[i].isActive() && !characterAdditionalObjects[i].hasDummyMesh()){
				characterAdditionalObjectsMeshs[i]->setVisible(true);
			}
			if (characterAdditionalObjects[i].animateWithMesh) {
				characterAdditionalObjectsMeshs[i]->setCurrentFrame(0, true);
				characterAdditionalObjectsMeshs[i]->OnRegisterSceneNode();
			}
		}

		hitStunFlag = false;
		setStance(FK_Stance_Type::GroundStance);
		setLockedDirection(core::vector3df(0, 0, 0));
		setVelocityPerSecond(core::vector3df(0, 0, 0));
		hitStunReferenceTime = 0;
		hitStunDuration = 0;
		isPerformingAirAttack = false;
		isThrown = false;
		guardFlag = false;
		cancelJump();
		cancelRunning();
		resetMoveDelayCounter();
		resetMove();
		resetNextMovement();
		wasCollisionEllipsoidRefreshed = false;
		setRingoutFlag(false);
		setInStageRingoutZoneFlag(false);
		setCorneredFlag(false);
		setRingWallCollisionFlag(false);
		bouncingCounter = 0;
		throwProcessingFlag = false;
		isTransforming = false;
		transformationToApply = FK_Move::FK_TransformationMove();
		if (resetObjects) {
			stats.clear();
		}
		if (resetObjects || currentForm.resetEveryRound){
			if (!isInBaseForm) {
				currentForm.durationMs = 16;
			}
			resetStartupTextures();
		}
	}

	// throw processing flag
	void FK_Character::setThrowProcessingFlag(bool flag){
		throwProcessingFlag = flag;
	}

	void FK_Character::setReaction(FK_Reaction_Type reactionType, 
		bool guardBreakTrigger, f32 hitDirection, f32 hitstunMultiplier){
		hitStunFlag = true;
		lastMoveNoHitboxFlag = false;
		triggerButtonPressedBeforeHitstun = true;
		resetCanSufferRingoutFlag();
		plannedMove.reset();
		if (isGrounded()){
			hitWhileGrounded = true;
		}
		if (isGrounded() || !isHitStun()) {
			setVelocityPerSecond(core::vector3df(0, 0, 0));
		}
		if (databaseAccessor.extendedJugglesAllowed()) {
			ISceneNodeAnimator* pAnim = *(getAnimatedMesh()->getAnimators().begin());
			if (pAnim != NULL && pAnim->getType() == ESNAT_COLLISION_RESPONSE) {
				auto gravity = ((scene::ISceneNodeAnimatorCollisionResponse*)pAnim)->getGravity();
				((scene::ISceneNodeAnimatorCollisionResponse*)pAnim)->setGravity(gravity);
			}
		}
		if (guardBreakTrigger && !isKO()){
			if (reactionType == FK_Reaction_Type::StrongFlight || reactionType == FK_Reaction_Type::ReverseStrongFlight ||
				reactionType == FK_Reaction_Type::StandingFlight || reactionType == FK_Reaction_Type::WeakFlight){
				reactionType = FK_Reaction_Type::StrongMedium;
			}
		}
		if (isAirborne() || (currentMove != NULL && velocityPerSecond.Z != 0)){
			if ((reactionType & FK_Reaction_Type::AnyStrongReaction) != 0){
				reactionType = FK_Reaction_Type::StrongFlight;
			}
			else if (reactionType & FK_Reaction_Type::AnyWeakReaction){
				//reactionType = (FK_Reaction_Type)(reactionType << FK_Reaction_Type::ReactionBitShift_WeakToStrong);
				reactionType = FK_Reaction_Type::WeakFlight;
			}
		}
		/* cancel throw if the reaction is not "NONE" */
		if (reactionType != FK_Reaction_Type::NoReaction){
			clearThrow();
		}
		/* cancel current move */
		resetMove();
		/* reset hitStun timer */
		hitStunReferenceTime = 0;
		/* set hitstun duration */
		setLockedDirection(core::vector3df(0, 0, 0));
		u32 baseHitStunDuration = databaseAccessor.getWeakHitstunDurationMs();
		u32 strongHitStunDuration = databaseAccessor.getStrongHitstunDurationMs();
		u32 maximumHitStunDuration = databaseAccessor.getMaxHitstunDurationMs();
		f32 maxParallelStunVelocity = databaseAccessor.getMaxTransversalSpeedAfterHit();
		f32 parallelStunVelocity = 2 * maxParallelStunVelocity * (f32)rand() / RAND_MAX - maxParallelStunVelocity;
		// test for consistency
		if (reactionType != FK_Reaction_Type::ReverseStrongFlight && reactionType != FK_Reaction_Type::StrongFlight) {
			parallelStunVelocity = 0;
		}

		bool wasWakingUp = (getStance() == FK_Stance_Type::WakeUpStance);
		if (wasWakingUp && isGuarding()){
			setVelocityPerSecond(core::vector3df(0, 0, 0));
			setBasicAnimation(FK_BasicPose_Type::GuardingAnimation, true);
		}
		if (!isKO()){
			if (getStance() == FK_Stance_Type::LandingStance) {
				if (reactionType != FK_Reaction_Type::StrongFlight &&
					reactionType != FK_Reaction_Type::ReverseStrongFlight &&
					reactionType != FK_Reaction_Type::StandingFlight) {
					reactionType = FK_Reaction_Type::WeakFlight;
				}
			}
			else if (isGrounded()){
				reactionType = FK_Reaction_Type::WeakFlight;
			}
			if (wasWakingUp){
				if (reactionType == FK_Reaction_Type::StrongFlight ||
					reactionType == FK_Reaction_Type::ReverseStrongFlight){
					reactionType = FK_Reaction_Type::StandingFlight;
				}
				velocityPerSecond = core::vector3df(0.f, 0.f, 0.f);
			}
		}
		/* if reaction type is STRONG_FLIGHT, check for item breaking */
		if (!isBeingThrown() && reactionType != FK_Reaction_Type::NoReaction && !guardBreakTrigger && !isGuarding()){
			checkForBreakingObjects(reactionType);
		}
		/* if a move is triggered by a broken object, break! */
		if (currentMove != NULL) {
			hitStunFlag = false;
			hitStunDuration = 0;
			hitStunMovementDuration = 0;
			return;
		}
		/* change reaction if character is being triggered */
		if (/*!isGrounded() && */(isJumping() || velocityPerSecond.Z == 0)){
			if (isJumping()){
				cancelJump();
			}
			if (!isGuarding()){
				setStance(FK_Stance_Type::GroundStance);
			}
			switch (reactionType){
			case FK_Reaction_Type::NoReaction:
				hitStunDuration = baseHitStunDuration;
				hitStunMovementDuration = databaseAccessor.getWeakHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(0, 0, 0));
				break;
			case FK_Reaction_Type::SmackdownLanding:
				hitStunDuration = 2 * baseHitStunDuration;
				hitStunMovementDuration = databaseAccessor.getWeakHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-200, 0, 0));
				if (!isGuarding()){
					if (hitDirection <= 0){
						setBasicAnimation(FK_BasicPose_Type::SupineLandingAnimation, true);
						setStance(FK_Stance_Type::LandingStance);
					}
					else{
						setBasicAnimation(FK_BasicPose_Type::HitStrongMediumBackAnimation, true);
					}
				}
				break;
			case FK_Reaction_Type::WeakHigh:
				hitStunDuration = baseHitStunDuration;
				hitStunMovementDuration = databaseAccessor.getWeakHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					if (isTriggerComboed()){
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitStrongHighAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitStrongHighBackAnimation, true);
						}
					}
					else{
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitWeakHighAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitWeakHighBackAnimation, true);
						}
					}
				}
				break;
			case FK_Reaction_Type::WeakMedium:
				hitStunDuration = baseHitStunDuration;
				hitStunMovementDuration = databaseAccessor.getWeakHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					if (isTriggerComboed()){
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitStrongMediumAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitStrongMediumBackAnimation, true);
						}
					}
					else{
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitWeakMediumAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitWeakMediumBackAnimation, true);
						}
					}
				}
				break;
			case FK_Reaction_Type::WeakLow:
				hitStunDuration = baseHitStunDuration;
				hitStunMovementDuration = databaseAccessor.getWeakHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					if (isTriggerComboed()){
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitStrongLowAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitStrongLowBackAnimation, true);
						}
					}
					else{
						if (hitDirection <= 0){
							setBasicAnimation(FK_BasicPose_Type::HitWeakLowAnimation, true);
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::HitWeakLowBackAnimation, true);
						}
					}
				}
				break;
			case FK_Reaction_Type::StrongHigh:
				hitStunDuration = baseHitStunDuration;
				//hitstunMultiplier = 1;
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					hitStunDuration = strongHitStunDuration;
					if (hitDirection <= 0){
						setBasicAnimation(FK_BasicPose_Type::HitStrongHighAnimation, true);
					}
					else{
						setBasicAnimation(FK_BasicPose_Type::HitStrongHighBackAnimation, true);
					}
				}
				else {
					velocityPerSecond.X -= 30.0;
				}
				break;
			case FK_Reaction_Type::StrongMedium:
				hitStunDuration = baseHitStunDuration;
				//hitstunMultiplier = 1;
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					hitStunDuration = strongHitStunDuration;
					if (hitDirection <= 0){
						setBasicAnimation(FK_BasicPose_Type::HitStrongMediumAnimation, true);
					}
					else{
						setBasicAnimation(FK_BasicPose_Type::HitStrongMediumBackAnimation, true);
					}
				}
				else {
					velocityPerSecond.X -= 30.0;
				}
				break;
			case FK_Reaction_Type::StrongLow:
				hitStunDuration = baseHitStunDuration;
				//hitstunMultiplier = 1;
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					hitStunDuration = strongHitStunDuration;
					if (hitDirection <= 0){
						setBasicAnimation(FK_BasicPose_Type::HitStrongLowAnimation, true);
					}
					else{
						setBasicAnimation(FK_BasicPose_Type::HitStrongLowBackAnimation, true);
					}
				}
				else {
					velocityPerSecond.X -= 30.0;
				}
				break;
			case FK_Reaction_Type::StrongFlight:
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity / 2, 0));
				if (isGuarding()) {
					velocityPerSecond.X -= 135.0;
				}
				else {
					velocityPerSecond.Z = 160.0;
					velocityPerSecond.X -= 200.0;
					hitStunMovementDuration = 600;
					hitStunDuration = 9999;
					positionWhenHitByImpactAttack = getPosition();
					// if the player is inside the ringout zone, make the impossible possible :)
					if (getInStageRingoutZoneFlag()){
						canBeSentOutOfRing = true;
					}
				}
				if (!isGuarding()){
					setBasicAnimation(FK_BasicPose_Type::HitStrongFlightAnimation, true);
				}
				break;
			case FK_Reaction_Type::ReverseStrongFlight:
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				maxParallelStunVelocity = 10;
				parallelStunVelocity = 2 * maxParallelStunVelocity * (f32)rand() / RAND_MAX - maxParallelStunVelocity;
				setVelocityPerSecond(core::vector3df(75, parallelStunVelocity, 0));
				if (isGuarding()) {
					velocityPerSecond.X += 120.0;
				} else {
					velocityPerSecond.Z = 160.0;
					velocityPerSecond.X += 200.0;
					hitStunMovementDuration = 600;
					hitStunDuration = 9999;
				}
				if (!isGuarding()){
					setBasicAnimation(FK_BasicPose_Type::HitStrongFlightAnimation, true);
				}
				break;
			case FK_Reaction_Type::WeakFlight:
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity / 2, 0));
				velocityPerSecond.X -= 60.0;
				if (!isGuarding()){
					velocityPerSecond.Z = 80.0;
					velocityPerSecond.X -= 20.0;
					hitStunMovementDuration = 600;
					hitStunDuration = 9999;
					setBasicAnimation(FK_BasicPose_Type::HitStrongFlightAnimation, true);
				}
				break;
			case FK_Reaction_Type::StandingFlight:
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity / 2, 0));
				if (isGuarding()) {
					velocityPerSecond.X -= 135.0;
				}
				else {
					velocityPerSecond.Z = 160.0;
					velocityPerSecond.X = -50.0;
					hitStunMovementDuration = 600;
					hitStunDuration = 9999;
					positionWhenHitByImpactAttack = getPosition();
				}
				if (!isGuarding()) {
					setBasicAnimation(FK_BasicPose_Type::HitStrongFlightAnimation, true);
				}
				break;
			default:
				hitStunDuration = baseHitStunDuration;
				hitStunMovementDuration = 600;
				if (!isGuarding()){
					setVelocityPerSecond(core::vector3df(-75, parallelStunVelocity, 0));
					velocityPerSecond.Z = 160.0;
					velocityPerSecond.X -= 180.0;
				}
				break;

			}
			/* lock direction */
			if (!isGuarding() && reactionType != FK_Reaction_Type::StrongFlight &&
				reactionType != FK_Reaction_Type::ReverseStrongFlight && 
				reactionType != FK_Reaction_Type::WeakFlight){
				setDirectionLock(true);
			}
		}
		else if (isHitStun() && velocityPerSecond.Z != 0){
			// if the player is inside the ringout zone, make the impossible possible :)
			if (getInStageRingoutZoneFlag() && reactionType == FK_Reaction_Type::StrongFlight){
				canBeSentOutOfRing = true;
			}
			if (getVelocityPerSecond().X > 0 && reactionType == FK_Reaction_Type::StrongFlight){
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				setVelocityPerSecond(core::vector3df(-175, parallelStunVelocity / 2, getVelocityPerSecond().Z + 160.0));
			}
			else if (getVelocityPerSecond().X < 0 && reactionType == FK_Reaction_Type::ReverseStrongFlight){
				hitStunMovementDuration = databaseAccessor.getStrongHitstunMovementDurationMs();
				hitStunDuration = 600;
				setVelocityPerSecond(core::vector3df(175, parallelStunVelocity / 2, getVelocityPerSecond().Z + 160.0));
			}
			else if(!isKO()){
				setVelocityPerSecond(core::vector3df(
					-65.f - 15.f * (f32)bouncingCounter, 
					parallelStunVelocity * (f32)(bouncingCounter + 1), 
					160.f - 30.f * (f32)bouncingCounter));
				if (reactionType == FK_Reaction_Type::StrongFlight){
					velocityPerSecond.X -= 200.0;
				}
				else if ((reactionType & FK_Reaction_Type::AnyStrongReaction) != 0){
					velocityPerSecond.X -= 40.0;
				}
				hitStunMovementDuration = 600;
				hitStunDuration = 9999;
				bouncingCounter += 1;
				setBasicAnimation(FK_BasicPose_Type::HitStrongFlightAnimation);
			}
		}
		/* increase hitstun */
		hitStunDuration = (u32)(hitStunDuration * hitstunMultiplier);
		if (hitStunDuration > maximumHitStunDuration){
			hitStunDuration = maximumHitStunDuration;
		}
		/* reduce the hitstun time by 2 if the character is guarding */
		if (isGuarding()){
			u32 maxGuardHitstun = databaseAccessor.getGuardHitstunDurationMs();
			hitStunMovementDuration = databaseAccessor.getGuardHitstunMovementDurationMs();
			hitStunDuration = maxGuardHitstun;
			if (getStance() == FK_Stance_Type::CrouchedStance){
				setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation);
			}
			else{
				setBasicAnimation(FK_BasicPose_Type::GuardingAnimation);
			}
		}
		else if (isHitStun() && velocityPerSecond.Z > 0){
			hitStunDuration = 9999;
			delayAfterMoveTimeMs = 0;
		}
		/* halve hitstun if the player was hit while "waking up" - but leave it in hitstun if it's flying*/
		//if (wasWakingUp && hitStunDuration > baseHitStunDuration){
		//	hitStunDuration = baseHitStunDuration;
		//}
	};

	// update hit reaction
	void FK_Character::updateReaction(f32 delta_t_s){
		/* update time in millisecond from last frame*/
		hitStunReferenceTime += (u32)(delta_t_s * 1000.0f);
		/* if hitstun has ended, end update*/
		if (hitStunReferenceTime >= hitStunMovementDuration && velocityPerSecond.X != 0 && abs(velocityPerSecond.Z) < 2.0f){
			//setDirectionLock(false);
			setVelocityPerSecond(core::vector3df(0, 0, 0));
		}
		if (velocityPerSecond.Z == 0){
			if (canCancelHitstun(false)){
				velocityPerSecond.X = 0;
				velocityPerSecond.Y = 0;
				cancelHitstun();
			}
		}
	};

	bool FK_Character::canCancelHitstun(bool verticalVelocity){
		if (isBeingThrown()){
			return false;
		}
		if (verticalVelocity){
			return (hitStunFlag && hitStunReferenceTime > fk_constants::FK_MinimumFlightTime);
		};
		return (hitStunFlag && (hitStunReferenceTime >= hitStunDuration));
	}

	void FK_Character::cancelHitstun(){
		hitStunFlag = false;
		hitStunDuration = 0;
		if (!isTriggerGuardActive() && triggerComboedFlag){
			cancelTriggerComboedFlag();
		}
		bouncingCounter = 0;
		velocityPerSecond.X = 0;
		velocityPerSecond.Y = 0;
	}
	// guard
	bool FK_Character::isGuarding(){
		return guardFlag;
	}
	void FK_Character::breakGuard(){
		guardFlag = false;
	}
	// manage basic poses for the character
	void FK_Character::setBasicAnimation(FK_BasicPose_Type poseType, 
		bool force_replacement, 
		bool invert_animation, 
		bool remove_direction_lock){
		// reset frame rate
		animatedMesh->setAnimationSpeed(animationFrameRate);
		FK_Pose temp = basicAnimations[poseType];
		core::stringw newAnimationName = core::stringw(temp.getAnimationName().c_str());
		std::string newAnimationNameString = temp.getAnimationName();
		core::stringw tempCurrentAnimationName = getCurrentAnimationName();
		bool newAnimationFlag = newAnimationName != tempCurrentAnimationName;
		if (newAnimationFlag && remove_direction_lock) {
			setDirectionLock(false);
			setLockedDirection(core::vector3df(0, 0, 0));
		}
		/* call new animation from database */
		if (newAnimationFlag){
			bool loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), animationDirectory + newAnimationNameString + ".x");
			//bool loaded = false;
			if (!loaded){
				loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), sharedAnimationDirectory + newAnimationNameString + ".x");
			}
			if (loaded){
				setAnimation(newAnimationNameString.c_str());
			}
			else{
				currentAnimationName = newAnimationName;
			}
		}
		/* invert animation if needed */
		if (invert_animation){
			animatedMesh->setAnimationSpeed(-animationFrameRate);
		}
		else{
			animatedMesh->setAnimationSpeed(animationFrameRate);
		}
		/*for (int k = 0; k < characterAdditionalObjects.size(); ++k){
			if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
				characterAdditionalObjectsMeshs[k]->setAnimationSpeed(animatedMesh->getAnimationSpeed());
			}
		}*/
		if (animatedMesh->getStartFrame() != temp.getStartingFrame() ||
			animatedMesh->getEndFrame() != temp.getEndingFrame() ||
			force_replacement){
			animatedMesh->setFrameLoop(0, 0);
			animatedMesh->setFrameLoop(temp.getStartingFrame(), temp.getEndingFrame());
			for (int k = 0; k < characterAdditionalObjects.size(); ++k){
				if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
					characterAdditionalObjectsMeshs[k]->setFrameLoop(0, 0);
					characterAdditionalObjectsMeshs[k]->setFrameLoop(temp.getStartingFrame(), temp.getEndingFrame());
				}
			}
		}
		if (animatedMesh->getLoopMode() != temp.isLooping()){
			animatedMesh->setLoopMode(temp.isLooping());
			for (int k = 0; k < characterAdditionalObjects.size(); ++k){
				if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
					characterAdditionalObjectsMeshs[k]->setLoopMode(temp.isLooping());
				}
			}
		}
		//synchronizeAnimatedObjects();
	};
	/* animation end*/
	bool FK_Character::isAnimationEnded(){
		return (!animatedMesh->getLoopMode() &&
			animatedMesh->getFrameNr() >= animatedMesh->getEndFrame());
	};

	/* replace basic animation */
	void FK_Character::replaceBasicAnimation(FK_BasicPose_Type pose, std::string animationName,
		s32 startFrame, s32 endFrame, bool loop)
	{
		FK_Pose temp(animationName, startFrame, endFrame, loop);
		basicAnimations[FK_BasicPose_Type::IdleAnimation] = temp;
	}

	/* check if can be sent out of ring (send boolean)*/
	bool FK_Character::canSufferRingOut(){
		if (!databaseAccessor.ringoutAllowed()) {
			return false;
		}
		bool positionFlag = positionWhenHitByImpactAttack.getDistanceFrom(getPosition()) <= 
			databaseAccessor.getRingoutDistanceFromWall();
		return positionFlag && canBeSentOutOfRing && 
			(velocityPerSecond.X < 0 || currentMove == NULL && velocityPerSecond.X > 0);
	}
	/* reset can be sent ring out flag */
	void FK_Character::resetCanSufferRingoutFlag(){
		canBeSentOutOfRing = false;
		positionWhenHitByImpactAttack = core::vector3df(0, 0, 0);
	}
	/* set ring-out */
	void FK_Character::setRingoutFlag(bool new_ringoutFlag){
		isRingOut = new_ringoutFlag;
	}
	/* get ring out status*/
	bool FK_Character::getRingoutFlag(){
		return isRingOut;
	}
	/* set if the player is in the "zone in which a player can be sent ringout with an attack" */
	void FK_Character::setInStageRingoutZoneFlag(bool new_ringoutFlag){
		isInStageRingoutZone = new_ringoutFlag;
		if (!new_ringoutFlag){
			canBeSentOutOfRing = false;
		}
	}
	/* get if the player is in the "zone in which a player can be sent ringout with an attack" */
	bool FK_Character::getInStageRingoutZoneFlag(){
		return isInStageRingoutZone;
	}
	/* main update routine */
	void FK_Character::update(u32 lastButtonPressed, f32 delta_t_s, bool& isNewMovePerformed){
		//std::cout << introQuotes["generic"].size() << std::endl;
		positionAtPreviousFrame = getPosition();
		updateParticleEffect(delta_t_s);
		updateMoveDelayCounter(delta_t_s);
		updateTriggerGuard(delta_t_s);
		updateBullets(delta_t_s);
		u32 delta_t_ms = (u32)(floor(1000 * delta_t_s));
		updateRunning(delta_t_ms);
		updateJump(delta_t_ms);
		updateImpactCancelCheck(delta_t_ms);
		updateSoundTimer(delta_t_ms);
		updatePushback(delta_t_ms);
		isNewMovePerformed = false;
		
		// update armor special effect
		if (hasActiveArmorSpecialEffect()) {
			specialEffectArmorTimeCounterMS += delta_t_ms;
			if (currentMove == NULL && specialEffectArmorTimeCounterMS > FK_Character::ArmorSpecialEffectDurationMS) {
				disableArmorSpecialEffect();
			}
		}

		// update Trigger cancel special effect
		if (hasActiveTriggerCancelSpecialEffect()) {
			impactCancelShaderEffectTimer -= (s32)delta_t_ms;
		}

		// check if the player is triggering
		if ((lastButtonPressed & fk_constants::FK_TriggerButton) == fk_constants::FK_TriggerButton){
			triggerFlag = true;
		}
		else{
			// else, disable trigger flag
			triggerFlag = false;
		}
		// check throw movement
		if (isBeingThrown()){
			float currentFrame = animatedMesh->getFrameNr();
			if (lastFrame != currentFrame){
				if (targetThrowToApply.getMoveId() > 0){
					if (lastFrame == -1) lastFrame = currentFrame;
					nextMovement = targetThrowToApply.getThrowTargetMovementAtFrame(
						(int)floor(currentFrame))*(currentFrame - lastFrame);
				}
				lastFrame = currentFrame;
			}
		}
		// create bullets if needed
		if (currentMove != NULL){
			if (!currentMove->getBulletCollection().empty()){
				auto collection = currentMove->getBulletCollection();
				u32 moveId = getCurrentMove()->getMoveId();
				//for (auto it = currentMove->getBulletCollection().begin(); it < currentMove->getBulletCollection().end(); ++it){
				for each(FK_Bullet temp_bullet in currentMove->getBulletCollection()){
					//FK_Bullet temp_bullet = *(it);
					if (temp_bullet.getHitbox().getStartingFrame() <= getCurrentFrame() && 
						temp_bullet.getHitbox().getEndingFrame() >= getCurrentFrame() &&
						floor(lastFrame) != getCurrentFrame()){
						scene::ISceneNode* parent = hitboxCollection[temp_bullet.getHitbox().getType()];// ->getParent();
						parent->getParent()->updateAbsolutePosition();
						FK_Bullet* new_bullet = new FK_Bullet();
						new_bullet->setup(&temp_bullet);
						new_bullet->setPosition(parent->getAbsolutePosition());
						new_bullet->applyRotation(getRotation());
						new_bullet->setMeshPath(getCharacterDirectory() + temp_bullet.getMeshPath());
						new_bullet->setOriginalMoveId(moveId);
						if (temp_bullet.getParticleEmitterTextureName() != std::string()){
							new_bullet->setParticleEmitter(getCharacterDirectory() + temp_bullet.getParticleEmitterTextureName());
						}
						new_bullet->setupMesh(smgr_reference);
						new_bullet->update();
						bulletsCollection.push_back(new_bullet);
					}
				}
			}
		}
		if (!canAcceptInput() && canUpdateGuardTransition()) {
			updateReaction(delta_t_s);
			updateGuardTransition(lastButtonPressed);
		}
		// update reactions
		else if (isHitStun()){
			if (triggerButtonPressedBeforeHitstun) {
				if (!(lastButtonPressed & fk_constants::FK_TriggerButton)) {
					triggerButtonPressedBeforeHitstun = false;
				}
			}
			updateReaction(delta_t_s);
			/* now, if the character is Hitstun, but the trigger and guard button are pressed together, start
			revenge trigger! */
			if (canRevengeTrigger()){
				u32 temp = fk_constants::FK_GuardButton;
				u32 tempG = lastButtonPressed & fk_constants::FK_GuardButton;
				u32 tempT = lastButtonPressed & fk_constants::FK_TriggerButton;
				if (tempG == fk_constants::FK_GuardButton && tempT == fk_constants::FK_TriggerButton){
					u32 requiredTriggerCounters = databaseAccessor.getTriggerCountersPerActivation();
					if ((u32)getTriggerCounters() >= requiredTriggerCounters){
						activateTriggerMode();
						cancelHitstun();
						guardFlag = true;
						if (getStance() == FK_Stance_Type::GroundStance){
							setBasicAnimation(FK_BasicPose_Type::GuardingAnimation);
						}
						else if (getStance() == FK_Stance_Type::CrouchedStance){
							setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation);
						}
						setTrackingFlag(true);
						stats.numberOfTriggerGuards += 1;
						triggerGuardDurationS = 0;
						// add hitstun to TG
						hitStunFlag = true;
						u32 frameAdvantageMs = 
							databaseAccessor.getTriggerGuardFrameAdvantage() * 1000 / 60; // defender TG frame advantage
						hitStunDuration = databaseAccessor.getStrongHitstunDurationMs();
						if (hitStunDuration > frameAdvantageMs) {
							hitStunDuration -= frameAdvantageMs;
						}
						
					}
				}
			}
		}
		else if (canAcceptInput()){
			if (canCheckForImpactCancelInput){
				bool wasNotCanceling = isInImpactCancelState();
				checkForImpactCancel(lastButtonPressed);
				if (!wasNotCanceling && isInImpactCancelState()){
					return;
				}
			}
			if (currentMove != NULL){
				float currentFrame = animatedMesh->getFrameNr();
				if (lastFrame != currentFrame){
					if (lastFrame == -1) lastFrame = currentFrame;
					nextMovement = currentMove->getMovementAtFrame((int)floor(currentFrame))*(currentFrame - lastFrame);
					// remove vertical bouncing if player touches ground
					if (hasTouchedGround){
						nextMovement.Z = 0;
					}
					lastFrame = currentFrame;
				}

				if (currentFrame >= currentMove->getEndingFrame() ||
					(plannedMove.isValid() && currentFrame >= 
						plannedMove.activationFrame) ){
					std::string autoFollowupMoveName = currentMove->getAutoFollowupMove();
					//std::cout << "Move end: frame " << currentFrame << std::endl;
					// check if trigger combo has to be maintained
					bool cancelTrigger = true;
					if (isTriggerModeActive()){
						bool abortTrigger = true;
						int size = currentMove->getHitboxCollection().size();
						if (size == 0 &&
							(getNextMove() != NULL || plannedMove.isValid())){
							abortTrigger = false;
							cancelTrigger = false;
						}
						else{
							if (currentMove->hasConnectedWithTarget() && 
								(getNextMove() != NULL || plannedMove.isValid()) ) {
								abortTrigger = false;
							}
							bool validHitbox = true;
							if (plannedMove.isValid() && currentFrame >=
								plannedMove.activationFrame) {
								validHitbox = false;
								for (int i = 0; i < size; i++) {
									if (currentMove->getHitboxCollection()[i].getStartingFrame() < currentFrame) {
										validHitbox = true;
									}
								}
							}
							if (validHitbox) {
								for (int i = 0; i < size; i++) {
									if (currentMove->getHitboxCollection()[i].hasHit()) {
										abortTrigger = false;
										if ((currentMove->getHitboxCollection()[i].getAttackType() & FK_Attack_Type::ThrowAtk) != 0) {
											cancelTrigger = false;
										}
										break;
									}
								}
							}
							else {
								abortTrigger = false;
								cancelTrigger = false;
							}
						}
						if (abortTrigger){
							cancelTriggerMode();
						}
					}
					if (plannedMove.isValid()) {
						isNewMovePerformed = performMove(plannedMove.move, true);
						plannedMove.reset();
					}
					else if (getNextMove() != NULL){
						isNewMovePerformed = performMove(getNextMove(), true);
					}
					// perform auto followup
					else if(!autoFollowupMoveName.empty()){						
						for (auto it = getMovesCollection().begin(); it != getMovesCollection().end(); ++it)
						{
							FK_Move* move = &(*it);
							if (move->getName() == autoFollowupMoveName) {
								isNewMovePerformed = performMove(move, true);
								if (!isNewMovePerformed) {
									resetMove(cancelTrigger);
								}
								break;
							}
						}
					}
					else{
						// temporary
						bool lockAirDirection = false;
						core::vector3df directionToLock = getLockedDirection();
						if (currentMove != NULL && currentMove->getEndStance() == FK_Stance_Type::AirStance) {
							velocityPerSecond.X = currentMove->getAdditionalFallVelocity();
							lockAirDirection = true;
						}
						/* set flag for move cancels */
						lastMoveNoHitboxFlag = false;
						if (!currentMove->canBeTriggered() && currentMove->getTotalBulletDamage() <= 0 &&
							currentMove->getEndStance() != FK_Stance_Type::RunningStance) {
							lastMoveNoHitboxFlag = true;
						}
						resetMove(cancelTrigger);
						if (lockAirDirection) {
							setDirectionLock(true);
							setLockedDirection(directionToLock);
						}
					}
				}
			}
			else{
				// add running buffer for moves (TEST)
				if (isRunning() &&
					delayAfterMoveTimeCounterMs >= delayAfterMoveTimeMs &&
					currentMove == NULL &&
					plannedMove.isValid() &&
					plannedMove.move->getStance() == FK_Stance_Type::RunningStance &&
					plannedMove.move->getEndStance() != FK_Stance_Type::RunningStance) {
					isNewMovePerformed = performMove(plannedMove.move, true);
					plannedMove.reset();
				}
				else {
					basicPoseUpdate(lastButtonPressed, delta_t_s);
				}
			}
		}
		/* update movement */
		nextMovement.X += velocityPerSecond.X * delta_t_s;
		nextMovement.Y += velocityPerSecond.Y * delta_t_s;
		nextMovement.Z += velocityPerSecond.Z * delta_t_s;
		if (hasPushback()) {
			nextMovement.X += pushbackVelocity.X * delta_t_s;
			nextMovement.Y += pushbackVelocity.Y * delta_t_s;
			nextMovement.Z += pushbackVelocity.Z * delta_t_s;
		}
		/* animate joints for mesh*/
		animatedMesh->animateJoints();
		/* update additional (i.e. non visible) hurtboxes */
		if (!hurtboxCollection.empty()){
			hurtboxCollection[FK_Hurtbox_Type::FullBodyHurtbox]->updateAbsolutePosition();
		}
		/* update additional objects */
		updateAdditionalObjects(delta_t_s, false);
		/* update texture switches*/
		checkTextureSwitches();
		/* check for transformations */
		checkForTransformation(delta_t_ms);
	}

	// update triggering
	void FK_Character::updateTriggeringFlag(u32 allButtonsPressed)
	{
		// check if the player is triggering
		if ((allButtonsPressed & fk_constants::FK_TriggerButton) == fk_constants::FK_TriggerButton) {
			triggerFlag = true;
		}
		else {
			// else, disable trigger flag
			triggerFlag = false;
		}
	}
	;

	// Jump section
	bool FK_Character::canJump(){
		if (velocityPerSecond.Z != 0 ||
			isHitStun()){
			return false;
		}
		if (!canMove()){
			return false;
		}
		return true;
	}
	float FK_Character::getGravity(){
		if (isJumping()){
			return testGravity + jumpGravity;
		}
		else{
			return testGravity;
		}
	}
	void FK_Character::performJump(FK_JumpDirection jumpDirection, f32 additionalAxialvelocity){
		isSidestepping = false;
		sidestepCounter = 0;
		guardFlag = false;
		//velocityPerSecond.Z = jumpSpeed;
		velocityPerSecond.Z = 400.0f;
		velocityPerSecond.X = (100.0f + additionalAxialvelocity) * (s32)jumpDirection;
		jumpingFlag = true;
		if (jumpDirection == FK_JumpDirection::JumpUp) {
			setBasicAnimation(FK_BasicPose_Type::JumpingAnimation);
			storedJumpingAnimation = FK_BasicPose_Type::JumpingAnimation;
		}
		else if (jumpDirection == FK_JumpDirection::JumpForward) {
			setBasicAnimation(FK_BasicPose_Type::ForwardJumpAnimation);
			storedJumpingAnimation = FK_BasicPose_Type::ForwardJumpAnimation;
		}
		else if (jumpDirection == FK_JumpDirection::JumpBackward) {
			setBasicAnimation(FK_BasicPose_Type::BackwardJumpAnimation);
			storedJumpingAnimation = FK_BasicPose_Type::BackwardJumpAnimation;
		}
		setStance(FK_Stance_Type::AirStance);
		setDirectionLock(true);
		setLockedDirection(core::vector3df(0, 0, 0));
		delayAfterMoveTimeMs = 130;
		delayMovementAfterMoveTimeMs = 100;
		delayAfterMoveTimeCounterMs = 0;
		jumpCounterMs = 0;
		jumpAttackThresholdMs = 700;
		if (getInStageRingoutZoneFlag()) {
			positionWhenHitByImpactAttack = getPosition();
			canBeSentOutOfRing = true;
		}
	}

	bool FK_Character::canPerformJumpAttack(){
		return jumpCounterMs < jumpAttackThresholdMs;
	}

	bool FK_Character::isJumping(){
		bool basicJumpFlag = jumpingFlag || currentStance == FK_Stance_Type::AirStance;
		bool advancedJumpFlag = currentMove != NULL && currentMove->hasLowerGravity();
		return (basicJumpFlag && !advancedJumpFlag);
	};

	bool FK_Character::isAirborne(){
		bool airMoveFlag = false;
		if (currentMove != NULL){
			airMoveFlag = currentMove->getStance() == FK_Stance_Type::AirStance ||
				currentMove->getEndStance() == FK_Stance_Type::AirStance;
		}
		return isJumping() || airMoveFlag;
	}

	void FK_Character::cancelJumpVariables() {
		jumpCounterMs = 0;
		jumpingFlag = false;
	}

	void FK_Character::cancelJump(bool leaveTriggerStateUnchanged){
		jumpCounterMs = 0;
		jumpingFlag = false;
		//velocityPerSecond.Z = 0;
		//std::cout << getDisplayName() << " canceling jump..." << std::endl;
		if (getLastStackedMove() == NULL){
			resetMoveDelayCounter();
			setStance(FK_Stance_Type::GroundStance);
			delayMovementAfterMoveTimeMs = 150;
		}
		else{
			if (getLastStackedMove()->getStance() == FK_Stance_Type::AirStance){
				setStance(FK_Stance_Type::GroundStance);
			}
		}
		if (isPerformingAirAttack){
			if (getNextMove() == NULL && 
				(currentMove != NULL && currentMove->getEndStance() == FK_Stance_Type::AirStance)){
				resetMove(!leaveTriggerStateUnchanged);
				setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
				setVelocityPerSecond(core::vector3df(0, 0, 0));
			}
			else if ((currentMove == NULL || (currentMove->getEndStance() == FK_Stance_Type::GroundStance ||
				currentMove->getEndStance() == FK_Stance_Type::CrouchedStance)) && getNextMove() == NULL){
				jumpCounterMs = 0;
				jumpingFlag = false;
				setVelocityPerSecond(core::vector3df(0, 0, 0));
			}
			else if (getNextMove() != NULL && 
				(getNextMove()->getStance() & FK_Stance_Type::AnyStandardStance) != 0 &&
				(currentMove == NULL || (currentMove->getEndStance() & FK_Stance_Type::AnyStandardStance) == 0)
				){
				jumpCounterMs = 0;
				jumpingFlag = false;
				setStance(FK_Stance_Type::GroundStance);
				setVelocityPerSecond(core::vector3df(0, 0, 0));
				if (!leaveTriggerStateUnchanged) {
					if (isTriggerModeActive() && (
						(currentMove != NULL && !currentMove->hasConnectedWithTarget() && currentMove->canBeTriggered()) ||
						(storageMove != NULL && !storageMove->hasConnectedWithTarget() && storageMove->canBeTriggered()))) {
						cancelTriggerMode();
					}
				}
				currentMove = NULL;
				storageMove = NULL;
				performMove(getNextMove(),true);
			}
		}
	};
	// check if jump can be canceled (still to be implemented)
	bool FK_Character::canCancelJump(){
		return true;
	}
	// check if the character can guard
	bool FK_Character::canPerformGuard(){
		bool stanceFlag = (currentStance & FK_Stance_Type::AnyStandardStance) > 0;
		stanceFlag &= (!isJumping());
		//stanceFlag |= currentStance == FK_Stance_Type::WakeUpStance;
		return canMove() && (isGuarding() || !isHitStun()) && stanceFlag;
	}

	// check if character can evade bullets
	bool FK_Character::canEvadeBullets() {
		return getNextMovement().Y != 0 && !isMovingAgainstWalls;
	}

	// check if player is touching walls
	bool FK_Character::isTouchingWall()
	{
		return isMovingAgainstWalls;
	}

	// check if the palyer is cornered
	bool FK_Character::isCornered()
	{
		return corneredFlag;
	}

	// set cornered flag
	void FK_Character::setCorneredFlag(bool newFlag) {
		corneredFlag = newFlag;
	}

	// set wall collision flag
	void FK_Character::setRingWallCollisionFlag(bool newFlag) {
		isMovingAgainstWalls = newFlag;
	}

	// check if the character is running
	bool FK_Character::isRunning(){
		return currentStance == FK_Stance_Type::RunningStance;
	}

	// check if run is being canceled
	bool FK_Character::isCancelingRun(){
		return runningBufferTimerMsTarget > 0;
	}

	// update running process
	void FK_Character::updateRunning(u32 frameDelta_Ms){
		if (isRunning() && isCancelingRun()){
			runningBufferTimerMs += frameDelta_Ms;
			if (runningBufferTimerMs >= runningBufferTimerMsTarget){
				cancelRunning();
			}
		}
	}

	// stop run - if a time is given, the run is aborted after a certain amount of time
	void FK_Character::cancelRunning(u32 timeMs){
		if (isCancelingRun() && timeMs > 0){
			return;
		}
		if (timeMs == 0){
			if (impactCancelRun_flag){
				hasUsedImpactCancelInCurrentCombo = false;
			}
			impactCancelRun_flag = false;
			runningBufferTimerMs = -1;
			runningBufferTimerMsTarget = -1;
			setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
			setStance(FK_Stance_Type::GroundStance);
		}
		else{
			runningBufferTimerMs = 0;
			runningBufferTimerMsTarget = timeMs;
		}
	}

	// update basic pose
	void FK_Character::basicPoseUpdate(u32 lastButtonPressed, f32 delta_t_s){
		// check if character is landing
		if (getStance() == FK_Stance_Type::LandingStance && !isHitStun()){
			if (lastButtonPressed & fk_constants::FK_GuardButton) {
				u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
				// back dash wake up
				if ((directionButtonPressed & FK_Input_Buttons::Left_Direction) != 0) {
					velocityPerSecond.X = -140;
				}
				// front dash wake up
				else if ((directionButtonPressed & FK_Input_Buttons::Right_Direction) != 0) {
					velocityPerSecond.X = 100;
				}
				recoverFromGrounding();
			}
			return;
		}
		//if (getStance() == FK_Stance_Type::AirStance){
		//	
		//}
		// check if character is KO
		if (isGrounded() && isKO()){
			setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation);
			setDirectionLock(true);
			setLockedDirection(core::vector3df(0, 0, 0));
			rollingTimeCounterMs = 0;
			return;
		}
		// check guard
		u32 actionButtonsPressed = lastButtonPressed & FK_Input_Buttons::Any_NonDirectionButton;
		bool onlyGuardButtonPressed = (actionButtonsPressed & fk_constants::FK_GuardButton) == fk_constants::FK_GuardButton;
		bool guardButtonPressed = (actionButtonsPressed & fk_constants::FK_GuardButton) != 0;
		if (guardButtonPressed){
			if (canPerformGuard()){
				if (!isGuarding() && !onlyGuardButtonPressed){
					guardFlag = false;
				}
				else{
					if (getStance() == FK_Stance_Type::GroundStance){
						if (!isGuarding()) {
							guardFlag = true;
							setBasicAnimation(FK_BasicPose_Type::GuardingAnimation, true);
							setTrackingFlag(true);
						}
					}
					else if (getStance() == FK_Stance_Type::CrouchedStance){
						if (!guardFlag){
							guardFlag = true;
							setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation, true);
							setTrackingFlag(true);
						}
						u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
						if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) == 0){
							setBasicAnimation(FK_BasicPose_Type::GuardingAnimation, true);
							setStance(FK_Stance_Type::GroundStance);
						}
					}
					else if (getStance() == FK_Stance_Type::WakeUpStance){
						if (!guardFlag){
							//setStance(FK_Stance_Type::GroundStance);
							setVelocityPerSecond(core::vector3df(0, 0, 0));
							guardFlag = true;
						}
					}
				}
			}
			else if (isGrounded()){
				if (canMove()){
					recoverFromGrounding();
					////animatedMesh->setTransitionTime(0.2f);
					//u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
					//if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) != 0){
					//	guardFlag = true;
					//	// low guard wake up
					//	setVelocityPerSecond(core::vector3df(0, 0, 0));
					//	setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation);
					//	setStance(FK_Stance_Type::CrouchedStance);
					//}
					//else{
					//	recoverFromGrounding();
					//}
				}
			}
		}
		else{
			guardFlag = false;
			/*
			if (getStance() == FK_Stance_Type::CrouchedStance){
				setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
				setStance(FK_Stance_Type::GroundStance);
			}
			*/
		}
		u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
		u32 tempButton = directionButtonPressed;
		float walkingStep = walkingSpeed * delta_t_s;
		float runningStep = runningSpeed * delta_t_s;
		float sidestepStep = sidestepSpeed * delta_t_s;
		float mixedstepStep = mixedstepSpeed * delta_t_s;
		if (impactCancelRun_flag){
			runningStep *= databaseAccessor.getRunCancelSpeedMultiplier();
		}
		// invert input if on the other side of the screen
		if (!isOnLeftSide()){
			if (tempButton & FK_Input_Buttons::Left_Direction){
				directionButtonPressed = tempButton - FK_Input_Buttons::Left_Direction +
					FK_Input_Buttons::Right_Direction;
			}
			else if (tempButton & FK_Input_Buttons::Right_Direction){
				directionButtonPressed = tempButton - FK_Input_Buttons::Right_Direction +
					FK_Input_Buttons::Left_Direction;
			}
		}
		/*
		if (!canMove()){
			directionButtonPressed = 0;
			//setStance(FK_Stance_Type::GroundStance);
		}
		*/
		if (isJumping()){
			setBasicAnimation(storedJumpingAnimation, false, false, false);
		}
		else if (isGuarding()){
			if (getStance() != FK_Stance_Type::WakeUpStance) {
				bool directionalInputWithGuard = (directionButtonPressed & FK_Input_Buttons::Left_Direction) != 0 ||
					(directionButtonPressed & FK_Input_Buttons::Right_Direction) != 0;
				if (isSidestepping && !(lastButtonPressed & FK_Input_Buttons::Trigger_Button)) {
					if (sidestepCounter < 100  || directionalInputWithGuard) {
						isSidestepping = false;
						sidestepCounter = 0;
					}
					else {
						if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) == 0 &&
							(directionButtonPressed & FK_Input_Buttons::Up_Direction) == 0) {
							isSidestepping = false;
							sidestepCounter = 0;
						}
					}
				}
				else {
					if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) != 0) {
						//setBasicAnimation(FK_BasicPose_Type::CrouchingIdleAnimation);
						isSidestepping = false;
						sidestepCounter = 0;
						setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation);
						setStance(FK_Stance_Type::CrouchedStance);
					}
					else
						if ((directionButtonPressed & FK_Input_Buttons::Up_Direction) != 0) {
							if (canJump()) {
								isSidestepping = false;
								sidestepCounter = 0;
								//setBasicAnimation(FK_BasicPose_Type::CrouchingIdleAnimation);
								FK_JumpDirection jumpDirection = FK_JumpDirection::JumpUp;
								if ((directionButtonPressed & FK_Input_Buttons::Right_Direction) != 0) {
									jumpDirection = FK_JumpDirection::JumpForward;
								}
								else if ((directionButtonPressed & FK_Input_Buttons::Left_Direction) != 0) {
									jumpDirection = FK_JumpDirection::JumpBackward;
								}
								performJump(jumpDirection);
							}
						}
				}
			}
		}
		else{
			if (getStance() == FK_Stance_Type::CrouchedStance){
				isSidestepping = false;
				sidestepCounter = 0;
				if (canMove() && (directionButtonPressed & FK_Input_Buttons::Down_Direction) == 0){
					setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
					setStance(FK_Stance_Type::GroundStance);
				}
				else{
					setBasicAnimation(FK_BasicPose_Type::CrouchingIdleAnimation);
				}
				
			}
			// update running;
			else if (isRunning()){
				isSidestepping = false;
				sidestepCounter = 0;
				if ((directionButtonPressed & FK_Input_Buttons::Right_Direction) == FK_Input_Buttons::Right_Direction){
						setBasicAnimation(FK_BasicPose_Type::RunningAnimation);
						setStance(FK_Stance_Type::RunningStance);
						nextMovement.X = runningStep;
						nextMovement.Y = 0;
						if (!isImpactRunning() && (directionButtonPressed & FK_Input_Buttons::Up_Direction) != 0 &&
							(lastButtonPressed & FK_Input_Buttons::Guard_Button) != 0) {
							if (canJump() && databaseAccessor.runJumpCancelAllowed()) {
								cancelRunning();
								performJump(FK_JumpDirection::JumpForward, 100.f);
							}
						}
				}
				else{
					// cancel running if the direction button is not pressed
					cancelRunning();
				}
			}
			else if ((currentStance & FK_Stance_Type::AnySpecialStance) != 0){
				isSidestepping = false;
				sidestepCounter = 0;
				switch(currentStance){
				case FK_Stance_Type::SpecialStance1:
					setBasicAnimation(FK_BasicPose_Type::SpecialStance1Animation);
					break;
				case FK_Stance_Type::SpecialStance2:
					setBasicAnimation(FK_BasicPose_Type::SpecialStance2Animation);
					break;
				case FK_Stance_Type::SpecialStance3:
					setBasicAnimation(FK_BasicPose_Type::SpecialStance3Animation);
					break;
				case FK_Stance_Type::SpecialStance4:
					setBasicAnimation(FK_BasicPose_Type::SpecialStance4Animation);
					break;
				case FK_Stance_Type::SpecialStance5:
					setBasicAnimation(FK_BasicPose_Type::SpecialStance5Animation);
					break;
				default:
					setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
					break;
				};
			}
			// update idle stance, sidestep and walking
			else if (getStance() == FK_Stance_Type::GroundStance){
				if (directionButtonPressed == 0 || !canMove()){
					isSidestepping = false;
					sidestepCounter = 0;
					setBasicAnimation(FK_BasicPose_Type::IdleAnimation);
					setStance(FK_Stance_Type::GroundStance);
				}
				// look for movement, starting from diagonal motion
				else if ((directionButtonPressed & FK_Input_Buttons::UpLeft_Direction) == FK_Input_Buttons::UpLeft_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::BackWalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = -walkingStep;
						nextMovement.Y = -mixedstepStep;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::UpRight_Direction) == FK_Input_Buttons::UpRight_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::WalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = walkingStep;
						nextMovement.Y = -mixedstepStep;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::DownRight_Direction) == FK_Input_Buttons::DownRight_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::WalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = walkingStep;
						nextMovement.Y = mixedstepStep;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::DownLeft_Direction) == FK_Input_Buttons::DownLeft_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::BackWalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = -walkingStep;
						nextMovement.Y = mixedstepStep;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::Right_Direction) == FK_Input_Buttons::Right_Direction){
					if (canMove()){
						isSidestepping = false;
						sidestepCounter = 0;
						setBasicAnimation(FK_BasicPose_Type::WalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = walkingStep;
						nextMovement.Y = 0;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::Left_Direction) == FK_Input_Buttons::Left_Direction){
					if (canMove()){
						isSidestepping = false;
						sidestepCounter = 0;
						setBasicAnimation(FK_BasicPose_Type::BackWalkingAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = -walkingStep;
						nextMovement.Y = 0;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::Up_Direction) == FK_Input_Buttons::Up_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::SidestepAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = 0;
						nextMovement.Y = -sidestepStep;
					}
				}
				else if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) == FK_Input_Buttons::Down_Direction){
					if (canMove()){
						isSidestepping = true;
						sidestepCounter += std::floor(1000 * delta_t_s);
						setBasicAnimation(FK_BasicPose_Type::SidestepAnimation);
						setStance(FK_Stance_Type::GroundStance);
						nextMovement.X = 0;
						nextMovement.Y = sidestepStep;
					}
				}
				else {
					isSidestepping = false;
					sidestepCounter = 0;
				}
			}
			else if (isGrounded()){
				isSidestepping = false;
				sidestepCounter = 0;
				if (canMove()){
					if (directionButtonPressed == 0 ||
						(directionButtonPressed & FK_Input_Buttons::Left_Direction) == FK_Input_Buttons::Left_Direction ||
						(directionButtonPressed & FK_Input_Buttons::Right_Direction) == FK_Input_Buttons::Right_Direction ||
						rollingTimeCounterMs >= maximumRollingTimeMs){
						if (directionButtonPressed != 0 || hasRotatedOnTheGround || hitWhileGrounded){
							// back dash wake up
							if ((directionButtonPressed & FK_Input_Buttons::Left_Direction) != 0){
								velocityPerSecond.X = -140;
							}
							// front dash wake up
							else if ((directionButtonPressed & FK_Input_Buttons::Right_Direction) != 0){
								velocityPerSecond.X = 100;
							}
							recoverFromGrounding();
						}
						else{
							setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation);
							setDirectionLock(true);
							setLockedDirection(core::vector3df(0, 0, 0));
							rollingTimeCounterMs = 0;
						}
					}
					else if ((directionButtonPressed & FK_Input_Buttons::Up_Direction) != 0){
						if (hitWhileGrounded || groundRotationDirection == FK_GroundRotationDirection::RotationTowardsForeground){
							recoverFromGrounding();
						}
						if (canMove()){
							setBasicAnimation(FK_BasicPose_Type::GroundedSupineRotationAnimation, false, isOnLeftSide());
							nextMovement.X = 0;
							nextMovement.Y = -walkingStep * 2;
							hasRotatedOnTheGround = true;
							rollingTimeCounterMs += (u32)(1000 * delta_t_s);
							groundRotationDirection = FK_GroundRotationDirection::RotationTowardsBackground;
						}
						//setDirectionLock(true);
						//setLockedDirection(core::vector3df(0, 0, 0));
					}
					else if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) != 0){
						if (hitWhileGrounded || groundRotationDirection == FK_GroundRotationDirection::RotationTowardsBackground){
							recoverFromGrounding();
						}
						if (canMove()){
							setBasicAnimation(FK_BasicPose_Type::GroundedSupineRotationAnimation, false, !isOnLeftSide());
							nextMovement.X = 0;
							nextMovement.Y = walkingStep * 2;
							hasRotatedOnTheGround = true;
							rollingTimeCounterMs += (u32)(1000 * delta_t_s);
							groundRotationDirection = FK_GroundRotationDirection::RotationTowardsForeground;
						}
						//setDirectionLock(true);
						//setLockedDirection(core::vector3df(0, 0, 0));
					}
				}
			}
		}
	}

	// Physics section
	float FK_Character::getWalkingSpeed(){
		return walkingSpeed;
	};
	float FK_Character::getJumpSpeed(){
		return jumpSpeed;
	};
	float FK_Character::getWeight(){
		return weight;
	};
	// bones
	scene::IBoneSceneNode* FK_Character::getBone(FK_Bones_Type boneType){
		std::string boneName = armatureBonesCollection[boneType];
		return animatedMesh->getJointNode(boneName.c_str());
	}
	/* Animation manager - get animation name */
	core::stringw FK_Character::getCurrentAnimationName(){
		return currentAnimationName;
	}
	/* Animation manager - set new animation (if conditions meet)*/
	bool FK_Character::setAnimation(core::stringw animationName){
		/* if the animation does not exist, return false */
		if (animationKeymap.count(animationName) == 0){
			return false;
		}
		/* if the animation is already played, return*/
		if (getCurrentAnimationName() == animationName){
			return true;
		}
		/* if the animation is new, check if the animation was saved before */
		if (getCurrentAnimationName() != animationName){
			core::array<ISkinnedMesh::SJoint*> AllJoints = ((ISkinnedMesh*)animatedMesh->getMesh())->getAllJoints();
			for (u32 i = 0; i < AllJoints.size(); ++i)
			{
				ISkinnedMesh::SJoint *joint = AllJoints[i];
				joint->RotationKeys.clear();
				joint->ScaleKeys.clear();
				joint->PositionKeys.clear();
				core::stringc jointName = joint->Name;
				joint->RotationKeys = animationKeymap[animationName]->getRotationKeys(jointName);
				joint->ScaleKeys = animationKeymap[animationName]->getScaleKeys(jointName);
				joint->PositionKeys = animationKeymap[animationName]->getPositionKeys(jointName);
			}
			animatedMesh->setCurrentFrame(0);
			((ISkinnedMesh*)animatedMesh->getMesh())->setFrameBoundaries(animationKeymap[animationName]->getFrameCount());
			/* set new animation name */
			currentAnimationName = animationName;
			/*refreshCollisionEllipsoid = true && !wasCollisionEllipsoidRefreshed;
			wasCollisionEllipsoidRefreshed = true;*/
			// check if animation has to be updated for some objects
			for (int k = 0; k < characterAdditionalObjects.size(); ++k){
				if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
					core::array<ISkinnedMesh::SJoint*> AllJoints = ((ISkinnedMesh*)characterAdditionalObjectsMeshs[k]->getMesh())->getAllJoints();
					for (u32 i = 0; i < AllJoints.size(); ++i)
					{
						ISkinnedMesh::SJoint *joint = AllJoints[i];
						joint->RotationKeys.clear();
						joint->ScaleKeys.clear();
						joint->PositionKeys.clear();
						core::stringc jointName = joint->Name;
						joint->RotationKeys = animationKeymap[animationName]->getRotationKeys(jointName);
						joint->ScaleKeys = animationKeymap[animationName]->getScaleKeys(jointName);
						joint->PositionKeys = animationKeymap[animationName]->getPositionKeys(jointName);
					}
					characterAdditionalObjectsMeshs[k]->setCurrentFrame(0);
					characterAdditionalObjectsMeshs[k]->setAnimationSpeed(animationFrameRate);
					((ISkinnedMesh*)characterAdditionalObjectsMeshs[k]->getMesh())->setFrameBoundaries(
						animationKeymap[animationName]->getFrameCount());
				}
			}
			return true;
		}
		return false;
	}

	// set opponent's throw animations
	void FK_Character::setOpponentThrowAnimation(std::deque<FK_Move>& opponentMoveList, std::string opponentAnimationDirectory){
		for (u32 i = 0; i < opponentMoveList.size(); ++i){
			if (!opponentMoveList[i].getThrowReactionAnimationPose().getAnimationName().empty()){
				//m_threadMutex.lock();
				std::string animation_name = opponentMoveList[i].getThrowReactionAnimationPose().getAnimationName().c_str();
				bool loadedFromCharacterDirectory = addAnimationFromDirectXFile(animation_name.c_str(), animationDirectory + animation_name + ".x");
				if (!loadedFromCharacterDirectory){
					loadedFromCharacterDirectory = addAnimationFromDirectXFile(animation_name.c_str(), sharedAnimationDirectory + animation_name + ".x");
				}
				if (!loadedFromCharacterDirectory) {
					addAnimationFromDirectXFile(animation_name.c_str(), opponentAnimationDirectory + animation_name + ".x");
				}
				//m_threadMutex.unlock();
			}
		}
	}

	/* Animation manager - set new animation (with loop and frames)*/
	bool FK_Character::setNewAnimation(core::stringw animationName, s32 beginFrame, s32 endFrame, bool isLooping){
		if (setAnimation(animationName)){
			animatedMesh->setFrameLoop(beginFrame, endFrame);
			animatedMesh->setLoopMode(isLooping);
			for (int k = 0; k < characterAdditionalObjects.size(); ++k){
				if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
					characterAdditionalObjectsMeshs[k]->setFrameLoop(beginFrame, endFrame);
					characterAdditionalObjectsMeshs[k]->setLoopMode(isLooping);
				}
			}
			return true;
		};
		return false;
	}
	/* Add animation from DirectX file */
	bool FK_Character::addAnimationFromDirectXFile(core::stringw animationName, std::string inputFileName){
		/* if an animation with the same name has already been loaded, return true*/
		if (animationKeymap.count(animationName) > 0){
			return true;
		}
		/* check if input file exists - if yes, proceed. If not, return before attempting to access the mesh */
		std::ifstream testFile(inputFileName.c_str());
		if (testFile.good()){
			testFile.clear();
			testFile.close();
		}
		else{
			testFile.clear();
			testFile.close();
			std::cout << "WARNING: animation file " << inputFileName << " not found." << std::endl;
			return false;
		}
		/* load the mesh which contains the animation data*/
		//m_threadMutex.lock();
		IAnimatedMesh* tempMesh = smgr_reference->getMesh(inputFileName.c_str());
		//m_threadMutex.unlock();
		if (!tempMesh){
			return false;
		}
		u32 newFrameCount = tempMesh->getFrameCount();
		/* return false if such file does not exist */
		if (!tempMesh){
			return false;
		}
		/* create new key in the animation manager map */
		animationKeymap[animationName] = new FK_AnimationKeyMap();
		animationKeymap[animationName]->setFrameCount(newFrameCount);
		/* cycle through the array of joints and store the relevant values*/
		core::array<ISkinnedMesh::SJoint*> AllJoints = ((ISkinnedMesh*)tempMesh)->getAllJoints();
		for (u32 i = 0; i < AllJoints.size(); ++i)
		{
			ISkinnedMesh::SJoint *joint = AllJoints[i];
			core::stringw jointName = joint->Name;
			animationKeymap[animationName]->setRotationArray(jointName, core::array<ISkinnedMesh::SRotationKey>(joint->RotationKeys));
			animationKeymap[animationName]->setPositionArray(jointName, core::array<ISkinnedMesh::SPositionKey>(joint->PositionKeys));
			animationKeymap[animationName]->setScaleArray(jointName, core::array<ISkinnedMesh::SScaleKey>(joint->ScaleKeys));
		}
		/* return success */
		return true;
	};

	// check if player can be thrown
	bool FK_Character::canBeThrown() {
		if (currentMove == NULL || 
			(!currentMove->isActive(getCurrentFrame()) || 
				!currentMove->canBeTriggered() || 
				currentMove->hasThrowAttacks())) {
			return true;
		}
		return false;
	}

	// check if player is thrown
	bool FK_Character::isBeingThrown(){
		return isThrown;
	}
	// check if player is throwing opponent
	bool FK_Character::isThrowingOpponent(){
		return isThrowing;
	}

	// set thrown flag
	void FK_Character::setThrowFlag(bool newFlag){
		isThrown = newFlag;
	}

	// set throw move
	void FK_Character::setThrowMove(FK_Move* throwToApply){
		targetThrowToApply = FK_Move(*throwToApply);
	}

	// set throw move
	void FK_Character::setThrowMoveReaction(FK_Reaction_Type newReaction) {
		if (targetThrowToApply.getMoveId() > 0) {
			targetThrowToApply.setThrowReactionForTarget(newReaction);
		}
	}

	// set throwing flag
	void FK_Character::setThrowingOpponentFlag(bool newFlag){
		isThrowing = newFlag;
	}

	// clear throw move
	void FK_Character::clearThrow(){
		targetThrowToApply.clear();
		if (isBeingThrown()){
			setThrowFlag(false);
		}else if(isThrowingOpponent()){
			setThrowingOpponentFlag(false);
		}
	}

	// get reaction to perform after throw
	FK_Reaction_Type FK_Character::getThrowReaction(){
		if (isBeingThrown()){
			if (targetThrowToApply.getMoveId() > 0){
				return targetThrowToApply.getThrowReactionForTarget();
			}
			else{
				return FK_Reaction_Type::NoReaction;
			}
		}
		else{
			if (currentMove != NULL){
				return currentMove->getThrowReactionForPerformer();
			}
			else{
				return FK_Reaction_Type::NoReaction;
			}
		}
	}

	// get reaction to perform after throw
	FK_Stance_Type FK_Character::getThrowStance(){
		if (isBeingThrown() && targetThrowToApply.getMoveId() > 0){
			return targetThrowToApply.getThrowStanceForTarget();
		}
		return FK_Stance_Type::NoStance;
	}

	//set Thrown animation
	void FK_Character::setThrowAnimation(FK_Pose reactionAnimation, core::vector3df rotationAngle){
		setRotation(rotationAngle);
		std::string animationName = reactionAnimation.getAnimationName();
		bool loaded = addAnimationFromDirectXFile(animationName.c_str(), animationDirectory + animationName + ".x");
		core::stringw newAnimationName = core::stringw(animationName.c_str());
		if (!loaded){
			loaded = addAnimationFromDirectXFile(newAnimationName, sharedAnimationDirectory + animationName + ".x");
		}
		if (loaded){
			setAnimation(newAnimationName);
		}
		animatedMesh->setFrameLoop(reactionAnimation.getStartingFrame(), reactionAnimation.getEndingFrame());
		animatedMesh->setLoopMode(false);
		for (int k = 0; k < characterAdditionalObjects.size(); ++k){
			if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
				characterAdditionalObjectsMeshs[k]->setFrameLoop(reactionAnimation.getStartingFrame(), reactionAnimation.getEndingFrame());
				characterAdditionalObjectsMeshs[k]->setLoopMode(false);
			}
		}
		setThrowFlag(true);
		//std::cout << "Starting throw with animation " << animationName << std::endl;
	}

	// read intro quotes from file
	void FK_Character::loadIntroQuotes(){
		/* generate "generic sentence" array */
		std::string currentKey = "generic";
		introQuotes[currentKey] = std::vector<std::wstring>();
		/* if this keyword is found, a new sentence is added against a character with the specific name*/
		std::string characterKeyword = "#VS";
		std::string sameCharacterKeyword = "#SELF";
		std::string quotesFileName = characterDirectory + characterOutfits[outfitId].outfitDirectory + "introQuotes.txt";
		std::ifstream inputFile(quotesFileName.c_str());
		if (!inputFile){
			quotesFileName = characterDirectory + "introQuotes.txt";
			inputFile.clear();
			inputFile.close();
			inputFile = std::ifstream(quotesFileName.c_str());
			if (!inputFile){
				return;
			}
		}
		std::string line;
		while (!inputFile.eof()){
			bool lineIsKeyword = false;
			std::getline(inputFile, line);
			if (line.empty()) continue;
			/* check if the line contains any keyword */
			std::istringstream temp_stream(line);
			std::string temp;
			while (!temp_stream.eof()){
				temp_stream >> temp;
				if (strcmp(temp.c_str(), characterKeyword.c_str()) == 0){
					lineIsKeyword = true;
					/* read new key */
					temp_stream >> temp;
					/* replace "_" with spaces */
					std::replace(temp.begin(), temp.end(), '_', ' ');
					/* check if the key is matching the #SELF key*/
					if (strcmp(temp.c_str(), sameCharacterKeyword.c_str()) == 0){
						temp = getName();
					}
					/* set new key */
					currentKey = temp;
					/* create character vector is does not exist yet*/
					if (!introQuotes.count(currentKey)){
						introQuotes[currentKey] = std::vector<std::wstring>();
					}
					/* break while and go to next cycle */
					break;
				}
			}
			if (lineIsKeyword){
				continue;
			}
			else{
				/* add sentence to specific character vector */
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				introQuotes[currentKey].push_back(tempLine);
			}
		}
		inputFile.close();
	}

	// read intro win quotes from file
	void FK_Character::loadIntroQuotesFromCharacterDirectory(std::string externalCharacterPath,
		std::string externalCharacterName,
		FK_CharacterOutfit externalCharacterOutfit){
		/* if this keyword is found, a new sentence is added against a character with the specific name*/
		std::string quotesFileName = externalCharacterPath + externalCharacterOutfit.outfitDirectory + "opponentIntroQuotes.txt";
		std::ifstream inputFile(quotesFileName.c_str());
		if (!inputFile){
			quotesFileName = externalCharacterPath + "opponentIntroQuotes.txt";
			inputFile.clear();
			inputFile.close();
			inputFile = std::ifstream(quotesFileName.c_str());
			if (!inputFile){
				return;
			}
		}
		else{
			if (!externalCharacterOutfit.outfitCharacterName.empty()){
				externalCharacterName = externalCharacterOutfit.outfitCharacterName;
			}
		}
		/* generate "generic sentence" array */
		std::string currentKey = externalCharacterName;
		std::string characterKeyword = "#VS";
		std::string line;
		bool recordSentence = false;
		while (!inputFile.eof()){
			bool lineIsKeyword = false;
			std::getline(inputFile, line);
			if (line.empty()) continue;
			/* check if the line contains any keyword */
			std::istringstream temp_stream(line);
			std::string temp;
			while (!temp_stream.eof()){
				temp_stream >> temp;
				if (temp == characterKeyword){
					lineIsKeyword = true;
					/* read new key */
					temp_stream >> temp;
					/* replace "_" with spaces */
					std::replace(temp.begin(), temp.end(), '_', ' ');
					std::string name = getName();
					if (temp != name){
						recordSentence = false;
						continue;
					}
					else{
						recordSentence = true;
						/* create character vector is does not exist yet and clear it if it exist*/
						if (!introQuotes.count(currentKey)){
							introQuotes[currentKey] = std::vector<std::wstring>();
						}
						else{
							introQuotes[currentKey].clear();
						}
					}
					/* break while and go to next cycle */
					break;
				}
			}
			if (lineIsKeyword){
				continue;
			}
			else if (recordSentence){
				/* add sentence to specific character vector */
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				introQuotes[currentKey].push_back(tempLine);
			}
		}
		inputFile.close();
	}

	// read intro win quotes from file
	void FK_Character::loadWinQuotes(){
		std::string quotesFileName = characterDirectory + characterOutfits[outfitId].outfitDirectory + "winQuotes.txt";
		std::ifstream inputFile(quotesFileName.c_str());
		if (!inputFile){
			quotesFileName = characterDirectory + "winQuotes.txt";
			inputFile.clear();
			inputFile.close();
			inputFile = std::ifstream(quotesFileName.c_str());
			if (!inputFile){
				return;
			}
		}
		/* generate "generic sentence" array */
		std::string currentKey = "generic";
		roundWinQuotes[currentKey] = std::vector<std::wstring>();
		matchWinQuotes[currentKey] = std::vector<std::wstring>();
		/* if this keyword is found, a new sentence is added against a character with the specific name*/
		std::string characterKeyword = "#VS";
		std::string sameCharacterKeyword = "#SELF";
		std::string roundKeyword = "#ROUND";
		std::string matchKeyword = "#MATCH";
		bool processingMatchLines = false;
		std::string line;
		while (!inputFile.eof()){
			bool lineIsKeyword = false;
			std::getline(inputFile, line);
			if (line.empty()) continue;
			/* check if the line contains any keyword */
			std::istringstream temp_stream(line);
			std::string temp;
			while (!temp_stream.eof()){
				temp_stream >> temp;
				if (temp == characterKeyword){
					lineIsKeyword = true;
					/* read new key */
					temp_stream >> temp;
					/* replace "_" with spaces */
					std::replace(temp.begin(), temp.end(), '_', ' ');
					/* check if the key is matching the #SELF key*/
					if (temp == sameCharacterKeyword){
						temp = getName();
					}
					/* set new key */
					currentKey = temp;
					/* create character vector is does not exist yet*/
					if (!roundWinQuotes.count(currentKey)){
						roundWinQuotes[currentKey] = std::vector<std::wstring>();
						matchWinQuotes[currentKey] = std::vector<std::wstring>();
					}
					/* break while and go to next cycle */
					/* reset the "processingMatchLines" bool */
					processingMatchLines = false;
					break;
				}
				if (strcmp(temp.c_str(), roundKeyword.c_str()) == 0){
					processingMatchLines = false;
					std::getline(temp_stream, line);
				}
				if (strcmp(temp.c_str(), matchKeyword.c_str()) == 0){
					processingMatchLines = true;
					std::getline(temp_stream, line);
				}
			}
			if (lineIsKeyword){
				continue;
			}
			else{
				/* add sentence to specific character vector */
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				if (processingMatchLines){
					matchWinQuotes[currentKey].push_back(tempLine);
				}
				else{
					roundWinQuotes[currentKey].push_back(tempLine);
				}
			}
		}
		inputFile.close();
	}

	// read win quotes from file
	void FK_Character::loadWinQuotesFromCharacterDirectory(std::string externalCharacterPath, 
		std::string externalCharacterName,
		FK_CharacterOutfit externalCharacterOutfit){
		std::string quotesFileName = externalCharacterPath + externalCharacterOutfit.outfitDirectory + "opponentWinQuotes.txt";
		std::ifstream inputFile(quotesFileName.c_str());
		if (!inputFile){
			quotesFileName = externalCharacterPath + "opponentWinQuotes.txt";
			inputFile.clear();
			inputFile.close();
			inputFile = std::ifstream(quotesFileName.c_str());
			if (!inputFile){
				return;
			}
		}
		else{
			if (!externalCharacterOutfit.outfitCharacterName.empty()){
				externalCharacterName = externalCharacterOutfit.outfitCharacterName;
			}
		}
		/* generate "generic sentence" array */
		std::string currentKey = externalCharacterName;
		/* if this keyword is found, a new sentence is added against a character with the specific name*/
		std::string characterKeyword = "#VS";
		std::string roundKeyword = "#ROUND";
		std::string matchKeyword = "#MATCH";
		bool processingMatchLines = false;
		std::string line;
		bool recordSentence = false;
		while (!inputFile.eof()){
			bool lineIsKeyword = false;
			std::getline(inputFile, line);
			if (line.empty()) continue;
			/* check if the line contains any keyword */
			std::istringstream temp_stream(line);
			std::string temp;
			while (!temp_stream.eof()){
				temp_stream >> temp;
				if (temp == characterKeyword){
					lineIsKeyword = true;
					/* read new key */
					temp_stream >> temp;
					/* replace "_" with spaces */
					std::replace(temp.begin(), temp.end(), '_', ' ');
					if (temp != getName()){
						recordSentence = false;
						continue;
					}
					recordSentence = true;
					/* create character vector is does not exist yet, otherwise clean it*/
					if (!roundWinQuotes.count(currentKey)){
						roundWinQuotes[currentKey] = std::vector<std::wstring>();
						matchWinQuotes[currentKey] = std::vector<std::wstring>();
					}
					else{
						roundWinQuotes[currentKey].clear();
						matchWinQuotes[currentKey].clear();
					}
					/* break while and go to next cycle */
					/* reset the "processingMatchLines" bool */
					processingMatchLines = false;
					break;
				}
				if (temp == roundKeyword){
					processingMatchLines = false;
					std::getline(temp_stream, line);
				}
				if (temp == matchKeyword){
					processingMatchLines = true;
					std::getline(temp_stream, line);
				}
			}
			if (lineIsKeyword){
				continue;
			}
			else if (recordSentence){
				/* add sentence to specific character vector */
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				if (processingMatchLines){
					matchWinQuotes[currentKey].push_back(tempLine);
				}
				else{
					roundWinQuotes[currentKey].push_back(tempLine);
				}
			}
		}
		inputFile.close();
	}

	// override intro quotes
	void FK_Character::overrideIntroQuotes(std::vector<std::wstring> newIntroQuotes, std::string characterName){
		introQuotes[characterName] = newIntroQuotes;
	}
	// override round win quotes
	void FK_Character::overrideRoundWinQuotes(std::vector<std::wstring> newRoundWinQuotes, std::string characterName){
		roundWinQuotes[characterName] = newRoundWinQuotes;
	}
	// override match win quotes
	void FK_Character::overrideMatchWinQuotes(std::vector<std::wstring> newMatchWinQuotes, std::string characterName){
		matchWinQuotes[characterName] = newMatchWinQuotes;
	}

	/*get intro quotes */
	std::wstring FK_Character::getRandomIntroQuote(std::string key){
		if (introQuotes.count(key) == 0){
			key = "generic";
		}
		double quotesNumber = (introQuotes[key]).size();
		if (quotesNumber == 0) return std::wstring();
		double random = ((double)rand() / (RAND_MAX));
		int index = (int)floor(random*quotesNumber);
		return introQuotes[key][index];
	};
	std::vector<std::wstring> &FK_Character::getIntroQuotes(std::string key){
		if (roundWinQuotes.count(key) == 0){
			key = "generic";
		}
		return introQuotes[key];
	};

	/* select random win quote */
	std::wstring FK_Character::getRandomWinQuote(std::string key, bool endMatchFlag){
		if (endMatchFlag){
			return getRandomMatchWinQuote(key);
		}
		else{
			return getRandomRoundWinQuote(key);
		}
	};
	/* select random quote for round winning */
	std::wstring FK_Character::getRandomRoundWinQuote(std::string key){
		if (roundWinQuotes.count(key) == 0 || roundWinQuotes[key].empty()){
			key = "generic";
		}
		double quotesNumber = roundWinQuotes[key].size();
		if (quotesNumber == 0) return std::wstring();
		double random = ((double)rand() / (RAND_MAX));
		int index = (int)floor(random*quotesNumber);
		return roundWinQuotes[key][index];
	};
	/* select random quote for match winning */
	std::wstring FK_Character::getRandomMatchWinQuote(std::string key){
		if (matchWinQuotes.empty() || (matchWinQuotes[key].empty() && matchWinQuotes["generic"].empty())){
			return getRandomRoundWinQuote(key);
		}
		if (!matchWinQuotes.count(key) || matchWinQuotes[key].empty()){
			key = "generic";
		}
		double quotesNumber = matchWinQuotes[key].size();
		if (quotesNumber == 0) return std::wstring();
		double random = ((double)rand() / (RAND_MAX));
		int index = (int)floor(random*quotesNumber);
		return matchWinQuotes[key][index];
	};
	std::vector<std::wstring> &FK_Character::getRoundWinQuotes(std::string key){
		if (!roundWinQuotes.count(key)){
			key = "generic";
		}
		return roundWinQuotes[key];
	};
	std::vector<std::wstring> &FK_Character::getMatchWinQuotes(std::string key){
		if (!matchWinQuotes.count(key)){
			key = "generic";
		}
		return matchWinQuotes[key];
	};

	// get character directory
	std::string FK_Character::getCharacterDirectory(){
		return characterDirectory;
	}

	/* bullets */
	/* manage bullets */
	std::deque<FK_Bullet*> & FK_Character::getBulletsCollection(){
		return bulletsCollection;
	}
	/* check if bullet can be erased  - this is not a member of the "character class" and it's used 
	only to remove expired bullets*/
	static bool canEraseBullet(const FK_Bullet* bullet) {
		return bullet->canBeRemoved();
	}
	/* update bullets */
	void FK_Character::updateBullets(f32 frameDeltaTime){
		if (currentMove != NULL && currentMove->hasActiveMoveCinematic(getCurrentFrame())) {
			frameDeltaTime *= currentMove->getCinematic().opponentTimerMultiplier;
		}
		for each (FK_Bullet* bullet in bulletsCollection){
			bullet->update(frameDeltaTime);
			if (bullet->isDisposeable() && bullet->getNode()->isVisible()){
				bullet->prepareForDisposal();
				if (!bullet->getHitbox().hasHit() && bullet->canBeRemoved()){
					activateParticleEffect(bullet->getPosition());
					//std::cout << "Effect activated..." << std::endl;
				}
			}
		}
		// the removal of the bullets has been delegated to the game scene, dure to additional issues with lights
		/*
		bulletsCollection.erase(std::remove_if(bulletsCollection.begin(), bulletsCollection.end(), canEraseBullet),
			bulletsCollection.end());
			*/
	}

	/* update bullets */
	void FK_Character::markAllBulletsForDisposal() {
		for each (FK_Bullet* bullet in bulletsCollection) {
			bullet->setRange(-1.f);
			if (bullet->getNode()->isVisible()) {
				bullet->prepareForDisposal();
				if (!bullet->getHitbox().hasHit() && bullet->canBeRemoved()) {
					activateParticleEffect(bullet->getPosition());
				}
			}
		}
		// the removal of the bullets has been delegated to the game scene, dure to additional issues with lights
		/*
		bulletsCollection.erase(std::remove_if(bulletsCollection.begin(), bulletsCollection.end(), canEraseBullet),
		bulletsCollection.end());
		*/
	}
	/* toggle animation transition */
	void FK_Character::allowAnimationTransitions(){
		animatedMesh->setTransitionTime(animationTransitionTime);
		for (int i = 0; i < characterAdditionalObjectsMeshs.size(); ++i){
			if (!characterAdditionalObjects[i].hasDummyMesh()){
				characterAdditionalObjectsMeshs[i]->setTransitionTime(animationTransitionTime);
			}
		}
	}
	void FK_Character::stopAnimationTransitions(){
		animatedMesh->setTransitionTime(0.0f);
		for (int i = 0; i < characterAdditionalObjectsMeshs.size(); ++i){
			if (!characterAdditionalObjects[i].hasDummyMesh()){
				characterAdditionalObjectsMeshs[i]->setTransitionTime(0.0f);
			}
		}
	}

	/* recover from grounded position */
	void FK_Character::recoverFromGrounding(bool setIdleStance){
		hasRotatedOnTheGround = false;
		groundRotationDirection = FK_GroundRotationDirection::NoRotation;
		animatedMesh->setAnimationSpeed(animationFrameRate);
		rollingTimeCounterMs = 0;
		if (setIdleStance && !hitWhileGrounded){
			setBasicAnimation(FK_BasicPose_Type::SupineWakeUpAnimation, true);
			setStance(FK_Stance_Type::WakeUpStance);
			setTrackingFlag(true);
		}
		hitWhileGrounded = false;
	}

	/* get special stance dictionary (for move list)*/
	std::map<FK_Stance_Type, std::string> FK_Character::getSpecialStanceDictionary(){
		return specialStanceDictionary;
	}

	/* get invincibility */
	bool FK_Character::isInvincible(FK_Attack_Type type){
		bool wakeUpFlag = getStance() == FK_Stance_Type::WakeUpStance;
		if (wakeUpFlag) {
			return true;
		}
		//// temporary!
		//wakeUpFlag = false;
		if (isBeingThrown()) {
			return false;
		}
		bool moveFlag = false;
		if ((type & FK_Attack_Type::HighAtks) != 0 && getStance() == FK_Stance_Type::CrouchedStance){
			return true;
		}
		if (currentMove != NULL){
			moveFlag = currentMove->isInvincible((u32)std::floor(animatedMesh->getFrameNr()), type);
		}
		if (moveFlag) {
			return true;
		}
		bool jumpFlag = false;
		if (isJumping() && (type & FK_Attack_Type::LowAtks) != 0){
			jumpFlag = true;
		}
		if (jumpFlag) {
			return true;
		}
		// ground rollling flag
		bool rollingLowInvincibilityFlag = false;
		if (rollingTimeCounterMs > 0 && (getStance() & FK_Stance_Type::AnyGroundedStance)) {
			rollingLowInvincibilityFlag = (type & FK_Attack_Type::LowAtks);
		}
		bool objectFlag = (getTotalObjectInvincibility() & type) != 0;
		return (rollingLowInvincibilityFlag || moveFlag || jumpFlag || objectFlag);
	}

	/* get armor  */
	bool FK_Character::hasArmor(FK_Attack_Type type) {
		if (isBeingThrown()) {
			return false;
		}
		bool moveFlag = false;
		if (currentMove != NULL) {
			moveFlag = currentMove->hasArmor((u32)std::floor(animatedMesh->getFrameNr()), type);
		}
		if (moveFlag) {
			return true;
		}
		bool objectFlag = (getTotalObjectArmor() & type) != 0;
		return moveFlag || objectFlag;
	}

	/* get AI archetype*/
	FK_Character::FK_AIArchetypes FK_Character::getAIArchetype(){
		return AIarchetype;
	}

	/* set ground collision flag */
	void FK_Character::setGroundCollisionFlag(bool newFlag){
		hasTouchedGround = newFlag;
	}

	/* get ground collision flag */
	bool FK_Character::getGroundCollisionFlag(){
		return hasTouchedGround;
	}

	/* update jumping routine */
	void FK_Character::updateJump(u32 delta_t_ms){
		if (jumpingFlag){
			jumpCounterMs += delta_t_ms;
		}
		else{
			jumpCounterMs = 0;
		}
	}

	// load additional hitboxes
	void FK_Character::loadAdditionalHitboxes(){
		std::string HitboxKey = "#newHitbox_begin";
		std::string HitboxEndKey = "#newHitbox_end";
		// bone map
		std::map<std::string, FK_Bones_Type> stringToBoneMap;
		stringToBoneMap["HEAD"] = FK_Bones_Type::HeadArmature;
		stringToBoneMap["NECK"] = FK_Bones_Type::NeckArmature;
		stringToBoneMap["TORSO"] = FK_Bones_Type::TorsoArmature;
		stringToBoneMap["SPINE"] = FK_Bones_Type::SpineArmature;
		stringToBoneMap["HIPS"] = FK_Bones_Type::HipsArmature;
		stringToBoneMap["SHOULDER_L"] = FK_Bones_Type::LeftArmArmature;
		stringToBoneMap["SHOULDER_R"] = FK_Bones_Type::RightArmArmature;
		stringToBoneMap["ELBOW_L"] = FK_Bones_Type::LeftForearmArmature;
		stringToBoneMap["ELBOW_R"] = FK_Bones_Type::RightForearmArmature;
		stringToBoneMap["WRIST_L"] = FK_Bones_Type::LeftWristArmature;
		stringToBoneMap["WRIST_R"] = FK_Bones_Type::RightWristArmature;
		stringToBoneMap["LEG_L"] = FK_Bones_Type::LeftLegArmature;
		stringToBoneMap["LEG_R"] = FK_Bones_Type::RightLegArmature;
		stringToBoneMap["KNEE_L"] = FK_Bones_Type::LeftKneeArmature;
		stringToBoneMap["KNEE_R"] = FK_Bones_Type::RightKneeArmature;
		stringToBoneMap["ANKLE_L"] = FK_Bones_Type::LeftAnkleArmature;
		stringToBoneMap["ANKLE_R"] = FK_Bones_Type::RightAnkleArmature;
		stringToBoneMap["FOOT_L"] = FK_Bones_Type::LeftFootArmature;
		stringToBoneMap["FOOT_R"] = FK_Bones_Type::RightFootArmature;
		stringToBoneMap["THUMB_L"] = FK_Bones_Type::LeftThumbArmatureTip;
		stringToBoneMap["THUMB_R"] = FK_Bones_Type::RightThumbArmatureTip;
		stringToBoneMap["FINGERS_L"] = FK_Bones_Type::LeftFingersArmatureMedium;
		stringToBoneMap["FINGERS_R"] = FK_Bones_Type::RightFingersArmatureMedium;
		stringToBoneMap["EXTRA_1"] = FK_Bones_Type::AdditionalArmature01;
		stringToBoneMap["EXTRA_2"] = FK_Bones_Type::AdditionalArmature02;
		stringToBoneMap["EXTRA_3"] = FK_Bones_Type::AdditionalArmature03;
		stringToBoneMap["EXTRA_4"] = FK_Bones_Type::AdditionalArmature04;
		stringToBoneMap["EXTRA_5"] = FK_Bones_Type::AdditionalArmature05;
		stringToBoneMap["EXTRA_6"] = FK_Bones_Type::AdditionalArmature06;
		stringToBoneMap["EXTRA_7"] = FK_Bones_Type::AdditionalArmature07;
		stringToBoneMap["EXTRA_8"] = FK_Bones_Type::AdditionalArmature08;
		stringToBoneMap["EXTRA_9"] = FK_Bones_Type::AdditionalArmature09;
		stringToBoneMap["EXTRA_10"] = FK_Bones_Type::AdditionalArmature10;
		// hitbox map
		std::map<std::string, FK_Hitbox_Type> hitboxTypeMap;
		hitboxTypeMap["Extra1"] = FK_Hitbox_Type::AdditionalHitbox1;
		hitboxTypeMap["Extra2"] = FK_Hitbox_Type::AdditionalHitbox2;
		hitboxTypeMap["Extra3"] = FK_Hitbox_Type::AdditionalHitbox3;
		hitboxTypeMap["Extra4"] = FK_Hitbox_Type::AdditionalHitbox4;
		hitboxTypeMap["Extra5"] = FK_Hitbox_Type::AdditionalHitbox5;
		hitboxTypeMap["Extra6"] = FK_Hitbox_Type::AdditionalHitbox6;
		hitboxTypeMap["Extra7"] = FK_Hitbox_Type::AdditionalHitbox7;
		hitboxTypeMap["Extra8"] = FK_Hitbox_Type::AdditionalHitbox8;
		hitboxTypeMap["Extra9"] = FK_Hitbox_Type::AdditionalHitbox9;
		hitboxTypeMap["Extra10"] = FK_Hitbox_Type::AdditionalHitbox10;
		std::string temp;
		std::string configFile = characterDirectory + "additionalHitboxes.txt";
		FK_Character::FK_CharacterAdditionalObject newObject;
		std::ifstream configurationFile(configFile.c_str());
		if (!configurationFile){
			return;
		}
		if (!hitboxCollection.empty()){
			while (configurationFile >> temp){
				if (temp == HitboxKey){
					while (configurationFile){
						configurationFile >> temp;
						if (temp == HitboxEndKey){
							break;
						}
						if (hitboxTypeMap.count(temp) == 0){
							break;
						}
						FK_Hitbox_Type type = hitboxTypeMap[temp];
						configurationFile >> temp;
						if (stringToBoneMap.count(temp) == 0){
							break;
						}
						std::string boneName = armatureBonesCollection[stringToBoneMap[temp]];
						f32 x, y, z;
						configurationFile >> x >> y >> z;
						core::vector3df offsetPosition(x, y, z);
						configurationFile >> x >> y >> z;
						core::vector3df hitboxScale(x, y, z);
						scene::ISceneNode * hitbox = smgr_reference->addSphereSceneNode();
						hitbox->setMaterialFlag(video::EMF_LIGHTING, false);
						hitbox->setMaterialFlag(video::EMF_WIREFRAME, true);
						scene::IBoneSceneNode* bone = animatedMesh->getJointNode(boneName.c_str());
						if (bone == NULL){
							continue;
						}
						hitbox->setParent(bone);
						hitbox->setScale(hitboxScale);
						hitbox->setPosition(offsetPosition);
						//hitbox->setDebugDataVisible(scene::EDS_BBOX);
						hitbox->setVisible(false);
						hitboxCollection[type] = hitbox;
						hitbox->updateAbsolutePosition();
					}
				}
			}
		}
	}

	/* damage multipliers */
	f32 FK_Character::getReceivedPhysicalDamageMultiplier(){
		return receivedPhysicalDamageMultiplier;
	}
	f32 FK_Character::getReceivedBulletDamageMultiplier(){
		return receivedBulletDamageMultiplier;
	}
	void FK_Character::setReceivedPhysicalDamageMultiplier(f32 newMultiplier){
		receivedPhysicalDamageMultiplier = newMultiplier;
	}
	void FK_Character::setReceivedBulletDamageMultiplier(f32 newMultiplier){
		receivedBulletDamageMultiplier = newMultiplier;
	}

	/*tracking variables - they tell the scene to rotate the player towards the opponent when the move ends*/
	void FK_Character::setTrackingFlag(bool flag){
		opponentTrackingFlag = flag;
	}
	bool FK_Character::hasToTrackOpponent(){
		return opponentTrackingFlag;
	}

	/* sound variables (to avoid chaos) */
	bool FK_Character::canPlayHitSound(){
		return hitSoundTimer <= 0;
	}
	void FK_Character::resetSoundTimer(){
		// 200 ms between sounds
		hitSoundTimer = 300;
	}
	void FK_Character::updateSoundTimer(u32 delta_t_ms){
		if (hitSoundTimer > 0){
			hitSoundTimer -= (s32)delta_t_ms;
		}
	}

	// pushback
	bool FK_Character::hasPushback() {
		return pushbackDuration >= 0;
	}

	void FK_Character::updatePushback(u32 delta_t_ms) {
		if (hasPushback()) {
			pushbackDuration -= delta_t_ms;
		}
	}

	void FK_Character::activatePushback() {
		//pushbackDuration = FK_Character::MaximumPushbackDuration;
		if (databaseAccessor.blockPushbackAllowed()) {
			pushbackVelocity = core::vector3df(databaseAccessor.getPushbackVelocity(), 0.f, 0.f);
			pushbackDuration = databaseAccessor.getMaxPushbackDurationMs();
		}
	}

	//check for transformations
	void FK_Character::checkForTransformation(u32 delta_t_ms) {
		// if the character is already transforming, don't bother checking over
		if (isTransforming) {
			return;
		}
		// reset to base form if the transformation time expires
		if (currentForm.durationMs > 0) {
			currentForm.durationMs -= (s32)delta_t_ms;
			if (currentForm.durationMs <= 0) {
				if (isKO() || currentMove != NULL) {
					currentForm.durationMs = 1;
				}
				else {
					if (currentForm.previousFormNameTag != std::string() &&
						availableTransformations.count(currentForm.previousFormNameTag) > 0) {
						prepareTransformation(availableTransformations[currentForm.previousFormNameTag]);
						isInBaseForm = false;
					}
					else {
						prepareTransformation(baseForm);
						isInBaseForm = true;
					}
				}
				return;
			}
		}
		// if no move is being performed, abort
		if(getCurrentMove() == NULL ||
			!getCurrentMove()->hasTransformation()) {
			return;
		}
		s32 currentFrame = getCurrentFrame();
		if (getCurrentMove()->hasTransformationAtFrame(currentFrame)) {
			f32 previousFrame = animatedMesh->getFrameNr();
			if (getCurrentMove()->getTransformation().isBasicForm) {
				prepareTransformation(baseForm);
				isInBaseForm = true;
			}
			else {
				FK_Move::FK_TransformationMove transformation = getCurrentMove()->getTransformation();
				prepareTransformation(transformation);
				isInBaseForm = false;
			}
			//std::string newAnimationName = currentMove->getAnimationName(isOnLeftSide());
			//bool loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), animationDirectory + newAnimationName + ".x");
			////bool loaded = false;
			//if (!loaded) {
			//	loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), sharedAnimationDirectory + newAnimationName + ".x");
			//}
			//if (loaded) {
			//	setAnimation(newAnimationName.c_str());
			//}
			//animatedMesh->setFrameLoop(currentMove->getStartingFrame(), currentMove->getEndingFrame());
			//animatedMesh->setCurrentFrame(previousFrame);
			//animatedMesh->setAnimationSpeed(animationFrameRate);
			//animatedMesh->setTransitionTime(0);
			//resetTransitionTime = true;
		}
	}

	//check for transformations
	void FK_Character::performTransformation() {
		// mesh
		f32 previousFrame = animatedMesh->getFrameNr();
		std::wstring tempString = std::wstring(getCurrentAnimationName().c_str());
		std::string newAnimationName = std::string(tempString.begin(), tempString.end());
		s32 loopStart = animatedMesh->getStartFrame();
		s32 loopEnd = animatedMesh->getEndFrame();

		FK_Move::FK_TransformationMove transformation = transformationToApply;
		if (transformation.newMeshName != std::string()) {
			std::string meshfile = (characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory +
				transformation.newMeshName);
			ISkinnedMesh* tempmesh = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
			if (tempmesh == NULL) {
				meshfile = (characterDirectory + transformation.newMeshName);
				tempmesh = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
			}
			currentAnimationName = core::stringw();
			for (int i = 0; i < getCharacterAdditionalObjects().size(); ++i) {
				if (!characterAdditionalObjects[i].hasDummyMesh()) {
					characterAdditionalObjectsMeshs[i]->remove();
				}
			}
			characterAdditionalObjectsMeshs.clear();
			for (auto it = hitboxCollection.begin(); it != hitboxCollection.end(); ++it)
			{
				it->second->remove();
			}
			hitboxCollection.clear();
			for (auto it = hurtboxCollection.begin(); it != hurtboxCollection.end(); ++it)
			{
				it->second->remove();
			}
			hurtboxCollection.clear();
			mesh = tempmesh;
			mesh->convertMeshToTangents();
			animatedMesh->setMesh(mesh);
			animatedMesh->setJointMode(irr::scene::EJUOR_CONTROL);
			animatedMesh->setAnimationEndCallback(animationCallback);
			int mcount = animatedMesh->getMaterialCount();
			animatedMesh->setMaterialFlag(video::EMF_LIGHTING, false);
			for (int i = 0; i < mcount; i++) {
				animatedMesh->getMaterial(i).BackfaceCulling = setBackfaceCulling;
				animatedMesh->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
				animatedMesh->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			/* parse bones after having determined the filename*/
			parseBones();
			/* load additional objects */
			fillHitboxCollection();
			/* add non-standard hitboxes */
			loadAdditionalHitboxes();
			/* create standard hurtbox */
			fillHurtboxCollection();
			loadAdditionalObjectsMesh();
			if (currentMove == NULL) {
				isTransforming = false;
			}
		}
		// check for new stances and replace old ones
		for (auto& stancePair : transformation.newStances) {
			if (stancePair.second.getAnimationName() != std::string()) {
				basicAnimations[stancePair.first] = stancePair.second;
			}
		}
		currentForm = transformation;

		// load animations 
		bool loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), animationDirectory + newAnimationName + ".x");
		//bool loaded = false;
		if (!loaded) {
			loaded = addAnimationFromDirectXFile(newAnimationName.c_str(), sharedAnimationDirectory + newAnimationName + ".x");
		}
		if (loaded) {
			setAnimation(newAnimationName.c_str());
		}
		animatedMesh->setFrameLoop(loopStart, loopEnd);
		animatedMesh->setCurrentFrame(previousFrame);
		animatedMesh->setAnimationSpeed(animationFrameRate);
		animatedMesh->setTransitionTime(0);
		resetTransitionTime = true;
		transformationToApply.hasBeenPerformed = true;
	}

	//check for transformations
	void FK_Character::prepareTransformation(FK_Move::FK_TransformationMove transformation) {
		isTransforming = true;
		transformationToApply = transformation;
	}

	bool FK_Character::hasToTransform() {
		return isTransforming && !transformationToApply.hasBeenPerformed;
	}

	bool FK_Character::hasActiveTransformation() {
		return !isInBaseForm;
	}
	const std::string& FK_Character::getCurrentTransformationId() {
		return currentForm.transformationNameTag;
	}
	const std::vector<std::string>& FK_Character::getAvailableTransformationNames() {
		return availableTransformationsNames;
	}

	/* special effects */

	//! TRIGGER
	bool FK_Character::hasActiveTriggerCancelSpecialEffect() {
		return impactCancelShaderEffectTimer >= 0;
	}

	//! ARMOR
	void FK_Character::activateArmorSpecialEffect() {
		specialEffectArmorTimeCounterMS = 0;
		specialEffectArmor = true;
	}
	void FK_Character::disableArmorSpecialEffect() {
		specialEffectArmorTimeCounterMS = 0;
		specialEffectArmor = false;
	}
	bool FK_Character::hasActiveArmorSpecialEffect() {
		return specialEffectArmor;
	}

	/* guard transition */
	bool FK_Character::canUpdateGuardTransition()
	{
		if (isBeingThrown()) {
			return false;
		}
		return ( isGuarding() && (isHitStun() && (hitStunReferenceTime >= 100)));
	}

	// avoid low guard lock due to hitstun 
	void FK_Character::updateGuardTransition(u32 lastButtonPressed)
	{
		// check guard
		u32 actionButtonsPressed = lastButtonPressed & FK_Input_Buttons::Any_NonDirectionButton;
		bool onlyGuardButtonPressed = 
			(actionButtonsPressed & fk_constants::FK_GuardButton) == fk_constants::FK_GuardButton;
		bool guardButtonPressed = (actionButtonsPressed & fk_constants::FK_GuardButton) != 0;
		if (isGuarding() && guardButtonPressed) {
			u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
			if (getStance() == FK_Stance_Type::GroundStance) {
				if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) != 0) {
					setStance(FK_Stance_Type::CrouchedStance);
					setBasicAnimation(FK_BasicPose_Type::CrouchingGuardAnimation, true);
				}
				/*else if ((directionButtonPressed & FK_Input_Buttons::Up_Direction) != 0) {
					u32 directionToCheck = FK_Input_Buttons::Right_Direction;
					if (isOnLeftSide()) {
						directionToCheck = FK_Input_Buttons::Left_Direction;
					}
					if (directionButtonPressed & directionToCheck) {
						cancelHitstun();
						performJump(FK_JumpDirection::JumpBackward);
					}
				}*/
			}
			else if (getStance() == FK_Stance_Type::CrouchedStance) {
				u32 directionButtonPressed = lastButtonPressed & FK_Input_Buttons::Any_Direction_Plus_Held;
				if ((directionButtonPressed & FK_Input_Buttons::Down_Direction) == 0) {
					setStance(FK_Stance_Type::GroundStance);
					setBasicAnimation(FK_BasicPose_Type::GuardingAnimation, true);
				}
			}
		}
	}

	/* set last damaging move for tutorial and trials */
	void FK_Character::setLastDamagingMoveId(u32 moveId)
	{
		lastMoveReceivedDamageId = moveId;
	}

	/* get last damaging move for tutorial and trials */
	u32 FK_Character::getLastDamagingMoveId()
	{
		return lastMoveReceivedDamageId;
	}
};