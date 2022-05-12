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

#ifndef FK_STAGE_CLASS
#define FK_STAGE_CLASS

#include<irrlicht.h>
#include"FK_Database.h"
#include"FK_StageAdditionalObject.h"
#include"FK_StageFog.h"
#include"FK_StageWeather.h"
#include<string>
#include<map>
#include<vector>

using namespace irr;

namespace fk_engine{
	class FK_Stage{
	public:
		struct FK_Stage_SFX {
			std::string filename = std::string();
			f32 volume = 100.f;
			f32 pitch = 1.f;
		};
		struct FK_Stage_ShaderParameters{
			f32 brightnessCutLow = 10.0f;
			f32 brightnessCutHigh = -0.1f;
			f32 bloomValue = 0.5f;
			f32 sceneValue = 0.9f;
		};
		struct FK_StagePixelShader {
			enum FK_StagePixelShaderParameters {
				characterPositionsXY,
				timerValue,
				timeMs,
			};
			std::string filename = std::string();
			std::vector<FK_StagePixelShaderParameters> parameters = std::vector<FK_StagePixelShaderParameters>();
		};
	public:
		// default constructor
		FK_Stage();
		// advanced constructor
		FK_Stage(IrrlichtDevice* new_device, video::IVideoDriver* driver, scene::ISceneManager* new_smgr, std::string new_path,
			std::string player1Path = std::string(), std::string player2Path = std::string());
		// setup is usually called when a multi-threading is needed. Setup is called as weel by the constructor
		void setup(IrrlichtDevice* new_device, video::IVideoDriver* driver, scene::ISceneManager* new_smgr, std::string new_path,
			std::string player1Path = std::string(), std::string player2Path = std::string());
		// destructor
		~FK_Stage();
		// bool which returns true if the stage has been loaded
		bool hasBeenLoaded();
		scene::ISceneNode* getStage();
		scene::ISceneNode* getStageWaterNode();
		scene::ISceneNode* getSkybox();
		std::vector<scene::ISceneNode*> & getAdditionalNodes();
		scene::IAnimatedMesh* getStageMesh();
		scene::IAnimatedMesh* getWaterMesh();
		std::wstring getName();
		std::wstring getDescription();
		std::string getBGMName();
		video::SColor getAmbientColor();
		video::SColor getEmissionColor();
		video::SColor getLightColor();
		video::SColor getBackgroundColor();
		f32 getRingoutDepth();
		f32 getBGMVolume();
		f32 getBGMPitch();
		f32 getBGMPauseVolume();
		f32 getBGMTriggerVolume();
		core::plane3d<f32> getWallboxPlane(FK_PlaneDirections direction);
		core::plane3d<f32> getComplexWallBoxPlane(u32 index);
		u32 getNumberofPlanesOfComplexWallBox();
		core::plane3d<f32> getComplexRingoutBoxPlane(u32 index);
		u32 getNumberofPlanesOfComplexRingoutBox();
		bool hasComplexShape();
		bool allowsRingout();
		bool allowsRingout(FK_PlaneDirections direction);
		bool allowsRingout(u32 direction);
		void toggleVisibility(bool new_visibility);
		core::aabbox3d<f32> getWallbox();
		core::aabbox3d<f32> getRingoutbox();
		void update(u32 deltaT_ms = 0);
		void setFog();
		void disallowRingout();
		void allowRingout(bool north, bool east, bool south, bool west);
		void setRainEmitter();
		void updateRainEmitter();
		scene::IParticleSystemSceneNode* getRainEmitter();
		bool hasRingoutSplashEffect();
		void setWaterSplashEmitter();
		void updateWaterSplashEmitter(u32 delta_t_ms);
		void showWaterSplashEmitter(core::vector3df position, s32 emitterId = 0);
		scene::IParticleSystemSceneNode* getWaterSplashEmitter(s32 emitterId = 0);
		bool hasWalkingSandEffect();
		void setSandSplashEmitter();
		void updateSandSplashEmitter(u32 delta_t_ms);
		void showSandSplashEmitter(core::vector3df position, s32 emitterId = 0);
		scene::IParticleSystemSceneNode* getSandSplashEmitter(s32 emitterId = 0);
		bool hasAvailableSkybox();
		FK_Stage_ShaderParameters getParametersForShader();
		const std::vector<FK_StagePixelShader> &getAdditionalPixelShaders() const;
		FK_Stage_SFX getSplashSoundEffect();
		std::vector<std::string> getDioramaLoreText();
		std::pair<std::wstring, std::wstring> getBGMMetaData();
		void getCenterStartingPosition(core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition);
		void getRightPlayerAtWallPosition(core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition);
		void getLeftPlayerAtWallPosition(core::vector3df& leftPlayerPosition, core::vector3df& rightPlayerPosition);
	protected:
		void prepareKeysForFileLoad();
		void loadStageFromFile(std::string player1Path = std::string(), std::string player2Path = std::string());
		//void loadStageStories();
		void updateLightGradients(u32 deltaT_ms);
		void setNode();
		void setWaterNode();
		void setAdditionalObjects();
		void setSkybox();
		void createWallboxPlanes();
		void createComplexWallboxPlanes();
		void createComplexRingoutBoxPlanes();
	private:
		IrrlichtDevice* device;
		video::IVideoDriver* driver;
		scene::ISceneManager* smgr;
		scene::IAnimatedMesh* mesh;
		scene::IAnimatedMesh* waterMesh;
		scene::ISceneNode* node;
		scene::ISceneNode* waterNode;
		scene::ISceneNode* skybox;
		core::vector3df scale;
		core::vector3df position;
		std::string path;
		std::string meshName;
		std::string archiveName;
		std::string name;
		std::string bgmName;
		std::string description;
		std::wstring wdescription;
		std::wstring wname;
		std::string waterMeshName;
		core::vector3df waterNodePosition;
		core::vector3df waterNodeCharacteristics;
		video::SColor ambientColor;
		video::SColor emissionColor;
		video::SColor lightColor;
		video::SColor backgroundColor;
		bool hasSkybox;
		bool isZipped;
		f32 basicBGMVolume;
		f32 basicBGMPitch;
		f32 ringoutDepth;
		std::map<FK_StageFileKeys, std::string> fileKeys;
		// create array to store the introduction stories to the stage
		//std::map<std::pair<std::string, std::string>, std::vector<std::string> > stageStories;
		// texture effect to apply to each material
		video::E_MATERIAL_TYPE textureEffect;
		// wireframe effect
		bool stageWireframeFlag;
		bool stageFogEffectFlag;
		// boolean used during scene loading (true only if the file has been loaded)
		bool m_thread_loaded;
		// wallbox (i.e. arena limits for ringout)
		core::aabbox3d<f32> wallbox;
		// corner box (stores corners for training mode)
		core::aabbox3d<f32> cornerbox;
		// ringoutbox (a smaller box which tells if the player can be sent ringout from here)
		core::aabbox3d<f32> ringoutbox;
		// check whether stage allow ringout or not
		std::map<FK_PlaneDirections, bool> allowsRingoutFlag;
		// manage complex ring shapes
		std::vector<core::vector3df> complexWallBoxEdges;
		std::vector<core::vector3df> complexRingOutBoxEdges;
		std::vector<bool> complexAllowsRingoutFlag;
		bool hasComplexStageShape;
		u32 ringoutAllowedDirections;
		// additional scenario objects
		std::vector<FK_StageAdditionalObject> additionalObjectsArray;
		std::vector<scene::ISceneNode*> additionalObjectsNodes;
		// store wallbox planes in a map
		std::map<FK_PlaneDirections, core::plane3d<f32> > wallboxPlanes;
		std::map<u32, core::plane3d<f32> > complexWallBoxPlanes;
		std::map<u32, core::plane3d<f32> > complexRingoutBoxPlanes;
		// fog details
		FK_StageFog stageFog;
		// rain
		bool hasActiveWeather;
		FK_StageWeather stageWeather;
		scene::IParticleSystemSceneNode* rainParticleSystem;
		scene::IParticleSystemSceneNode* dropImpactParticleSystem;
		std::vector<scene::IParticleSystemSceneNode*> splashingWaterParticleSystem;
		std::vector<s32> splashingWaterParticleSystemCounters;
		std::vector<scene::IParticleSystemSceneNode*> splashingSandParticleSystem;
		std::vector<s32> splashingSandParticleSystemCounters;
		u32 dropTimeCounterMS;
		std::vector<std::pair<u32, video::SColor> > ambientColorGradient;
		std::vector<std::pair<u32, video::SColor> > lightColorGradient;
		u32 ambientGradientIndex;
		u32 lightGradientIndex;
		u32 ambientGradientTimer;
		u32 lightGradientTimer;
		std::string splashRingoutTextureName;
		std::string splashSandTextureName;
		FK_Stage_ShaderParameters shaderParameters;
		std::vector<FK_StagePixelShader> additionalPixelShaders;
		FK_Stage_SFX splashSoundEffect;
		std::vector<std::string> dioramaLoreLines;
		std::pair<std::wstring, std::wstring> bgmMetadata;
	};
}

#endif