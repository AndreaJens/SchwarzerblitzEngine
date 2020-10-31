#include "FK_Stage.h"
#include "FK_Database.h"
#include<fstream>
#include<sstream>
#include<algorithm>
#include<iostream>
#include"ExternalAddons.h"

using namespace irr;
using namespace scene;
using namespace video;

namespace fk_engine{
	FK_Stage::FK_Stage(){
		isZipped = false;
		hasSkybox = false;
		archiveName = std::string();
		name = std::string();
		path = std::string();
		meshName = std::string();
		position = core::vector3df(0, 0, 0);
		scale = core::vector3df(1.0f, 1.0f, 1.0f);
		description = std::string();
		backgroundColor = video::SColor(255, 0, 0, 0);
		ambientColor = video::SColor(255, 255, 255, 255);
		emissionColor = video::SColor(255, 255, 255, 255);
		lightColor = video::SColor(255, 255, 255, 255);
		basicBGMVolume = 75.0;
		basicBGMPitch = 1.0;
		ringoutDepth = -30.0;
		textureEffect = video::EMT_LIGHTMAP_M4;
		stageWireframeFlag = false;
		stageFogEffectFlag = false;
		m_thread_loaded = false;
		wallbox = core::aabbox3d<f32>(0, 0, 0, 0, 0, 0);
		ringoutbox = core::aabbox3d<f32>(0, 0, 0, 0, 0, 0);
		allowsRingoutFlag[FK_PlaneDirections::PlaneNorth] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneWest] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneEast] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneSouth] = false;
		ringoutAllowedDirections = 0;
		additionalObjectsArray.clear();
		additionalObjectsNodes.clear();
		wallboxPlanes.clear();
		stageFog = FK_StageFog();
		rainParticleSystem = NULL;
		dropImpactParticleSystem = NULL;
		dropTimeCounterMS = 0;
		ambientGradientTimer;
		lightGradientTimer;
		ambientGradientIndex = 0;
		lightGradientIndex = 0;
		splashRingoutTextureName = std::string();
		shaderParameters = FK_Stage_ShaderParameters();
		additionalPixelShaders.clear();
		hasComplexStageShape = false;
		cornerbox = core::aabbox3df(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	}
	// constructor #2 (main one): it's just a wrapper for the setup method
	FK_Stage::FK_Stage(IrrlichtDevice* new_device, video::IVideoDriver* new_driver, 
		scene::ISceneManager* new_smgr, std::string new_path) : FK_Stage(){
		setup(new_device, new_driver,
			new_smgr, new_path);
	}

	// destructor
	FK_Stage::~FK_Stage(){
		wallboxPlanes.clear();
		additionalObjectsArray.clear();
		if (getStage() != NULL && getStage()->getReferenceCount() > 0){
			getStage()->remove();
		}
		if (waterNode != NULL && waterNode->getReferenceCount() > 0){
			waterNode->remove();
		}
		for (int i = 0; i < additionalObjectsNodes.size(); ++i){
			if (additionalObjectsNodes[i] != NULL && additionalObjectsNodes[i]->getReferenceCount() > 0){
				additionalObjectsNodes[i]->remove();
			}
		}
		additionalObjectsNodes.clear();
		if (hasRingoutSplashEffect()){
			if (splashingWaterParticleSystem.size() > 0){
				if (splashingWaterParticleSystem[0]->getReferenceCount() > 0){
					splashingWaterParticleSystem[0]->remove();
				}
				if (splashingWaterParticleSystem[1]->getReferenceCount() > 0){
					splashingWaterParticleSystem[1]->remove();
				}
			}
		}
		if (hasWalkingSandEffect()){
			if (splashingSandParticleSystem.size() > 0){
				if (splashingSandParticleSystem[0]->getReferenceCount() > 0){
					splashingSandParticleSystem[0]->remove();
				}
				if (splashingSandParticleSystem[1]->getReferenceCount() > 0){
					splashingSandParticleSystem[1]->remove();
				}
			}
		}
		if (hasActiveWeather && rainParticleSystem->getReferenceCount() > 0){
			rainParticleSystem->remove();
		}
		if (stageWeather.isRaindropEffectActive() && dropImpactParticleSystem->getReferenceCount() > 0){
			dropImpactParticleSystem->remove();
		}
	};

	// setup (true initializer of the class, called during multi-thread processes)
	void FK_Stage::setup(IrrlichtDevice* new_device, video::IVideoDriver* new_driver,
		scene::ISceneManager* new_smgr, std::string new_path){
		device = new_device;
		driver = new_driver;
		smgr = new_smgr;
		path = new_path;
		prepareKeysForFileLoad();
		loadStageFromFile();
		setNode();
		setWaterNode();
		setAdditionalObjects();
		setRainEmitter();
		// set one water splash emitter per player (used on ringout)
		setWaterSplashEmitter();
		// set sand splash effect
		setSandSplashEmitter();
		//if (hasSkybox){
		setSkybox();
		//}
		m_thread_loaded = true;
	}

	// hasBeenLoaded() returns true if the stage has been properly loaded. It is used for multithreading purposes

	bool FK_Stage::hasBeenLoaded(){
		return m_thread_loaded;
	}

	void FK_Stage::prepareKeysForFileLoad(){
		fileKeys[FK_StageFileKeys::StageScale] = "#SCALE";
		fileKeys[FK_StageFileKeys::StagePosition] = "#POSITION";
		fileKeys[FK_StageFileKeys::StageName] = "#NAME";
		fileKeys[FK_StageFileKeys::StageHasSkybox] = "#SKYBOX";
		fileKeys[FK_StageFileKeys::StageArchiveName] = "#ARCHIVE";
		fileKeys[FK_StageFileKeys::StageBGMName] = "#BGM";
		fileKeys[FK_StageFileKeys::StageBGMVolume] = "#BGM_VOLUME";
		fileKeys[FK_StageFileKeys::StageBGMPitch] = "#BGM_PITCH";
		fileKeys[FK_StageFileKeys::StageMeshName] = "#MESH";
		fileKeys[FK_StageFileKeys::StageDescription] = "#DESCRIPTION";
		fileKeys[FK_StageFileKeys::StageDescriptionEnd] = "#DESCRIPTION_END";
		fileKeys[FK_StageFileKeys::StageAmbientColor] = "#AMBIENT_COLOR";
		fileKeys[FK_StageFileKeys::StageEmissionColor] = "#EMISSION_COLOR";
		fileKeys[FK_StageFileKeys::StageLightColor] = "#LIGHT_COLOR";
		fileKeys[FK_StageFileKeys::StageBackgroundColor] = "#BACKGROUND_COLOR";
		fileKeys[FK_StageFileKeys::StageRingoutDepth] = "#RINGOUT_DEPTH";
		fileKeys[FK_StageFileKeys::StageTextureEffect] = "#MATERIAL_EFFECT";
		fileKeys[FK_StageFileKeys::StageShaderParametersBloom] = "#BLOOM_SHADER_LEVELS";
		fileKeys[FK_StageFileKeys::StageShaderParametersBrightPass] = "#BRIGHTPASS_SHADER_LEVELS";
		fileKeys[FK_StageFileKeys::StageWallboxCoordinates] = "#WALLBOX";
		fileKeys[FK_StageFileKeys::StageCornerBoxCoordinates] = "#TRAINING_MODE_CORNERS";
		fileKeys[FK_StageFileKeys::StageRingoutboxCoordinates] = "#RINGOUT_BOX";
		fileKeys[FK_StageFileKeys::StageWaterMesh] = "#WATER";
		fileKeys[FK_StageFileKeys::StageAllowRingout] = "#ALLOW_RINGOUT";
		fileKeys[FK_StageFileKeys::StageFogEffect] = "#FOG_EFFECT";
		fileKeys[FK_StageFileKeys::StageRain] = "#WEATHER";
		fileKeys[FK_StageFileKeys::StageRaindropEffect] = "#RAINDROP_EFFECT";
		fileKeys[FK_StageFileKeys::StageAdditionalObject] = "#ADDITIONAL_OBJECT";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_Position] = "#position";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_Rotation] = "#rotation";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_Scale] = "#scale";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_BackfaceCulling] = "#backface_culling";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_MaterialEffect] = "#effect";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_Animator] = "#animator";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_MeshName] = "#mesh";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_ParticleSystemName] = "#particles";
		fileKeys[FK_StageFileKeys::StageAdditionalObject_End] = "#ADDITIONAL_OBJECT_END";
		fileKeys[FK_StageFileKeys::StageTechnicolorAmbientEffect] = "#AMBIENT_COLOR_GRADIENT";
		fileKeys[FK_StageFileKeys::StageTechnicolorAmbientEffect_End] = "#AMBIENT_COLOR_GRADIENT_END";
		fileKeys[FK_StageFileKeys::StageTechnicolorLightEffect] = "#LIGHT_COLOR_GRADIENT";
		fileKeys[FK_StageFileKeys::StageTechnicolorLightEffect_End] = "#LIGHT_COLOR_GRADIENT_END";
		fileKeys[FK_StageFileKeys::StageSplashRingout] = "#RINGOUT_SPLASH_EFFECT";
		fileKeys[FK_StageFileKeys::StageSplashRingoutSoundEffect] = "#RINGOUT_SPLASH_SOUND";
		fileKeys[FK_StageFileKeys::StageSandSplashEffect] = "#WALKING_SPLASH_EFFECT";
		fileKeys[FK_StageFileKeys::StageAdditionalShaders] = "#ADDITIONAL_SHADERS";
		fileKeys[FK_StageFileKeys::StageAdditionalShadersEnd] = "#ADDITIONAL_SHADERS_END";
		fileKeys[FK_StageFileKeys::StageDioramaLore] = "#DIORAMA_DESCRIPTION";
		fileKeys[FK_StageFileKeys::StageDioramaLoreEnd] = "#DIORAMA_DESCRIPTION_END";
		fileKeys[FK_StageFileKeys::StageBGMMetadata] = "#BGM_METADATA";
		fileKeys[FK_StageFileKeys::StageComplexWallboxCoordinates] = "#COMPLEX_WALLBOX";
		fileKeys[FK_StageFileKeys::StageComplexWallboxCoordinates_End] = "#COMPLEX_WALLBOX_END";
		fileKeys[FK_StageFileKeys::StageComplexRingoutboxCoordinates] = "#COMPLEX_RINGOUTBOX";
		fileKeys[FK_StageFileKeys::StageComplexRingoutboxCoordinates_End] = "#COMPLEX_RINGOUTBOX_END";
		fileKeys[FK_StageFileKeys::StageComplexRingoutPlaneIndices] = "#COMPLEX_RINGOUT_ALLOWANCE_FLAGS";
		fileKeys[FK_StageFileKeys::StageComplexRingoutPlaneIndices_End] = "#COMPLEX_RINGOUT_ALLOWANCE_FLAGS_END";
	}

	void FK_Stage::loadStageFromFile(){
		/* open file and start parsing it */
		std::string defaultConfigFileName = "Config.txt";
		std::string filename(path + defaultConfigFileName);
		std::ifstream inputFile(filename.c_str());

		/* prepare material map for texture effects */
		std::map < std::string, video::E_MATERIAL_TYPE > materialMap;
		materialMap["solid_texture"] = EMT_SOLID;
		materialMap["lighting"] = EMT_LIGHTMAP_M4;
		materialMap["ghost"] = EMT_TRANSPARENT_ADD_COLOR;
		materialMap["alpha"] = EMT_TRANSPARENT_ALPHA_CHANNEL;
		materialMap["alpha_ref"] = EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		materialMap["reflect"] = EMT_REFLECTION_2_LAYER;

		std::map <std::string, FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType> additionalObjectsAnimators;
		additionalObjectsAnimators["circular"] = FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::CircularAnimator;
		additionalObjectsAnimators["linear_loop"] = FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::LinearLoopAnimator;
		additionalObjectsAnimators["linear_pingpong"] = 
			FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::LinearPingPongAnimator;


		std::map <std::string, FK_StagePixelShader::FK_StagePixelShaderParameters> pixelShaderParameters;
		pixelShaderParameters["character_positions"] = FK_StagePixelShader::FK_StagePixelShaderParameters::characterPositionsXY;
		pixelShaderParameters["time_ms"] = FK_StagePixelShader::FK_StagePixelShaderParameters::timeMs;
		pixelShaderParameters["timer_value"] = FK_StagePixelShader::FK_StagePixelShaderParameters::timerValue;

		while (inputFile && !inputFile.eof()){
			std::string temp;
			inputFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (temp == fileKeys[FK_StageFileKeys::StageName]){
				inputFile >> temp;
				name = temp;
				std::replace(name.begin(), name.end(), '_', ' ');
				wname = fk_addons::convertNameToNonASCIIWstring(name);
			}
			/* archive name */
			else if (temp == fileKeys[FK_StageFileKeys::StageArchiveName]){
				inputFile >> temp;
				archiveName = temp;
				isZipped = true;
			}
			/* mesh name */
			else if (temp == fileKeys[FK_StageFileKeys::StageMeshName]){
				inputFile >> temp;
				meshName = temp;
			}
			/* rain */
			else if (temp == fileKeys[FK_StageFileKeys::StageRain]){
				hasActiveWeather = true;
				std::string texName;
				u32 intensityMin, intensityMax;
				f32 minWidth, minHeight, maxWidth, maxHeight;
				f32 gravity;
				u32 fadingTime;
				inputFile >> texName >> intensityMin >> intensityMax >> minWidth >> minHeight >> maxWidth >> maxHeight;
				inputFile >> gravity >> fadingTime;
				stageWeather = FK_StageWeather(path + texName, intensityMin, intensityMax, core::dimension2df(minWidth, minHeight),
					core::dimension2df(maxWidth, maxHeight), gravity, fadingTime);
			}
			/* raindrops */
			else if (temp == fileKeys[FK_StageFileKeys::StageRaindropEffect]){
				inputFile >> temp;
				stageWeather.setRaindropEffect(temp);
			}
			/* ring out depth */
			else if (temp == fileKeys[FK_StageFileKeys::StageRingoutDepth]){
				f32 temp_float = 0;
				inputFile >> temp_float;
				ringoutDepth = temp_float;
			}
			/* ring out splash (water effect) */
			else if (temp == fileKeys[FK_StageFileKeys::StageSplashRingout]){
				inputFile >> splashRingoutTextureName;
			}
			/* ring out splash (water effect) */
			else if (temp == fileKeys[FK_StageFileKeys::StageSplashRingoutSoundEffect]) {
				std::string filename;
				f32 volume, pitch;
				inputFile >> filename >> volume >> pitch;
				splashSoundEffect.filename = filename;
				splashSoundEffect.volume = volume;
				splashSoundEffect.pitch = pitch;
			}
			/* sand splash (water effect) */
			else if (temp == fileKeys[FK_StageFileKeys::StageSandSplashEffect]){
				inputFile >> splashSandTextureName;
			}
			/* bgm name */
			else if (temp == fileKeys[FK_StageFileKeys::StageBGMName]){
				inputFile >> temp;
				bgmName = temp;
			}
			/* bgm volume */
			else if (temp == fileKeys[FK_StageFileKeys::StageBGMVolume]){
				f32 temp_float = 0;
				inputFile >> temp_float;
				basicBGMVolume = temp_float;
			}
			/* bgm pitch */
			else if (temp == fileKeys[FK_StageFileKeys::StageBGMPitch]){
				f32 temp_float = 0;
				inputFile >> temp_float;
				basicBGMPitch = temp_float;
			}
			/* fog */
			else if (temp == fileKeys[FK_StageFileKeys::StageFogEffect]){
				s32 a, r, g, b;
				f32 temp_rangeMin, temp_rangeMax;
				inputFile >> a >> r >> g >> b;
				inputFile >> temp_rangeMin >> temp_rangeMax;
				stageFog = FK_StageFog(video::SColor(a, r, g, b), video::E_FOG_TYPE::EFT_FOG_LINEAR, temp_rangeMin,
					temp_rangeMax, 0.1f);
				stageFogEffectFlag = true;
			}
			/* description */
			else if (temp == fileKeys[FK_StageFileKeys::StageDescription]){
				std::ostringstream buffer;
				while (!inputFile.eof()){
					std::string temp;
					inputFile >> temp;
					/* if the DESCRIPTION end tag is found, break cycle*/
					if (temp == fileKeys[FK_StageFileKeys::StageDescriptionEnd]){
						break;
					}
					else{
						buffer << temp << " ";
					}
				}
				description = buffer.str();
				wdescription = fk_addons::convertNameToNonASCIIWstring(description);
			}
			/* lore */
			else if (temp == fileKeys[FK_StageFileKeys::StageDioramaLore]) {
				dioramaLoreLines.clear();
				while (inputFile) {
					std::getline(inputFile, temp);
					if (temp == fileKeys[FK_StageFileKeys::StageDioramaLoreEnd]) {
						break;
					}
					if (!(temp.empty())) {
						dioramaLoreLines.push_back(temp);
					}
				}
			}
			/* bgm metadata */
			else if (temp == fileKeys[FK_StageFileKeys::StageBGMMetadata]) {
				std::string bgmName;
				std::string bgmAuthor;
				inputFile >> bgmName >> bgmAuthor;
				std::replace(bgmName.begin(), bgmName.end(), '_', ' ');
				std::replace(bgmAuthor.begin(), bgmAuthor.end(), '_', ' ');
				bgmMetadata.first = fk_addons::convertNameToNonASCIIWstring(bgmName);
				bgmMetadata.second = fk_addons::convertNameToNonASCIIWstring(bgmAuthor);
			}
			/* stage texture effect */
			else if (temp == fileKeys[FK_StageFileKeys::StageTextureEffect]){
				std::string temp;
				inputFile >> temp;
				if (materialMap.count(temp) > 0){
					textureEffect = materialMap[temp];
				}
				else{
					/* check if the option matches wireframe key*/
					std::string wireframeFlagKey = "wireframe";
					if (temp == wireframeFlagKey){
						stageWireframeFlag = true;
					}
				}
			}
			/* scale factor */
			else if (temp == fileKeys[FK_StageFileKeys::StageScale]){
				f32 x, y, z;
				inputFile >> x >> y >> z;
				scale = core::vector3df(x, y, z);
			}
			/* position */
			else if (temp == fileKeys[FK_StageFileKeys::StagePosition]){
				f32 x, y, z;
				inputFile >> x >> y >> z;
				position = core::vector3df(x, y, z);
			}
			/* ambient color */
			else if (temp == fileKeys[FK_StageFileKeys::StageAmbientColor]){
				f32 r, g, b;
				inputFile >> r >> g >> b;
				ambientColor = SColor(255, r, g, b);
			}
			/* emission color */
			else if (temp == fileKeys[FK_StageFileKeys::StageEmissionColor]){
				f32 r, g, b;
				inputFile >> r >> g >> b;
				emissionColor = SColor(255, r, g, b);
				textureEffect = E_MATERIAL_TYPE::EMT_LIGHTMAP_M4;
			}
			/* bloom settings */
			else if (temp == fileKeys[FK_StageFileKeys::StageShaderParametersBloom]){
				f32 normal, bloom;
				inputFile >> normal >> bloom;
				shaderParameters.sceneValue = normal;
				shaderParameters.bloomValue = bloom;
			}
			/* brightpass settings */
			else if (temp == fileKeys[FK_StageFileKeys::StageShaderParametersBrightPass]){
				f32 hicut, lowcut;
				inputFile >> hicut >> lowcut;
				shaderParameters.brightnessCutHigh = hicut;
				shaderParameters.brightnessCutLow = lowcut;
			}
			/* additional pixel shaders */
			else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalShaders]) {
				temp = std::string();
				while (inputFile) {
					while (temp.empty() || temp == " " || temp == "\t") {
						std::getline(inputFile, temp);
					}
					if (temp != fileKeys[FK_StageFileKeys::StageAdditionalShadersEnd]) {
						std::istringstream strstream(temp);
						std::string filename;
						strstream >> filename;
						FK_StagePixelShader shader;
						shader.filename = filename;
						shader.parameters.push_back(FK_StagePixelShader::FK_StagePixelShaderParameters::timeMs);
						additionalPixelShaders.push_back(shader);
						temp = std::string();
					}
					else {
						break;
					}
				}
			}
			/* ambient gradient color */
			else if (temp == fileKeys[FK_StageFileKeys::StageTechnicolorAmbientEffect]){
				temp = std::string();
				while (inputFile){
					while (temp.empty()){
						std::getline(inputFile, temp);
					}
					if (temp != fileKeys[FK_StageFileKeys::StageTechnicolorAmbientEffect_End]){
						std::istringstream strstream(temp);
						u32 time_ms;
						u32 r, g, b;
						strstream >> time_ms >> r >> g >> b;
						video::SColor color = SColor(255, r, g, b);
						ambientColorGradient.push_back(std::pair<u32, video::SColor>(time_ms, color));
						std::getline(inputFile, temp);
					}
					else{
						break;
					}
				}
			}
			/* light gradient color */
			else if (temp == fileKeys[FK_StageFileKeys::StageTechnicolorLightEffect]){
				temp = std::string();
				while (inputFile){
					while (temp.empty()){
						std::getline(inputFile, temp);
					}
					if (temp != fileKeys[FK_StageFileKeys::StageTechnicolorLightEffect_End]){
						std::istringstream strstream(temp);
						u32 time_ms;
						u32 r, g, b;
						strstream >> time_ms >> r >> g >> b;
						video::SColor color = SColor(255, r, g, b);
						lightColorGradient.push_back(std::pair<u32, video::SColor>(time_ms, color));
						std::getline(inputFile, temp);
					}
					else{
						break;
					}
				}
			}
			/* light color */
			else if (temp == fileKeys[FK_StageFileKeys::StageLightColor]){
				f32 r, g, b;
				inputFile >> r >> g >> b;
				lightColor = SColor(255, r, g, b);
			}
			/* background (i.e. not rendered) color */
			else if (temp == fileKeys[FK_StageFileKeys::StageBackgroundColor]){
				f32 r, g, b;
				inputFile >> r >> g >> b;
				backgroundColor = SColor(255, r, g, b);
			}
			/* has skybox */
			else if (temp == fileKeys[FK_StageFileKeys::StageHasSkybox]){
				hasSkybox = true;
			}
			/* allows ringout */
			else if (temp == fileKeys[FK_StageFileKeys::StageAllowRingout]){
				u32 north, east, south, west;
				inputFile >> north >> east >> south >> west;
				ringoutAllowedDirections = 
					east * (u32)FK_PlaneDirections::PlaneEast +
					west *(u32)FK_PlaneDirections::PlaneWest +
					north * (u32)FK_PlaneDirections::PlaneNorth +
					south * (u32)FK_PlaneDirections::PlaneSouth;
				allowsRingoutFlag[FK_PlaneDirections::PlaneNorth] = north;
				allowsRingoutFlag[FK_PlaneDirections::PlaneWest] = west;
				allowsRingoutFlag[FK_PlaneDirections::PlaneEast] = east;
				allowsRingoutFlag[FK_PlaneDirections::PlaneSouth] = south;
			}
			/* wallbox */
			else if (temp == fileKeys[FK_StageFileKeys::StageWallboxCoordinates]){
				f32 xMin, yMin, zMin, xMax, yMax, zMax;
				inputFile >> xMin >> yMin >> zMin >> xMax >> yMax >> zMax;
				wallbox = core::aabbox3d<f32>(xMin, yMin, zMin, xMax, yMax, zMax);
				// create wallbox planes
				createWallboxPlanes();
				// create ringout box
				if (ringoutbox.isEmpty()){
					f32 scaleFactorForRingoutBox = 0.9f;
					ringoutbox = core::aabbox3d<f32>(xMin * scaleFactorForRingoutBox,
						yMin,
						zMin * scaleFactorForRingoutBox,
						xMax * scaleFactorForRingoutBox,
						yMax,
						zMax * scaleFactorForRingoutBox);
				}
			}
			/* corner box */
			else if (temp == fileKeys[FK_StageFileKeys::StageCornerBoxCoordinates]) {
				f32 xMin, zMin, xMax, zMax;
				inputFile >> xMin >> zMin >> xMax >> zMax;
				cornerbox = core::aabbox3d<f32>(xMin, 0, zMin, xMax, 0, zMax);
			}
			/* ringout box */
			else if (temp == fileKeys[FK_StageFileKeys::StageRingoutboxCoordinates]){
				f32 xMin, yMin, zMin, xMax, yMax, zMax;
				inputFile >> xMin >> yMin >> zMin >> xMax >> yMax >> zMax;
				ringoutbox = core::aabbox3d<f32>(xMin, yMin, zMin, xMax, yMax, zMax);
			}
			/* complex wallbox */
			else if (temp == fileKeys[FK_StageFileKeys::StageComplexWallboxCoordinates]) {
				hasComplexStageShape = true;
				std::string temp;
				while (inputFile) {
					while (temp.empty()) {
						std::getline(inputFile, temp);
					}
					if (temp != fileKeys[FK_StageFileKeys::StageComplexWallboxCoordinates_End]) {
						std::istringstream strstream(temp);
						f32 x0, y0, z0;
						strstream >> x0 >> y0 >> z0;
						complexWallBoxEdges.push_back(core::vector3df(x0, y0, z0));
						std::getline(inputFile, temp);
					}
					else {
						break;
					}
				}
				createComplexWallboxPlanes();
			}
			/* ringout box */
			else if (temp == fileKeys[FK_StageFileKeys::StageComplexRingoutboxCoordinates]) {
				if (!hasComplexStageShape) {
					continue;
				}
				std::string temp;
				while (inputFile) {
					while (temp.empty()) {
						std::getline(inputFile, temp);
					}
					if (temp != fileKeys[FK_StageFileKeys::StageComplexRingoutboxCoordinates_End]) {
						std::istringstream strstream(temp);
						f32 x0, y0, z0;
						strstream >> x0 >> y0 >> z0;
						complexRingOutBoxEdges.push_back(core::vector3df(x0, y0, z0));
						std::getline(inputFile, temp);
					}
					else {
						break;
					}
				}
				createComplexRingoutBoxPlanes();
			}
			/* ringout allowance */
			else if (temp == fileKeys[FK_StageFileKeys::StageComplexRingoutPlaneIndices]) {
				if (!hasComplexStageShape) {
					continue;
				}
				u32 size = complexRingoutBoxPlanes.size();
				for (u32 i = 0; i < size; ++i) {
					u32 flag;
					inputFile >> flag;
					complexAllowsRingoutFlag.push_back(flag > 0);
				}
			}
			/* add water (just another special effect :3)*/
			else if (temp == fileKeys[FK_StageFileKeys::StageWaterMesh]){
				f32 x, y, z;
				f32 f, l, h;
				inputFile >> temp >> x >> y >> z;
				inputFile >> f >> l >> h;
				waterMeshName = temp;
				waterNodePosition = core::vector3df(x, y, z);
				waterNodeCharacteristics = core::vector3df(f, l, h);
			}
			/* add additional mesh */
			else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject]){
				FK_StageAdditionalObject tempObject;
				while (temp != fileKeys[FK_StageFileKeys::StageAdditionalObject_End]){
					inputFile >> temp;
					if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_MeshName]){
						inputFile >> temp;
						tempObject.setMeshName(temp);
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_ParticleSystemName]){
						inputFile >> temp;
						tempObject.setParticleTextureName(temp);
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_BackfaceCulling]){
						tempObject.setBackfaceCulling(true);
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_Position]){
						f32 x, y, z;
						inputFile >> x >> y >> z;
						tempObject.setPosition(core::vector3df(x, y, z));
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_Rotation]){
						f32 x, y, z;
						inputFile >> x >> y >> z;
						tempObject.setRotation(core::vector3df(x, y, z));
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_Scale]) {
						f32 x, y, z;
						inputFile >> x >> y >> z;
						tempObject.setScale(core::vector3df(x, y, z));
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_MaterialEffect]){
						inputFile >> temp;
						if (materialMap.count(temp) > 0){
							tempObject.setMaterialEffect(materialMap[temp]);
						}
						else{
							/* check if the option matches wireframe key*/
							std::string wireframeFlagKey = "wireframe";
							if (temp.c_str() == wireframeFlagKey){
								tempObject.setWireframeFlag();
							}
						}
					}
					else if (temp == fileKeys[FK_StageFileKeys::StageAdditionalObject_Animator]){
						inputFile >> temp;
						if (additionalObjectsAnimators.count(temp) > 0){
							tempObject.setAnimatorType(additionalObjectsAnimators[temp]);
						}
						if (additionalObjectsAnimators[temp] ==	
							FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::CircularAnimator){
							f32 x, y, z;
							inputFile >> x >> y >> z;
							core::vector3df rotcenter(x, y, z);
							inputFile >> x >> y >> z;
							core::vector3df rotaxis(x, y, z);
							inputFile >> x >> y;
							f32 velocity = y;
							f32 radius = x;
							tempObject.setCircularAnimatorParameters(rotcenter, rotaxis, radius, velocity);
						}
						else if (additionalObjectsAnimators[temp] ==
							FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::LinearLoopAnimator){
							f32 x, y, z;
							inputFile >> x >> y >> z;
							core::vector3df startingPoint(x, y, z);
							inputFile >> x >> y >> z;
							core::vector3df endPoint(x, y, z);
							inputFile >> y;
							f32 velocity = y;
							tempObject.setLinearAnimatorParameters(startingPoint, endPoint, velocity, true, false);
						}
						else if (additionalObjectsAnimators[temp] ==
							FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::LinearPingPongAnimator){
							f32 x, y, z;
							inputFile >> x >> y >> z;
							core::vector3df startingPoint(x, y, z);
							inputFile >> x >> y >> z;
							core::vector3df endPoint(x, y, z);
							inputFile >> y;
							f32 velocity = y;
							tempObject.setLinearAnimatorParameters(startingPoint, endPoint, velocity, true, true);
						}
					}
					
				}
				if (tempObject.isBuildable()){
					additionalObjectsArray.push_back(tempObject);
				}
			}
		}
	}

	/* create wallbox planes */
	void FK_Stage::createWallboxPlanes(){
		if (wallbox.isEmpty()){
			return;
		}
		core::vector3d<f32> edges[8];
		wallbox.getEdges(edges);
		core::plane3d<f32> eastPlane = core::plane3d<f32>(edges[2], edges[3], edges[6]);
		core::plane3d<f32> westPlane = core::plane3d<f32>(edges[1], edges[0], edges[4]);
		core::plane3d<f32> southPlane = core::plane3d<f32>(edges[5], edges[4], edges[6]);
		core::plane3d<f32> northPlane = core::plane3d<f32>(edges[0], edges[1], edges[2]);
		wallboxPlanes[FK_PlaneDirections::PlaneNorth] = northPlane;
		wallboxPlanes[FK_PlaneDirections::PlaneSouth] = southPlane;
		wallboxPlanes[FK_PlaneDirections::PlaneEast] = eastPlane;
		wallboxPlanes[FK_PlaneDirections::PlaneWest] = westPlane;
	}

	void FK_Stage::createComplexWallboxPlanes() {
		if (!hasComplexStageShape || complexWallBoxEdges.empty()) {
			return;
		}
		u32 size = complexWallBoxEdges.size();
		u32 numberOfFaces = size / 2 + 1;
		u32 planeIndex = 0;
		for (u32 i = 0; i < size; i += 2) {
			u32 i0 = i;
			u32 i1 = (i + 1) % size;
			u32 i2 = (i + 2) % size;
			core::plane3d<f32> testPlane = core::plane3d<f32>(complexWallBoxEdges[i0], 
				complexWallBoxEdges[i1], 
				complexWallBoxEdges[i2]);
			complexWallBoxPlanes[planeIndex] = testPlane;
			planeIndex += 1;
		}
		numberOfFaces = planeIndex;
	}

	void FK_Stage::createComplexRingoutBoxPlanes() {
		if (!hasComplexStageShape || complexRingOutBoxEdges.empty()) {
			return;
		}
		u32 size = complexWallBoxEdges.size();
		u32 numberOfFaces = size / 2 + 1;
		u32 planeIndex = 0;
		for (u32 i = 0; i < size; i += 2) {
			u32 i0 = i;
			u32 i1 = (i + 1) % size;
			u32 i2 = (i + 2) % size;
			core::plane3d<f32> testPlane = core::plane3d<f32>(complexWallBoxEdges[i0],
				complexWallBoxEdges[i1],
				complexWallBoxEdges[i2]);
			complexRingoutBoxPlanes[planeIndex] = testPlane;
			planeIndex += 1;
		}
		numberOfFaces = planeIndex;
	}

	/* get wallbox planes */
	core::plane3d<f32> FK_Stage::getWallboxPlane(FK_PlaneDirections direction){
		if (wallbox.isEmpty()){
			return core::plane3d<f32>();
		}
		return wallboxPlanes[direction];
	};

	core::plane3d<f32> FK_Stage::getComplexWallBoxPlane(u32 index) {
		if (complexWallBoxPlanes.empty() || index >= complexWallBoxPlanes.size()) {
			return core::plane3d<f32>();
		}
		return complexWallBoxPlanes[index];
	}
	u32 FK_Stage::getNumberofPlanesOfComplexWallBox()
	{
		return complexWallBoxPlanes.size();
	}
	core::plane3d<f32> FK_Stage::getComplexRingoutBoxPlane(u32 index)
	{
		if (complexRingoutBoxPlanes.empty() || index >= complexRingoutBoxPlanes.size()) {
			return core::plane3d<f32>();
		}
		return complexRingoutBoxPlanes[index];
	}

	u32 FK_Stage::getNumberofPlanesOfComplexRingoutBox()
	{
		return complexRingoutBoxPlanes.size();
	}


	bool FK_Stage::hasComplexShape()
	{
		return hasComplexStageShape;
	}
	;

	/* set irrlicht scene node*/
	void FK_Stage::setNode(){
		if (node == NULL){
			if (isZipped){
				device->getFileSystem()->addFileArchive((path + archiveName).c_str());
				mesh = smgr->getMesh(meshName.c_str());
			}
			else{
				mesh = smgr->getMesh((path + meshName).c_str());
			}
			mesh->setHardwareMappingHint(EHM_STATIC);
			node = smgr->addOctreeSceneNode(mesh->getMesh(0));
		}
		/* set material scale and position */
		node->setScale(scale);
		node->setPosition(position);
		/* set material transparency */
		int mcount = node->getMaterialCount();
		for (int i = 0; i < mcount; i++)
		{
			//node->getMaterial(i).Wireframe = true;
			//FIX TEXTURE OPTIONS!!!!
			//E_MATERIAL_TYPE mat = node->getMaterial(i).MaterialType;
			node->getMaterial(i).MaterialType = textureEffect;
			node->getMaterial(i).FogEnable = stageFogEffectFlag;
			node->getMaterial(i).Wireframe = stageWireframeFlag;
			node->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
			//node->getMaterial(i).Shininess = 25.0;
			node->getMaterial(i).EmissiveColor = emissionColor;
		}
	}

	/* set water if water is present */
	void FK_Stage::setWaterNode(){
		if (waterMeshName.empty()){
			return;
		}
		waterMesh = smgr->getMesh((path + waterMeshName).c_str());
		f32 wavelength = waterNodeCharacteristics.X;
		f32 speed = waterNodeCharacteristics.Y;
		f32 amplitude = waterNodeCharacteristics.Z;
		waterNode = smgr->addWaterSurfaceSceneNode(waterMesh, amplitude, speed, wavelength);
		waterNode->setScale(scale);
		int mcount = waterNode->getMaterialCount();
		waterNode->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
		waterNode->getMaterial(0).setFlag(video::EMF_ZWRITE_ENABLE, true);
		waterNode->getMaterial(0).BackfaceCulling = false;
		waterNode->setPosition(waterNodePosition);
	}

	/* set skybox if skybox is present */
	void FK_Stage::setSkybox(){
		if (skybox == NULL){
			std::string skyboxSubDirectoryIdentifier = "skybox\\";
			std::string skypath = path + skyboxSubDirectoryIdentifier;
			skybox = smgr->addSkyBoxSceneNode(
				driver->getTexture((skypath + "top.png").c_str()),
				driver->getTexture((skypath + "bottom.png").c_str()),
				driver->getTexture((skypath + "left.png").c_str()),
				driver->getTexture((skypath + "right.png").c_str()),
				driver->getTexture((skypath + "front.png").c_str()),
				driver->getTexture((skypath + "back.png").c_str()));
			if (!hasSkybox){
				skybox->setMaterialFlag(E_MATERIAL_FLAG::EMF_LIGHTING, true);
			}
		}
	}

	/* set additional objects if needed */
	void FK_Stage::setAdditionalObjects(){
		if (additionalObjectsArray.empty()){
			return;
		}
		if (!additionalObjectsNodes.empty()){
			return;
		}
		int size = additionalObjectsArray.size();
		for (int i = 0; i < size; ++i){
			if (additionalObjectsArray[i].isParticleSystem()){
				auto temp_node = smgr->addParticleSystemSceneNode(false);
				temp_node->setPosition(additionalObjectsArray[i].getPosition() - position);
				temp_node->setRotation(additionalObjectsArray[i].getRotation());
				std::string texpath = path + additionalObjectsArray[i].getParticleTextureName();
				auto SFXTexture = smgr->getVideoDriver()->getTexture(texpath.c_str());
				scene::IParticleEmitter* em = temp_node->createBoxEmitter(
					core::aabbox3d<f32>(-4, 0, -4, 4, 1, 4), // emitter size
					core::vector3df(0.0f, 0.1f, 0.0f),   // initial direction
					50, 60,                             // emit rate (0 assures that no particles are created until a hit)
					video::SColor(0, 255, 255, 255),       // darkest color
					video::SColor(0, 255, 255, 255),       // brightest color
					100, 500, 360,                         // min and max age, angle
					core::dimension2df(8.0f, 8.0f),         // min size
					core::dimension2df(26.0f, 26.0f));        // max size

				temp_node->setEmitter(em); // this grabs the emitter
				em->drop(); // so we can drop it here without deleting it
				temp_node->setMaterialFlag(video::EMF_LIGHTING, false);
				temp_node->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);

				temp_node->setMaterialTexture(0, SFXTexture);
				temp_node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
				additionalObjectsNodes.push_back(temp_node);
			}
			else{
				ISceneNode* temp_node;
				IAnimatedMesh* temp_mesh = smgr->getMesh((path + additionalObjectsArray[i].getMeshName()).c_str());
				temp_node = smgr->addAnimatedMeshSceneNode(temp_mesh);
				temp_node->setPosition(additionalObjectsArray[i].getPosition() - position);
				temp_node->setRotation(additionalObjectsArray[i].getRotation());
				auto objectScaling = scale * additionalObjectsArray[i].getScale();
				temp_node->setScale(objectScaling);
				for (int j = 0; j < temp_node->getMaterialCount(); ++j){
					temp_node->getMaterial(j).setFlag(video::EMF_ZWRITE_ENABLE, true);
					temp_node->getMaterial(j).BackfaceCulling = additionalObjectsArray[i].getBackfaceCulling();
					temp_node->getMaterial(j).FogEnable = stageFogEffectFlag;
					temp_node->getMaterial(j).MaterialType = additionalObjectsArray[i].getMaterialEffect();
					temp_node->getMaterial(j).Wireframe = additionalObjectsArray[i].getWireframeFlag();
				}
				// apply animator, if any
				scene::ISceneNodeAnimator* anim = additionalObjectsArray[i].setupAnimator(smgr);
				if (anim)
				{
					temp_node->addAnimator(anim);
					anim->drop();
				}
				additionalObjectsNodes.push_back(temp_node);
			}
		};
	}

	/* get mesh */
	scene::IAnimatedMesh* FK_Stage::getStageMesh(){
		return mesh;
	}

	/* get water mesh */
	scene::IAnimatedMesh* FK_Stage::getWaterMesh(){
		return waterMesh;
	}

	/* get scene node */
	scene::ISceneNode* FK_Stage::getStage(){
		return node;
	}

	/* get water scene node */
	scene::ISceneNode* FK_Stage::getStageWaterNode(){
		return waterNode;
	}

	/* get shader information */
	FK_Stage::FK_Stage_ShaderParameters FK_Stage::getParametersForShader(){
		return shaderParameters;
	}

	const std::vector<FK_Stage::FK_StagePixelShader>& FK_Stage::getAdditionalPixelShaders() const
	{
		return additionalPixelShaders;
	}

	/* get stage splash sound effect */
	FK_Stage::FK_Stage_SFX FK_Stage::getSplashSoundEffect()
	{
		return splashSoundEffect;
	}

	/* get stage diorama lore */
	std::vector<std::string> FK_Stage::getDioramaLoreText()
	{
		return dioramaLoreLines;
	}

	std::pair<std::wstring, std::wstring> FK_Stage::getBGMMetaData()
	{
		return bgmMetadata;
	}

	/* check if scene has skybox */
	bool FK_Stage::hasAvailableSkybox(){
		return hasSkybox;
	}

	/* get skybox node */
	scene::ISceneNode* FK_Stage::getSkybox(){
		return skybox;
	}

	/* get additional nodes */
	std::vector<ISceneNode*> & FK_Stage::getAdditionalNodes(){
		return additionalObjectsNodes;
	}
	/* get stage name */
	std::wstring FK_Stage::getName(){
		return wname;
	}

	/* get ring out depth (i.e. the Y value after which the character is considered out of the ring (default: -30)*/
	f32 FK_Stage::getRingoutDepth(){
		return ringoutDepth;
	}

	/* get bgm name */
	std::string FK_Stage::getBGMName(){
		return bgmName;
	}

	/* get bgm volume*/
	f32 FK_Stage::getBGMVolume(){
		return basicBGMVolume;
	}

	/* get bgm pitch*/
	f32 FK_Stage::getBGMPitch(){
		return basicBGMPitch;
	};

	/* get bgm pause volume*/
	f32 FK_Stage::getBGMPauseVolume(){
		return basicBGMVolume < 60.0 ? 15.0f : basicBGMVolume / 4.0;
	}

	/* get bgm trigger volume*/
	f32 FK_Stage::getBGMTriggerVolume(){
		return basicBGMVolume > 0 ? basicBGMVolume + 10.0 : basicBGMVolume;
	}

	/* get stage description */
	std::wstring FK_Stage::getDescription(){
		return wdescription;
	}

	/* get stage ambient color */
	video::SColor FK_Stage::getAmbientColor(){
		return ambientColor;
	}

	/* get stage light color */
	video::SColor FK_Stage::getLightColor(){
		return lightColor;
	}

	/* get stage emissive color */
	video::SColor FK_Stage::getEmissionColor(){
		return emissionColor;
	}

	/* get stage background color */
	video::SColor FK_Stage::getBackgroundColor(){
		return backgroundColor;
	}

	/* get wallbox */
	core::aabbox3d<f32> FK_Stage::getWallbox(){
		return wallbox;
	}

	/* get ringoutbox */
	core::aabbox3d<f32> FK_Stage::getRingoutbox(){
		return ringoutbox;
	}

	/* get starting positions for training mode*/
	void FK_Stage::getCenterStartingPosition(
		core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition) {
		leftPlayerPosition = core::vector3df(50, 0, -60);
		rightPlayerPosition = core::vector3df(50, 0, 60);
	}
	void FK_Stage::getRightPlayerAtWallPosition(
		core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition) {
		core::aabbox3df box = cornerbox;
		if (cornerbox.isEmpty()) {
			box = wallbox;
		}
		auto tempEdge = box.MaxEdge;
		tempEdge.X -= 10;
		tempEdge.Z -= 10;
		tempEdge.Y = 0;
		rightPlayerPosition = tempEdge;
		tempEdge.X -= 50;
		tempEdge.Z -= 50;
		leftPlayerPosition = tempEdge;
	}
	void FK_Stage::getLeftPlayerAtWallPosition(core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition) {
		core::aabbox3df box = cornerbox;
		if (cornerbox.isEmpty()) {
			box = wallbox;
		}
		auto tempEdge = box.MinEdge;
		tempEdge.X += 10;
		tempEdge.Z += 10;
		tempEdge.Y = 0;
		leftPlayerPosition = tempEdge;
		tempEdge.X += 50;
		tempEdge.Z += 50;
		rightPlayerPosition = tempEdge;
	}

	/* check if stage allows ringout */
	bool FK_Stage::allowsRingout(){
		return ringoutAllowedDirections != 0;
		/*return (allowsRingout(FK_PlaneDirections::PlaneNorth) ||
			allowsRingout(FK_PlaneDirections::PlaneEast) ||
			allowsRingout(FK_PlaneDirections::PlaneWest) ||
			allowsRingout(FK_PlaneDirections::PlaneSouth)
			);*/
	}

	/* check if stage allows ringout */
	bool FK_Stage::allowsRingout(FK_PlaneDirections direction){
		if (direction == FK_PlaneDirections::PlaneNone){
			return false;
		}
		return allowsRingoutFlag[direction];
	}

	/* check if stage allows ringout */
	bool FK_Stage::allowsRingout(u32 direction){
		if (!hasComplexStageShape) {
			return (direction & ringoutAllowedDirections) != 0;
		}
		else {
			u32 vecSize = complexAllowsRingoutFlag.size();
			if (vecSize == 0) {
				return false;
			}
			else {
				if (direction >= vecSize) {
					u32 firstDirection = direction % vecSize;
					u32 secondDirection = direction - firstDirection;
					return complexAllowsRingoutFlag[firstDirection] && complexAllowsRingoutFlag[secondDirection];
				}
				else {
					return complexAllowsRingoutFlag[direction];
				}
			}
		}
	}

	/* set all meshes as visible or not visible */
	void FK_Stage::toggleVisibility(bool new_visibility_flag){
		node->setVisible(new_visibility_flag);
		if (skybox != NULL){
			skybox->setVisible(new_visibility_flag);
		}
		if (waterNode != NULL){
			waterNode->setVisible(new_visibility_flag);
		}
		for each (ISceneNode* sceneNode in additionalObjectsNodes){
			sceneNode->setVisible(new_visibility_flag);
		}
	}

	/* update stage elements */
	void FK_Stage::update(u32 deltaT_ms){
		updateRainEmitter();
		// update droplets
		if (stageWeather.isRaindropEffectActive()){
			dropTimeCounterMS += deltaT_ms;
			if (dropTimeCounterMS > 32){
				f32 rx = (float)rand() / RAND_MAX * 400.0f - 200 + smgr->getActiveCamera()->getTarget().X;
				f32 rz = (float)rand() / RAND_MAX * 400.0f - 200 + smgr->getActiveCamera()->getTarget().Z;
				dropImpactParticleSystem->setPosition(core::vector3df(rx, -40.0f, rz));
				dropTimeCounterMS = 0;
			}
		}
		// update lights
		updateLightGradients(deltaT_ms);
		// update splash emitters
		updateWaterSplashEmitter(deltaT_ms);
		updateSandSplashEmitter(deltaT_ms);
		if (additionalObjectsArray.empty()){
			return;
		}
		int size = additionalObjectsArray.size();
		for (int i = 0; i < size; ++i){
			additionalObjectsArray[i].update(deltaT_ms, additionalObjectsNodes[i]->getAbsolutePosition());
			additionalObjectsNodes[i]->setRotation(additionalObjectsArray[i].getRotation());
		}
	}

	/* set fog */
	void FK_Stage::setFog(){
		stageFog.setFog(driver);
	}

	/* disallow ringout */
	void FK_Stage::disallowRingout(){
		allowsRingoutFlag[FK_PlaneDirections::PlaneNorth] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneWest] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneEast] = false;
		allowsRingoutFlag[FK_PlaneDirections::PlaneSouth] = false;
		ringoutAllowedDirections = 0;
	}

	void FK_Stage::allowRingout(bool north, bool east, bool south, bool west){
		allowsRingoutFlag[FK_PlaneDirections::PlaneNorth] = north;
		allowsRingoutFlag[FK_PlaneDirections::PlaneWest] = west;
		allowsRingoutFlag[FK_PlaneDirections::PlaneEast] = east;
		allowsRingoutFlag[FK_PlaneDirections::PlaneSouth] = south;
		ringoutAllowedDirections =
			east * (u32)FK_PlaneDirections::PlaneEast +
			west *(u32)FK_PlaneDirections::PlaneWest +
			north * (u32)FK_PlaneDirections::PlaneNorth +
			south * (u32)FK_PlaneDirections::PlaneSouth;
	}

	scene::IParticleSystemSceneNode* FK_Stage::getRainEmitter(){
		return rainParticleSystem;
	}

	bool FK_Stage::hasRingoutSplashEffect(){
		return !splashRingoutTextureName.empty();
	}

	void FK_Stage::setWaterSplashEmitter(){
		if (!hasRingoutSplashEffect()){
			return;
		}
		if (!splashingWaterParticleSystem.empty()){
			return;
		}
		for (s32 i = 0; i < 2; ++i){
			IParticleSystemSceneNode* waterSplash = smgr->addParticleSystemSceneNode(false);
			u32 minimumDuration = 100;
			u32 maximumDuration = 2000;
			core::vector3df initialPosition(0, 0, 0);
			core::vector3df direction(0, 1.f, 0);
			f32 radius = 20;
			f32 thickness = 1;
			scene::IParticleEmitter* em = waterSplash->createSphereEmitter(initialPosition, radius, direction);

			waterSplash->setEmitter(em);
			em->drop();
			waterSplash->getEmitter()->setMaxParticlesPerSecond(2500);
			waterSplash->getEmitter()->setMinParticlesPerSecond(1800);
			// add gravity
			scene::IParticleAffector* paf =
				waterSplash->createGravityAffector(core::vector3df(0.00f, -0.5f, 0.0f), 400);

			waterSplash->addAffector(paf);
			paf->drop();
			// add fadeout
			paf =
				waterSplash->createFadeOutParticleAffector();
			((IParticleFadeOutAffector*)paf)->setFadeOutTime(400);
			waterSplash->addAffector(paf);
		    paf->drop();
			// add attractor
			paf =
				waterSplash->createAttractionAffector(initialPosition, 200.0f, false, true, false, true);
			waterSplash->addAffector(paf);
			paf->drop();
			waterSplash->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			waterSplash->setMaterialFlag(video::EMF_LIGHTING, false);
			waterSplash->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
			std::string textureName = path + splashRingoutTextureName;
			waterSplash->setMaterialTexture(0, driver->getTexture(textureName.c_str()));
			splashingWaterParticleSystem.push_back(waterSplash);
			waterSplash = NULL;
			splashingWaterParticleSystemCounters.push_back(-1);
		}
	}
	void FK_Stage::updateWaterSplashEmitter(u32 delta_t_ms){
		if (!hasRingoutSplashEffect()){
			return;
		}
		for (s32 i = 0; i < 2; ++i){
			if (splashingWaterParticleSystemCounters[i] >= 0){
				splashingWaterParticleSystemCounters[i] -= (s32)delta_t_ms;
			}
			else{
				splashingWaterParticleSystemCounters[i] = -1;
				splashingWaterParticleSystem[i]->getEmitter()->setMaxParticlesPerSecond(0);
			}
		}
	}
	void FK_Stage::showWaterSplashEmitter(core::vector3df position, s32 emitterId){
		if (!hasRingoutSplashEffect()){
			return;
		}
		if (emitterId > (s32)splashingWaterParticleSystem.size()){
			return;
		}
		else{
			auto iter = splashingWaterParticleSystem[emitterId]->getAffectors().getLast();
			if ((*iter)->getType() == E_PARTICLE_AFFECTOR_TYPE::EPAT_ATTRACT){
				((IParticleAttractionAffector*)(*iter))->setPoint(position);
			}
			position.Y -= 45;
			//splashingWaterParticleSystem[emitterId]->setVisible(true);
			splashingWaterParticleSystem[emitterId]->getEmitter()->setMaxParticlesPerSecond(2500);
			splashingWaterParticleSystemCounters[emitterId] = 150;
			splashingWaterParticleSystem[emitterId]->setPosition(position);
		}
	}
	scene::IParticleSystemSceneNode* FK_Stage::getWaterSplashEmitter(s32 emitterId){
		if (emitterId > (s32)splashingWaterParticleSystem.size()){
			return NULL;
		}
		else{
			return splashingWaterParticleSystem[emitterId];
		}
	}

	// sand emitter
	bool FK_Stage::hasWalkingSandEffect(){
		return !splashSandTextureName.empty();
	}

	void FK_Stage::setSandSplashEmitter(){
		if (!hasWalkingSandEffect()){
			return;
		}
		if (!splashingSandParticleSystem.empty()){
			return;
		}
		for (s32 i = 0; i < 2; ++i){
			IParticleSystemSceneNode* sandSplash = smgr->addParticleSystemSceneNode(false);
			u32 minimumDuration = 100;
			u32 maximumDuration = 2000;
			core::vector3df initialPosition(0, 0, 0);
			core::vector3df direction(0, 0.4f, 0);
			f32 radius = 15;
			f32 thickness = 1;
			scene::IParticleEmitter* em = sandSplash->createSphereEmitter(initialPosition, radius, direction);

			sandSplash->setEmitter(em);
			em->drop();
			core::vector2df startMaxSize(3.f, 3.f);
			core::vector2df startMinSize(1.f, 1.f);
			sandSplash->getEmitter()->setMaxParticlesPerSecond(800);
			sandSplash->getEmitter()->setMinStartSize(startMinSize);
			sandSplash->getEmitter()->setMaxStartSize(startMaxSize);
			sandSplash->getEmitter()->setMinParticlesPerSecond(200);
			// add gravity
			scene::IParticleAffector* paf =
				sandSplash->createGravityAffector(core::vector3df(0.00f, -0.5f, 0.0f), 200);

			sandSplash->addAffector(paf);
			paf->drop();
			// add fadeout
			paf =
				sandSplash->createFadeOutParticleAffector();
			((IParticleFadeOutAffector*)paf)->setFadeOutTime(50);
			sandSplash->addAffector(paf);
			paf->drop();
			// add attractor
			paf =
				sandSplash->createAttractionAffector(initialPosition, 50.0f, false, true, false, true);
			sandSplash->addAffector(paf);
			paf->drop();
			sandSplash->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			sandSplash->setMaterialFlag(video::EMF_LIGHTING, false);
			sandSplash->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
			std::string textureName = path + splashSandTextureName;
			sandSplash->setMaterialTexture(0, driver->getTexture(textureName.c_str()));
			splashingSandParticleSystem.push_back(sandSplash);
			sandSplash = NULL;
			splashingSandParticleSystemCounters.push_back(-1);
		}
	}
	void FK_Stage::updateSandSplashEmitter(u32 delta_t_ms){
		if (!hasWalkingSandEffect()){
			return;
		}
		for (s32 i = 0; i < 2; ++i){
			if (splashingSandParticleSystemCounters[i] >= 0){
				splashingSandParticleSystemCounters[i] -= (s32)delta_t_ms;
			}
			else{
				splashingSandParticleSystemCounters[i] = -1;
				splashingSandParticleSystem[i]->getEmitter()->setMaxParticlesPerSecond(0);
			}
		}
	}
	void FK_Stage::showSandSplashEmitter(core::vector3df position, s32 emitterId){
		if (!hasWalkingSandEffect()){
			return;
		}
		if (emitterId > (s32)splashingSandParticleSystem.size()){
			return;
		}
		else{
			auto iter = splashingSandParticleSystem[emitterId]->getAffectors().getLast();
			if ((*iter)->getType() == E_PARTICLE_AFFECTOR_TYPE::EPAT_ATTRACT){
				((IParticleAttractionAffector*)(*iter))->setPoint(position);
			}
			//splashingSandParticleSystem[emitterId]->setVisible(true);
			splashingSandParticleSystem[emitterId]->getEmitter()->setMaxParticlesPerSecond(800);
			splashingSandParticleSystemCounters[emitterId] = 150;
			splashingSandParticleSystem[emitterId]->setPosition(position);
		}
	}
	scene::IParticleSystemSceneNode* FK_Stage::getSandSplashEmitter(s32 emitterId){
		if (emitterId > (s32)splashingSandParticleSystem.size()){
			return NULL;
		}
		else{
			return splashingSandParticleSystem[emitterId];
		}
	}


	/* set rain */
	void FK_Stage::setRainEmitter(){
		if (hasActiveWeather){
			/* thanks to user omaremad @irrlicht.sourceforge.net forums! */
			rainParticleSystem = smgr->addParticleSystemSceneNode(false);
			//rainParticleSystemSystem->setParent(smgr->getActiveCamera());
			u32 minimumDuration = 100;
			if (minimumDuration < 2 * stageWeather.getFadingTime()){
				minimumDuration = 2 * stageWeather.getFadingTime();
			}
			u32 maximumDuration = 2000;
			if (maximumDuration < 2 * minimumDuration){
				maximumDuration = 2 * minimumDuration;
			}
			scene::IParticleEmitter* em = rainParticleSystem->createBoxEmitter(
				core::aabbox3d<f32>(-300, 0, -300, 300, 500, 300),
				core::vector3df(0.00f, 0.00f, 0.0f),
				stageWeather.getMinParticleNumber(), stageWeather.getMaxParticleNumber(),
				video::SColor(0, 255, 255, 255), video::SColor(255, 255, 255, 255),
				minimumDuration, maximumDuration, 0,
				stageWeather.getMinParticleSize(), stageWeather.getMaxParticleSize());

			rainParticleSystem->setEmitter(em);
			em->drop();
			// add gravity
			scene::IParticleAffector* paf =
				rainParticleSystem->createGravityAffector(core::vector3df(0.00f, stageWeather.getGravity(), 0.0f), 500);

			rainParticleSystem->addAffector(paf);
			paf->drop();

			// add fader if fading time > 0
			if (stageWeather.getFadingTime() > 0){
				paf = rainParticleSystem->createFadeOutParticleAffector(video::SColor(0, 0, 0, 0), stageWeather.getFadingTime());
				rainParticleSystem->addAffector(paf);
				paf->drop();
			}

			rainParticleSystem->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			rainParticleSystem->setMaterialFlag(video::EMF_LIGHTING, false);
			rainParticleSystem->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
			rainParticleSystem->setMaterialTexture(0, driver->getTexture(stageWeather.getTextureName().c_str()));
		}

		if (stageWeather.isRaindropEffectActive()){
			dropImpactParticleSystem = smgr->addParticleSystemSceneNode(false);
			scene::IParticleEmitter* em = dropImpactParticleSystem->createBoxEmitter(
				core::aabbox3d<f32>(-1, 0, -1, 1, 1, 1),
				core::vector3df(0.00f, 0.001f, 0.0f),
				3000, 5000,
				video::SColor(255, 255, 255, 255), video::SColor(255, 255, 255, 255),
				500, 600, 0,
				core::dimension2df(1.0f, 0.3f), core::dimension2df(3.0f, 0.3f));

			dropImpactParticleSystem->setEmitter(em);
			em->drop();

			// add gravity
			scene::IParticleAffector* paf = dropImpactParticleSystem->createGravityAffector(core::vector3df(0.00f, -0.25f, 0.0f), 300);

			rainParticleSystem->addAffector(paf);
			paf->drop();
			dropImpactParticleSystem->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			dropImpactParticleSystem->setMaterialFlag(video::EMF_LIGHTING, false);
			dropImpactParticleSystem->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
			dropImpactParticleSystem->setMaterialTexture(0, driver->getTexture(stageWeather.getTextureName().c_str()));
		}
	}

	/* update rain emitter*/
	void FK_Stage::updateRainEmitter(){
		if (!hasActiveWeather){
			return;
		}
		rainParticleSystem->setPosition(smgr->getActiveCamera()->getTarget());
		core::vector3df camDir = smgr->getActiveCamera()->getTarget() - smgr->getActiveCamera()->getPosition();
		camDir = camDir.normalize();
		f32 camPitch = camDir.dotProduct(core::vector3df(0, 1, 0));
		if (camPitch < 0.0)
			camPitch = camDir.dotProduct(core::vector3df(0, -1, 0));
		camPitch = (1.0f - camPitch) + 0.001f;
		rainParticleSystem->getEmitter()->setMinStartSize(core::dimension2d<f32>(
			stageWeather.getMinParticleSize().Width, stageWeather.getMinParticleSize().Height*camPitch));
		rainParticleSystem->getEmitter()->setMaxStartSize(core::dimension2d<f32>(
			stageWeather.getMaxParticleSize().Width, stageWeather.getMaxParticleSize().Height*camPitch));
	}

	/* update technicolor */
	void FK_Stage::updateLightGradients(u32 deltaT_ms){
		if (!ambientColorGradient.empty()){
			if (ambientColorGradient.size() > 1){
				ambientGradientTimer += deltaT_ms;
				if (ambientGradientTimer >= ambientColorGradient[ambientGradientIndex].first){
					ambientGradientTimer = 0;
					ambientGradientIndex += 1;
					ambientGradientIndex %= ambientColorGradient.size();
					ambientColor = ambientColorGradient[ambientGradientIndex].second;
				}
				else{
					video::SColor color1 = ambientColorGradient[ambientGradientIndex].second;
					u32 newIndex = (ambientGradientIndex + 1) % ambientColorGradient.size();
					video::SColor color2 = ambientColorGradient[newIndex].second;
					f32 ratio = (f32)ambientGradientTimer /	(f32)ambientColorGradient[ambientGradientIndex].first;
					s32 r1 = color1.getRed();
					s32 r2 = color2.getRed();
					s32 g1 = color1.getGreen();
					s32 g2 = color2.getGreen();
					s32 b1 = color1.getBlue();
					s32 b2 = color2.getBlue();
					u32 red = (u32)(r1 + (s32)std::ceil((f32)(r2 - r1)*ratio));
					u32 blue = (u32)(b1 + (s32)std::ceil((f32)(b2 - b1)*ratio));
					u32 green = (u32)(g1 + (s32)std::ceil((f32)(g2 - g1)*ratio));
					//std::cout <<"amb: "<< red << " " << green << " " << blue << " "<< ratio <<std::endl;
					ambientColor = video::SColor(255, red, green, blue);
				}
			}
			else{
				ambientColor = ambientColorGradient[0].second;
			}
		}
		if (!lightColorGradient.empty()){
			if (lightColorGradient.size() > 1){
				lightGradientTimer += deltaT_ms;
				if (lightGradientTimer >= lightColorGradient[lightGradientIndex].first){
					lightGradientTimer = 0;
					lightGradientIndex += 1;
					lightGradientIndex %= lightColorGradient.size();
					lightColor = lightColorGradient[lightGradientIndex].second;
				}
				else{
					video::SColor color1 = lightColorGradient[lightGradientIndex].second;
					u32 newIndex = (lightGradientIndex + 1) % lightColorGradient.size();
					video::SColor color2 = lightColorGradient[newIndex].second;
					f32 ratio = (f32)lightGradientTimer / (f32)lightColorGradient[lightGradientIndex].first;
					s32 r1 = color1.getRed();
					s32 r2 = color2.getRed();
					s32 g1 = color1.getGreen();
					s32 g2 = color2.getGreen();
					s32 b1 = color1.getBlue();
					s32 b2 = color2.getBlue();
					u32 red = (u32)(r1 + (s32)std::ceil((f32)(r2 - r1)*ratio));
					u32 blue = (u32)(b1 + (s32)std::ceil((f32)(b2 - b1)*ratio));
					u32 green = (u32)(g1 + (s32)std::ceil((f32)(g2 - g1)*ratio));
					lightColor = video::SColor(255, red, green, blue);
				}
			}
			else{
				lightColor = lightColorGradient[0].second;
			}
		}
	}
}