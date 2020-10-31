#include"FK_SceneDiorama.h"

namespace fk_engine {

	/* create generic initializer */
	FK_SceneDiorama::FK_SceneDiorama() : FK_SceneExtra(){
		initialize();
	}

	void FK_SceneDiorama::initialize() {
		extraSceneType = FK_Scene::FK_SceneExtraType::ExtraDiorama;
		backToCharacterSelection = false;
		cycleId = 0;
		now = 0;
		then = 0;
		introCounter = 0;
		outroCounter = 0;
		scenePixelShaderParameters = new f32[4];
		for (u32 i = 0; i < 4; ++i) {
			scenePixelShaderParameters[i] = 0.0f;
		}
		dioramaMode = FK_DioramaMode::DioramaMenu;
		// setup cameras
		characterCamera.cameraAngle = InitialCameraAngle;
		characterCamera.cameraOffset = InitialYOffset;
		characterCamera.cameraDistance = InitialCameraDistance;

		defaultCamera.cameraAngle = InitialCameraAngle;
		defaultCamera.cameraOffset = InitialYOffset;
		defaultCamera.cameraDistance = InitialCameraDistance;

		stageCamera.cameraAngle = 0;
		stageCamera.cameraOffset = StageCameraOffset;
		stageCamera.cameraDistance = StageCameraDistance;
		stageCamera.cameraTargetOffset = StageCameraTargetOffset;

		characterPose = FK_Pose();
	}
	/* setup all objects */
	void FK_SceneDiorama::setupAllObjects()
	{
		FK_SceneWithInput::setupIrrlichtDevice();
		FK_SceneWithInput::setupJoypad();
		FK_SceneWithInput::setupInputMapper();
		setupSoundManager();
		setupStage();
		setupCharacter();
		setupCamera();
		setupLights();
		setupCharacterShader();
		setupMenu();
		setupTextWindow();
	}

	void FK_SceneDiorama::dispose() {
		// still have to find a good idea on how to manage this.
		//smgr->drop();
		stage_bgm.stop();
		//
		delete character;
		delete stage;
		delete player1input;
		delete player2input;
		delete inputMapper;
		delete characterCollisionCallback;
		delete soundManager;
		delete scenePixelShaderParameters;
		delete dioramaMenu;
		delete characterTextWindow;
		delete stageTextWindow;
		//delete camera;
		// clear the scene of all scene nodes
		smgr->clear();
		// remove all meshes from the mesh cache
		scene::IMeshCache* cache = smgr->getMeshCache();
		while (cache->getMeshCount())
			cache->removeMesh(cache->getMeshByIndex(0));
		// remove all textures
		for each(auto callback in scenePixelShaderCallback) {
			delete callback.second;
		}
		for each(auto callback in stagePixelShaderCallback) {
			delete callback;
		}
		smgr->getVideoDriver()->removeAllTextures();
	};

	/* setup resources */
	void FK_SceneDiorama::setupResourcesPaths(std::string new_player1path, std::string new_stagePath) {
		character_configPath = new_player1path;
		stage_configPath = new_stagePath;
		stagesPath = mediaPath + "stages\\";
		charactersPath = mediaPath + "characters\\";
		commonResourcesPath = mediaPath + "common\\";
		voiceClipsPath = mediaPath + "voice_clips\\";
	}

	bool FK_SceneDiorama::isValidCharacterPath(std::string path)
	{
		std::ifstream testFile(path + "character.txt");
		if (!testFile) {
			return false;
		}
		return true;
	}

	bool FK_SceneDiorama::isValidStagePath(std::string path)
	{
		std::ifstream testFile(path + "config.txt");
		if (!testFile) {
			return false;
		}
		return true;
	}

	/* check if scene is running */
	bool FK_SceneDiorama::isRunning() {
		return (!backToCharacterSelection || (backToCharacterSelection && introCounter >= 0));
	}

	/* setup */
	void FK_SceneDiorama::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions,
		std::string new_characterFilename, FK_Character::FK_CharacterOutfit new_characterOutfit,
		std::string new_stagePath) {
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupResourcesPaths(new_characterFilename, new_stagePath);
		characterOutfit = new_characterOutfit;
		loadDioramaInformation();
		setupAllObjects();
	}

	void FK_SceneDiorama::loadDioramaInformation() {
		std::string path = charactersPath + character_configPath;
		if (!isValidCharacterPath(path) && isValidCharacterPath(character_configPath)) {
			path = character_configPath;
		}
		std::string configFile = "diorama.txt";
		std::ifstream configurationFile((path + characterOutfit.outfitDirectory + configFile).c_str());
		if (!configurationFile) {
			configurationFile.clear();
			configurationFile = std::ifstream((path + configFile).c_str());
			if (!configurationFile) {
				stage_configPath = "TheWalkingNight\\";
				return;
			}
		}
		std::string stageIdentifier = "#stage";
		std::string defaultCameraIdentifier = "#default_camera";
		std::string characterCameraIdentifier = "#character_camera";
		std::string poseIdentifier = "#pose";
		std::string toggleObjectIdentifier = "#toggle_object";
		std::string textLinesIdentifier = "#profile";
		std::string textLinesIdentifierEnd = "#profile_end";
		std::string characterInfoIdentifier = "#character_info";
		std::string characterInfoIdentifierEnd = "#character_info_end";
		std::string temp;
		characterProfileLines.clear();
		while (configurationFile >> temp) {
			if (temp == stageIdentifier) {
				configurationFile >> stage_configPath;
			}
			else if (temp == defaultCameraIdentifier) {
				f32 distance, angle, offset, targetOffset;
				configurationFile >> distance >> angle >> offset >> targetOffset;
				defaultCamera.cameraDistance = distance;
				defaultCamera.cameraAngle = angle;
				defaultCamera.cameraOffset = offset;
				defaultCamera.cameraTargetOffset = targetOffset;
			}
			else if (temp == characterCameraIdentifier) {
				f32 distance, angle, offset, targetOffset;
				configurationFile >> distance >> angle >> offset >> targetOffset;
				characterCamera.cameraDistance = distance;
				characterCamera.cameraAngle = angle;
				characterCamera.cameraOffset = offset;
				characterCamera.cameraTargetOffset = targetOffset;
			}
			else if (temp == poseIdentifier) {
				std::string animationName;
				s32 frameStart, frameStop;
				configurationFile >> animationName >> frameStart >> frameStop;
				characterPose.setAnimationName(animationName);
				characterPose.setStartingFrame(frameStart);
				characterPose.setEndingFrame(frameStop);
				characterPose.setLoop(true);
			}
			else if (temp == toggleObjectIdentifier) {
				configurationFile >> temp;
				characterObjectsToToggle.push_back(temp);
			}
			else if (temp == textLinesIdentifier) {
				while (configurationFile) {
					std::getline(configurationFile, temp);
					if (temp == textLinesIdentifierEnd) {
						break;
					}
					if (!(temp.empty()) && temp != " ") {
						characterProfileLines.push_back(temp);
					}
				}
			}
			else if (temp == characterInfoIdentifier) {
				std::vector<std::string> characterData;
				dioramaCharacterInfo = FK_DioramaCharacterInfo();
				while (configurationFile) {
					std::getline(configurationFile, temp);
					if (temp == characterInfoIdentifierEnd) {
						break;
					}
					if (!(temp.empty()) && temp != " ") {
						characterData.push_back(temp);
					}
				}
				for (u32 i = characterData.size(); i < 4; ++i) {
					characterData.push_back(std::string());
				}
				for (u32 i = 4; i < characterData.size(); ++i) {
					dioramaCharacterInfo.signatureMoves.push_back(fk_addons::convertNameToNonASCIIWstring(characterData[i]));
				}
				//dioramaCharacterInfo.nickname = fk_addons::convertNameToNonASCIIWstring(characterData[0]);
				dioramaCharacterInfo.age = fk_addons::convertNameToNonASCIIWstring(characterData[0]);
				dioramaCharacterInfo.nationality = fk_addons::convertNameToNonASCIIWstring(characterData[1]);
				dioramaCharacterInfo.height = fk_addons::convertNameToNonASCIIWstring(characterData[2]);
				dioramaCharacterInfo.weight = fk_addons::convertNameToNonASCIIWstring(characterData[3]);
			}
		}
		configurationFile.close();
		configurationFile.clear();
	}

	/* setup stage */
	void FK_SceneDiorama::setupStage() {
		std::string newStagePath = stagesPath + stage_configPath;
		if (!isValidStagePath(newStagePath) && isValidStagePath(stage_configPath)) {
			newStagePath = stage_configPath;
		}
		//std::string newStagePath = stagesPath + "Elevator\\";
		stage = new FK_Stage();
		stage->setup(FK_SceneDiorama::device, driver, smgr, newStagePath);
		//m_threadMutex.unlock();
		/* play BGM!!!! */
		std::string filePath = newStagePath + stage->getBGMName();
		if (!stage_bgm.openFromFile(filePath.c_str()))
			return;
		stage_bgm.setLoop(true);
		stage_bgm.setVolume(stage->getBGMVolume() * gameOptions->getMusicVolume());
		stage_bgm.setPitch(stage->getBGMPitch());
		// set fog
		stage->setFog();
		stage->disallowRingout();
		stageInformationLines = stage->getDioramaLoreText();
	}

	/* setup character */
	/* setup the characters */
	void FK_SceneDiorama::setupCharacter() {
		std::string path = charactersPath + character_configPath;
		if (!isValidCharacterPath(path) && isValidCharacterPath(character_configPath)) {
			path = character_configPath;
		}
		/* load player 1 */
		//m_threadMutex.lock();
		/*core::vector3df startingPosition = player1startingPosition;
		startingPosition.Y += (player1->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;*/
		character = new FK_Character();
		character->setup(databaseAccessor,
			"character.txt", path,
			commonResourcesPath,
			smgr, core::vector3df(50, 0, 0),
			core::vector3df(0, 0, 0),
			characterOutfit.outfitId);
		//m_threadMutex.unlock();
		//player1->getAnimatedMesh()->setDebugDataVisible(E_DEBUG_SCENE_TYPE::EDS_BBOX);
		character->toggleHitboxVisibility(false);
		character->toggleHurtboxVisibility(false);
		character->toggleLighting(false);
		character->setRotation(core::vector3df(0, 0, 0));
		setupCharacterStageInteraction();
		character->getAnimatedMesh()->OnRegisterSceneNode();
		if (!characterPose.getAnimationName().empty()) {
			character->replaceBasicAnimation(FK_BasicPose_Type::IdleAnimation,
				characterPose.getAnimationName(), characterPose.getStartingFrame(),
				characterPose.getEndingFrame(), characterPose.isLooping());
			character->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
		}
		for each(std::string objectId in characterObjectsToToggle) {
			if (character->getObjectByName(objectId) != NULL) {
				character->getObjectByName(objectId)->toggleVisibility(
					!character->getObjectByName(objectId)->defaultVisibility);
			}
		}
	};

	// setup collisions
	void FK_SceneDiorama::setupCharacterStageInteraction() {
		/* create selector to manage collisions with the world */
		/* create stage collider */
		scene::ITriangleSelector* selector = 0;// stage->getStage()->getTriangleSelector();
		if (stage->getStage())
		{
			selector = smgr->createOctreeTriangleSelector(
				stage->getStageMesh()->getMesh(0), stage->getStage(), 128);
			stage->getStage()->setTriangleSelector(selector);
		}
		if (stage->getSkybox() != NULL) {
			scene::ITriangleSelector* skyboxSelector = smgr->createTriangleSelectorFromBoundingBox(stage->getSkybox());
			stage->getSkybox()->setTriangleSelector(skyboxSelector);
			skyboxSelector->drop();
		}
		/* prevent the creation of more than one collider*/
		if (characterCollisionCallback == 0) {
			characterCollisionCallback = new FK_WorldCollisionCallback(character);
		}

		if (selector)
		{
			core::vector3df ellipse_radius = core::vector3df(40, 44.5, 40);
			ellipse_radius.Y *= character->getAnimatedMesh()->getScale().Y;

			scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
				selector, character->getAnimatedMesh(), ellipse_radius,
				core::vector3df(0, -550.f, 0), core::vector3df(0, 0, 0));

			anim->setCollisionCallback(characterCollisionCallback);
			character->getAnimatedMesh()->addAnimator(anim);
		};
	};

	/* setup post-processing effects and lights */
	void FK_SceneDiorama::setupLights() {
		core::dimension2du renderSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			renderSize = borderlessWindowRenderTarget->getSize();
		}
		lightManager = new EffectHandler(device, renderSize, false, true, false);
		/*set filter for shadows*/
		shadowFilterType = (E_FILTER_TYPE)core::clamp<u32>(3, 0, 4);

		FK_Options::FK_PostProcessingEffect postProcessing = gameOptions->getPostProcessingShadersFlag();

		if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::FullShadows) {
			shadowModeCharacters = E_SHADOW_MODE::ESM_BOTH;
			shadowModeStage = E_SHADOW_MODE::ESM_BOTH;
		}
		else if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::OnlyCharaShadows) {
			shadowModeCharacters = E_SHADOW_MODE::ESM_CAST;
			shadowModeStage = E_SHADOW_MODE::ESM_RECEIVE;
		}
		else if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::NoShadowEffect) {
			shadowModeCharacters = E_SHADOW_MODE::ESM_EXCLUDE;
			shadowModeStage = E_SHADOW_MODE::ESM_EXCLUDE;
		}
		/* set shadow detail level */
		u32 shadowDimen = gameOptions->getShadowQuality();
		/* add shadow to arena after disabling standard lighting - UPDATE: not needed, if one wants emission effects */
		if (gameOptions->getLightEffectsActiveFlag()) {
			lightManager->addShadowToNode(stage->getStage(), shadowFilterType, shadowModeStage);
		}
		// add shadow to water node
		if (stage->getStageWaterNode() != NULL) {
			stage->getStageWaterNode()->setMaterialFlag(EMF_LIGHTING, false);
			if (gameOptions->getLightEffectsActiveFlag()) {
				lightManager->addShadowToNode(stage->getStageWaterNode(), shadowFilterType, shadowModeStage);
			}
		}
		if (!stage->getAdditionalNodes().empty()) {
			int size = stage->getAdditionalNodes().size();
			for (int i = 0; i < size; ++i) {
				stage->getAdditionalNodes()[i]->setMaterialFlag(EMF_LIGHTING, false);
				if (gameOptions->getLightEffectsActiveFlag()) {
					lightManager->addShadowToNode(stage->getAdditionalNodes()[i], shadowFilterType, shadowModeStage);
				}
			}
		}
		lightManager->addShadowToNode(character->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
		for (int i = 0; i < character->getCharacterAdditionalObjects().size(); ++i) {
			if (character->getCharacterAdditionalObjects().at(i).applyShadow &&
				character->getCharacterAdditionalObjects().at(i).isActive()) {
				lightManager->addShadowToNode(character->getCharacterAdditionalObjectsNodes().at(i),
					shadowFilterType, shadowModeCharacters);
			}
		}
		lightManager->setClearColour(stage->getBackgroundColor());
		lightManager->setAmbientColor(stage->getAmbientColor());
		/* add post processing effects */
		const core::stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

		if (postProcessing == FK_Options::FK_PostProcessingEffect::AllEffects) {
			s32 brightpassMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BrightPass2") + shaderExt);
			scenePixelShaderCallback["brightpass"] = new FK_PixelShaderSceneGameCallback(brightpassMaterialId, this,
				FK_SceneShaderTypes::ShaderBrightPass);
			lightManager->setPostProcessingRenderCallback(brightpassMaterialId, scenePixelShaderCallback["brightpass"]);

			lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BlurHP") + shaderExt);
			lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BlurVP") + shaderExt);

			s32 bloomMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BloomP") + shaderExt);
			scenePixelShaderCallback["bloom"] = new FK_PixelShaderSceneGameCallback(bloomMaterialId, this,
				FK_SceneShaderTypes::ShaderBloom);
			lightManager->setPostProcessingRenderCallback(bloomMaterialId, scenePixelShaderCallback["bloom"]);
			/*lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
			core::stringc("shaders/edgeDetect") + shaderExt);*/
		}
		else if (postProcessing == FK_Options::FK_PostProcessingEffect::BloomOnly) {
			s32 brightpassMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BrightPass2") + shaderExt);
			scenePixelShaderCallback["brightpass"] = new FK_PixelShaderSceneGameCallback(brightpassMaterialId, this,
				FK_SceneShaderTypes::ShaderBrightPass);
			lightManager->setPostProcessingRenderCallback(brightpassMaterialId, scenePixelShaderCallback["brightpass"]);
			s32 bloomMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BloomP") + shaderExt);
			scenePixelShaderCallback["bloom"] = new FK_PixelShaderSceneGameCallback(bloomMaterialId, this,
				FK_SceneShaderTypes::ShaderBloom);
			lightManager->setPostProcessingRenderCallback(bloomMaterialId, scenePixelShaderCallback["bloom"]);
		}
		else {
			// increase brightness in ambient colour
			video::SColorHSL tempColor;
			tempColor.fromRGB(stage->getAmbientColor());
			tempColor.Luminance = 1.1 * tempColor.Luminance;
			tempColor.Saturation = 1.1 * tempColor.Saturation;
			video::SColorf tempColorRGBf;
			tempColor.toRGB(tempColorRGBf);
			video::SColor tempColorRGB = tempColorRGBf.toSColor();
			lightManager->setAmbientColor(tempColorRGB);
		}
		// add stage shaders
		/*for each(FK_Stage::FK_StagePixelShader shader in stage->getAdditionalPixelShaders()) {
			if (!shader.parameters.empty()) {
				std::string path = stagesPath + stage_configPath + shader.filename;
				s32 materialId = lightManager->addPostProcessingEffectFromFile(core::stringc(path.c_str()) + shaderExt);
				FK_PixelShaderStageCallback* stageCallback = new FK_PixelShaderStageCallback(materialId, this, shader);
				stagePixelShaderCallback.push_back(stageCallback);
				lightManager->setPostProcessingRenderCallback(materialId, stagePixelShaderCallback[
					stagePixelShaderCallback.size() - 1]);
			}
		}*/
		/* create lights*/
		lightManager->addShadowLight(SShadowLight(shadowDimen, core::vector3df(0, 90, 0), core::vector3df(0, 0, 0),
			stage->getLightColor(), 1.0f, 240.0f, 120.0f * irr::core::DEGTORAD));
		lightManager->enableDepthPass(true);
		lightManager->addNodeToDepthPass(character->getAnimatedMesh());
		lightManager->addNodeToDepthPass(character->getHitParticleEffect());
		lightManager->addNodeToDepthPass(character->getGuardParticleEffect());
		lightManager->addNodeToDepthPass(stage->getStage());
		if (stage->getRainEmitter() != NULL) {
			lightManager->addNodeToDepthPass(stage->getRainEmitter());
		}
		if (stage->getStageWaterNode() != NULL) {
			lightManager->addNodeToDepthPass(stage->getStageWaterNode());
		}
		for (int i = 0; i < stage->getAdditionalNodes().size(); ++i) {
			lightManager->addNodeToDepthPass(stage->getAdditionalNodes()[i]);
		}

		// set light position
		core::vector3df position = character->getPosition();
		lightManager->getShadowLight(0).setTarget(position);
		position.Y = lightManager->getShadowLight(0).getPosition().Y;
		lightManager->getShadowLight(0).setPosition(position);
	}

	// show character and character shadow
	void FK_SceneDiorama::showCharacter() {
		character->getAnimatedMesh()->setVisible(true);
		lightManager->addShadowToNode(character->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
		for (int i = 0; i < character->getCharacterAdditionalObjects().size(); ++i) {
			if (character->getCharacterAdditionalObjects().at(i).applyShadow &&
				character->getCharacterAdditionalObjects().at(i).isActive()) {
				lightManager->addShadowToNode(character->getCharacterAdditionalObjectsNodes().at(i),
					shadowFilterType, shadowModeCharacters);
			}
		}
		character->getAnimatedMesh()->SetInternalTimerScalingFactor(1.f);
	}


	// hide character & character shadow
	void FK_SceneDiorama::hideCharacter() {
		character->getAnimatedMesh()->setVisible(false);
		lightManager->removeShadowFromNode(character->getAnimatedMesh());
		for (int i = 0; i < character->getCharacterAdditionalObjects().size(); ++i) {
			if (character->getCharacterAdditionalObjects().at(i).applyShadow &&
				character->getCharacterAdditionalObjects().at(i).isActive()) {
				lightManager->removeShadowFromNode(character->getCharacterAdditionalObjectsNodes().at(i));
			}
		}
		character->getAnimatedMesh()->SetInternalTimerScalingFactor(0.f);
	}

	/* setup sound manager*/
	void FK_SceneDiorama::setupSoundManager(){
		soundManager = new FK_SoundManager(commonResourcesPath + "sound_effects\\");
		soundManager->addSoundFromDefaultPath("trigger", "trigger.ogg");
		soundManager->addSoundFromDefaultPath("guard", "guard03.wav");
		soundManager->addSoundFromDefaultPath("hit_weak", "hitA03.wav");
		soundManager->addSoundFromDefaultPath("hit_strong", "hitF02.wav");
		soundManager->addSoundFromDefaultPath("wry", "wry.ogg");
		soundManager->addSoundFromDefaultPath("swing", "swing.ogg");
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
		soundManager->addSoundFromDefaultPath("splash", "splash.ogg");
	}

	/* setup camera*/
	void FK_SceneDiorama::setupCamera(){
		/* define base camera position*/
		float baseCameraDistance = 290.0f;
		float baseCameraYPosition = -23.0f;
		float cameraDistance = baseCameraDistance;// + 50.0f; //take into account the X position of the models!!!
		float cameraYPosition = baseCameraYPosition;
		/* initialize the camera */
		//m_threadMutex.lock();
		camera = smgr->addCameraSceneNode(0, core::vector3df(cameraDistance, cameraYPosition, 0), core::vector3df(0, 0, 0));
		camera->setAutomaticCulling(E_CULLING_TYPE::EAC_BOX);
		camera->setFarValue(20000.0f);
		camera->setNearValue(10.0f);
		camera->setAspectRatio(16.f / 9);
		f32 defaultFOVAngle = core::PI / 2.5;
		camera->setFOV(defaultFOVAngle);
		resetCameraPosition();
	}

	/* reset camera position */
	void FK_SceneDiorama::resetCameraPosition() {
		setCameraPosition(defaultCamera.cameraDistance, 
			defaultCamera.cameraAngle, 
			defaultCamera.cameraOffset,
			defaultCamera.cameraTargetOffset);
	}

	/* reset camera position */
	void FK_SceneDiorama::setCameraPosition(f32 distance, f32 new_angle, f32 offsetY, f32 targetOffsetY) {
		core::vector3df cameraPosition = character->getPosition();
		core::vector3df playerDirection = character->getRotation();
		f32 cameraDistanceFromPlayer = distance;
		f32 offsetAngleForVictoryCamera = new_angle;
		f32 angle = playerDirection.Y + offsetAngleForVictoryCamera;
		f32 cameraOffsetYPositionFromWinner = offsetY;
		cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Y += cameraOffsetYPositionFromWinner;
		//cameraPosition.Z 
		camera->setPosition(cameraPosition);
		core::vector3d<f32> targetPosition = character->getPosition();
		targetPosition.Y += targetOffsetY;
		camera->setTarget(targetPosition);
		oldAngle = new_angle;
		oldDistance = distance;
		oldOffsetY = offsetY;
		oldTargetOffsetY = targetOffsetY;
	}

	/* setup character shaders*/
	void FK_SceneDiorama::setupCharacterShader() {
		// create a shader callback
		FK_CharacterMaterialShaderCallback* shaderCallback = new FK_CharacterMaterialShaderCallback(device, lightManager,
			character);
		// create materials
		s32 characterMeshMaterial = 0;
		video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
		const core::stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";
		const core::stringc vsFileName = core::stringc(applicationPath.c_str()) +
			core::stringc("shaders/charaVxShader") + shaderExt;
		const core::stringc psFileName = core::stringc(applicationPath.c_str()) +
			core::stringc("shaders/charaVxShader") + shaderExt;

		const video::E_GPU_SHADING_LANGUAGE shadingLanguage = video::EGSL_DEFAULT;

		// create material from high level shaders (hlsl, glsl or cg)

		characterMeshMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain",
			driver->queryFeature(video::EVDF_VERTEX_SHADER_3_0) ? EVST_VS_3_0 : EVST_VS_2_0,
			psFileName, "pixelMain",
			driver->queryFeature(video::EVDF_PIXEL_SHADER_3_0) ? EPST_PS_3_0 : EPST_PS_2_0,
			shaderCallback, video::EMT_TRANSPARENT_ALPHA_CHANNEL, 0, shadingLanguage);
		// add material to buffer
		characterMeshMaterials.push_back(characterMeshMaterial);
		lightManager->addCustomMaterialToBuffer(characterMeshMaterial, video::EMT_TRANSPARENT_ALPHA_CHANNEL);

		characterMeshMaterial = characterMeshMaterials[characterMeshMaterials.size() - 1];

		// attach shader to player mesh
		//player1->getAnimatedMesh()->setMaterialType((video::E_MATERIAL_TYPE)characterMeshMaterial);
		for (int i = 0; i < character->getAnimatedMesh()->getMaterialCount(); ++i) {
			SMaterial mat = character->getAnimatedMesh()->getMaterial(i);
			if (mat.TextureLayer[0].Texture != NULL) {
				character->getAnimatedMesh()->getMaterial(i).MaterialType = (video::E_MATERIAL_TYPE)characterMeshMaterial;
			}
		}
		// add shader to additional objects
		for (int i = 0; i < character->getCharacterAdditionalObjects().size(); ++i) {
			if (!character->getCharacterAdditionalObjects().at(i).hasDummyMesh()) {
				for (int k = 0; k < character->getCharacterAdditionalObjectsNodes().at(i)->getMaterialCount(); ++k) {
					SMaterial mat = character->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k);
					if (mat.TextureLayer[0].Texture != NULL) {
						character->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k).MaterialType =
							(video::E_MATERIAL_TYPE)characterMeshMaterial;
					}
				}
			}
		}
		// drop shader callbacks
		shaderCallback->drop();
	}

	/* update input */
	void FK_SceneDiorama::updateInput(u32 delta_t_ms) {
		u32 inputButtons = 0;
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		u32 inputButtonsWDirections = inputButtons;
		inputButtons &= ~FK_Input_Buttons::Any_Direction;
		inputButtons &= FK_Input_Buttons::Any_MenuButton;
		if (dioramaMode == FK_DioramaMode::DioramaMenu) {
			dioramaMenu->update(inputButtonsWDirections, false);
			inputButtonsWDirections = 0;
			if (dioramaMenu->itemIsSelected()) {
				dioramaMenu->resetSelection();
				s32 itemIndex = dioramaMenu->getIndex();
				soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				switch (itemIndex) {
				case 0:
					activateProfileView();
					break;
				case 1:
					activateStageView();
					break;
				case 2:
					dioramaMode = FK_DioramaMode::DioramaFreeCamera;
					break;
				case 3:
					backToCharacterSelection = true;
					setNextScene(getPreviousScene());
					break;
					return;
				}
				dioramaMenu->setVisible(false);
				dioramaMenu->setActive(false);
			}
			inputReceiver->reset();
		}else{
			// skip frame
			if ((inputButtons & FK_Input_Buttons::Cancel_Button) != 0 ||
				(inputButtons & FK_Input_Buttons::Menu_Pause_Button) != 0){
				soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				deactivateTextWindow();
				dioramaMode = FK_DioramaMode::DioramaMenu;
				resetCameraPosition();
				inputReceiver->reset();
				dioramaMenu->setVisible(true);
				dioramaMenu->setActive(true);
				if (!character->getAnimatedMesh()->isVisible()) {
					showCharacter();
				}
				return;
			}
			else if ((inputButtons & FK_Input_Buttons::Modifier_Button) != 0) {
				if (dioramaMode == FK_DioramaMode::DioramaFreeCamera) {
					soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					resetCameraPosition();
					inputReceiver->reset();
					return;
				}
			}
		}
		if (dioramaMode == FK_DioramaMode::DioramaFreeCamera) {
			updateCameraPosition(inputButtonsWDirections, delta_t_ms);
		}else if (dioramaMode == FK_DioramaMode::DioramaStage){
			updateStagePanoramicCamera(delta_t_ms);
			stageTextWindow->update(inputButtonsWDirections, delta_t_ms);
		}else if(dioramaMode == FK_DioramaMode::DioramaProfile){
			updateCameraPosition(0, delta_t_ms);
			characterTextWindow->update(inputButtonsWDirections, delta_t_ms);
		}
		else {
			updateCameraPosition(0, delta_t_ms);
			characterTextWindow->update(0, delta_t_ms);
			stageTextWindow->update(0, delta_t_ms);
		}
	}

	/* update camera position */
	void FK_SceneDiorama::updateCameraPosition(u32 pressedButtons, u32 delta_t_ms) {
		f32 delta_t_s = (f32)delta_t_ms / 1000.f;
		u32 directions = pressedButtons & FK_Input_Buttons::Any_Direction;
		u32 menuButtons = pressedButtons & FK_Input_Buttons::Any_MenuButton;
		f32 newDistance = oldDistance;
		f32 newAngle = oldAngle;
		f32 newOffsetY = oldOffsetY;
		if ((directions & FK_Input_Buttons::Right_Direction) != 0) {
			newAngle -= DeltaCameraAnglePerSecond * delta_t_s;
		}
		else if ((directions & FK_Input_Buttons::Left_Direction) != 0) {
			newAngle += DeltaCameraAnglePerSecond * delta_t_s;
		}
		if ((directions & FK_Input_Buttons::Up_Direction) != 0) {
			newOffsetY += DeltaCameraOffsetYPerSecond * delta_t_s;
			newOffsetY = core::clamp(newOffsetY, MinimalYOffset, MaximalYOffset);
		}
		else if ((directions & FK_Input_Buttons::Down_Direction) != 0) {
			newOffsetY -= DeltaCameraOffsetYPerSecond * delta_t_s;
			newOffsetY = core::clamp(newOffsetY, MinimalYOffset, MaximalYOffset);
		}
		if ((menuButtons & FK_Input_Buttons::ScrollLeft_Button) != 0) {
			newDistance += DeltaCameraDistancePerSecond * delta_t_s;
			newDistance = core::clamp(newDistance, MinimalCameraDistance, MaximalCameraDistance);
		}
		else if ((menuButtons & FK_Input_Buttons::ScrollRight_Button) != 0) {
			newDistance -= DeltaCameraDistancePerSecond * delta_t_s;
			newDistance = core::clamp(newDistance, MinimalCameraDistance, MaximalCameraDistance);
		}
		setCameraPosition(newDistance, newAngle, newOffsetY, oldTargetOffsetY);
	}

	/* update stage panoramic camera */
	void FK_SceneDiorama::updateStagePanoramicCamera(u32 delta_t_ms) {
		f32 delta_t_s = (f32)delta_t_ms / 1000.f;
		f32 new_angle = oldAngle + StageCameraDeltaCameraAnglePerSecond * delta_t_s;
		core::vector3df cameraPosition = character->getPosition();
		core::vector3df targetPosition = cameraPosition;
		f32 cameraDistanceFromTarget = stageCamera.cameraDistance;
		f32 angle = new_angle;
		f32 cameraOffsetYPositionFromWinner = stageCamera.cameraOffset;
		targetPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromTarget;
		targetPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromTarget;
		targetPosition.Y += stageCamera.cameraTargetOffset;
		cameraPosition.Y += cameraOffsetYPositionFromWinner;
		//cameraPosition.Z 
		camera->setPosition(cameraPosition);
		camera->setTarget(targetPosition);
		oldAngle = new_angle;
	}

	/* update */
	void FK_SceneDiorama::update() {
		now = device->getTimer()->getTime();
		u32 delta_t_ms = now - then;
		then = now;
		f32 delta_t_s = (f32)delta_t_ms / 1000.f;
		if (cycleId == 0) {
			cycleId = 1;
			delta_t_ms = 0;
			stage_bgm.play();
		}
		else {
			if (introCounter < IntroDurationMs && !backToCharacterSelection) {
				introCounter += (s32)delta_t_ms;
				introCounter = (s32)core::clamp(introCounter, 0, (s32)IntroDurationMs);
			}
			if (backToCharacterSelection && introCounter >= 0) {
				introCounter -= (s32)delta_t_ms;
			}
			if (introCounter >= (s32)IntroDurationMs) {
				updateInput(delta_t_ms);
			}else{
				updateCameraPosition(0, delta_t_ms);
			}
		}
		bool fakeMoveBool;
		character->update(0, delta_t_s, fakeMoveBool);
		stage->update(delta_t_ms);
		// update ambient and light color (if needed)
		lightManager->setAmbientColor(stage->getAmbientColor());
		lightManager->getShadowLight(0).setLightColor(stage->getLightColor());
		draw();
	}

	/* setup menu */
	void FK_SceneDiorama::setupMenu() {
		// setup screen size
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = (f32)screenSize.Width / fk_constants::FK_DefaultResolution.Width;
		scale_factorY = (f32)screenSize.Height / fk_constants::FK_DefaultResolution.Height;
		// setup menu
		std::vector<std::string> menuItems = {
			"Character profile",
			"Stage information",
			"Free Camera",
			"Back to Selection",
		};
		dioramaMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		s32 x = (s32)std::round(24 * scale_factorX);
		s32 y = (s32)std::round(screenSize.Height - dioramaMenu->getFrame().getHeight() - 24 * scale_factorY);
		if (gameOptions->getBorderlessWindowMode()) {
			dioramaMenu->setViewport(core::dimension2di(-1, -1), borderlessWindowRenderTarget->getSize());
		}
		dioramaMenu->setPosition(x, y);
		dioramaMenu->reset();
		dioramaMenu->setActive(true);
		dioramaMenu->setVisible(true);
	}

	/* setup window */
	void FK_SceneDiorama::setupTextWindow() {
		core::rect<s32> frameBottom(50, 270, 590, 350);
		//core::rect<s32> frameSide(10, 10, 240, 350);
		characterTextWindow = new FK_ScrollTextWindow(device, frameBottom, "windowskin.png",
			commonResourcesPath, scale_factorX, characterProfileLines);
		stageTextWindow = new FK_ScrollTextWindow(device, frameBottom, "windowskin.png",
			commonResourcesPath, scale_factorX, stageInformationLines);
		deactivateTextWindow();
	}

	/*draw character information */
	void FK_SceneDiorama::drawCharacterInformation() {
		gui::IGUIFont* textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		gui::IGUIFont* mainFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		s32 height = (s32) mainFont->getDimension(L"A").Height;
		s32 heightText = (s32)textFont->getDimension(L"A").Height;
		s32 x11 = (s32)std::floor(10 * scale_factorX);
		s32 y1 = (s32)std::floor(10 * scale_factorY);
		s32 x21 = (s32)std::floor(100 * scale_factorX);
		
		std::wstring characterName = fk_addons::convertNameToNonASCIIWstring(character->getName());
		std::vector<std::wstring> itemDescriptions{
			characterName,
			L"Age:",
			L"From:",
			L"Height:",
			L"Weight:",
			std::wstring(),
			L"Signature moves:",
		};
		std::vector<std::wstring> itemValue = {
			std::wstring(),
			dioramaCharacterInfo.age,
			dioramaCharacterInfo.nationality,
			dioramaCharacterInfo.height,
			dioramaCharacterInfo.weight,
			std::wstring(),
			std::wstring(),
		};

		for (u32 i = 0; i < itemDescriptions.size(); ++i) {
			s32 y11 = y1 + height * i;
			s32 sentenceWidth = (s32)mainFont->getDimension(itemDescriptions[i].c_str()).Width;
			core::rect<s32> destRect(x11, y11, x11 + sentenceWidth, y11 + height);
			fk_addons::drawBorderedText(
				mainFont, itemDescriptions[i].c_str(), destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
			sentenceWidth = (s32)textFont->getDimension(itemValue[i].c_str()).Width;
			y11 = y11 + (height - heightText);
			destRect = core::rect<s32>(x21, y11, x21 + sentenceWidth, y11 + heightText);
			fk_addons::drawBorderedText(
				textFont, itemValue[i].c_str(), destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
		}

		for (u32 i = 0; i < dioramaCharacterInfo.signatureMoves.size(); ++i) {
			s32 y11 = y1 + height * (i + itemDescriptions.size()) + (height - heightText);
			s32 sentenceWidth = (s32)textFont->getDimension(dioramaCharacterInfo.signatureMoves[i].c_str()).Width;
			core::rect<s32> destRect(x11, y11, x11 + sentenceWidth, y11 + height);
			fk_addons::drawBorderedText(
				textFont, dioramaCharacterInfo.signatureMoves[i].c_str(), destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
		}
	}

	/* draw */
	void FK_SceneDiorama::draw() {
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		lightManager->update(gameOptions->getBorderlessWindowMode() ? borderlessWindowRenderTarget : 0);
		driver->setRenderTarget(gameOptions->getBorderlessWindowMode() ? borderlessWindowRenderTarget : 0, false, false);
		if (dioramaMenu->isVisible()) {
			dioramaMenu->draw();
		}
		if (characterTextWindow->isVisible()) {
			if ((dioramaMode == FK_DioramaMode::DioramaProfile && !characterProfileLines.empty())) {
				characterTextWindow->draw(screenSize, scale_factorX, scale_factorY, true);
			}
			drawCharacterInformation();
		}
		if (stageTextWindow->isVisible()) {
			if ((dioramaMode == FK_DioramaMode::DioramaStage && !stageInformationLines.empty())) {
				stageTextWindow->draw(screenSize, scale_factorX, scale_factorY, true);
			}
		}
		if (introCounter < (s32)IntroDurationMs || backToCharacterSelection) {
			f32 ratio = (f32)introCounter / IntroDurationMs;
			s32 alpha = (s32)(1 + 254 * (1.f - ratio));
			if (alpha < 1) {
				alpha = 1;
			}else if(alpha >= 255){
				alpha = 255;
			}
			video::SColor color(alpha,
				0, 0, 0);
			driver->draw2DRectangle(color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	/* a boolean which returns shader parameters */
	f32* FK_SceneDiorama::getSceneParametersForShader(FK_SceneShaderTypes newShader) {
		if (newShader == FK_SceneShaderTypes::ShaderBloom) {
			scenePixelShaderParameters[0] = stage->getParametersForShader().bloomValue;
			scenePixelShaderParameters[1] = stage->getParametersForShader().sceneValue;
			scenePixelShaderParameters[2] = 0.f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderBrightPass) {
			scenePixelShaderParameters[0] = stage->getParametersForShader().brightnessCutHigh;
			scenePixelShaderParameters[1] = stage->getParametersForShader().brightnessCutLow;
			scenePixelShaderParameters[2] = 0.f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderRipple) {
			scenePixelShaderParameters[0] = 0.0f;
			scenePixelShaderParameters[1] = 0.5f;
			scenePixelShaderParameters[2] = 0.5f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderZoomBlur) {
			scenePixelShaderParameters[0] = 0.0f;
			scenePixelShaderParameters[1] = -0.2f;
			scenePixelShaderParameters[2] = 0.75f;
			scenePixelShaderParameters[3] = 0.f;
		}
		return scenePixelShaderParameters;
	}

	void FK_SceneDiorama::activateProfileView() {
		dioramaMode = FK_DioramaMode::DioramaProfile;
		setCameraPosition(characterCamera.cameraDistance,
			characterCamera.cameraAngle,
			characterCamera.cameraOffset,
			characterCamera.cameraTargetOffset);
		characterTextWindow->resetTextPosition();
		characterTextWindow->forceOpen();
		characterTextWindow->setVisibility(true);
		characterTextWindow->setActive(true);
	}

	void FK_SceneDiorama::activateStageView() {
		dioramaMode = FK_DioramaMode::DioramaStage;
		oldAngle = stageCamera.cameraAngle;
		hideCharacter();
		stageTextWindow->resetTextPosition();
		stageTextWindow->forceOpen();
		stageTextWindow->setVisibility(true);
		stageTextWindow->setActive(true);
	}

	void FK_SceneDiorama::deactivateTextWindow() {
		characterTextWindow->setVisibility(false);
		characterTextWindow->setActive(false);
		stageTextWindow->setVisibility(false);
		stageTextWindow->setActive(false);
	}
}