#include "FK_PixelShaderStageCallback.h"
#include "FK_SceneGame.h"

namespace fk_engine {

	FK_PixelShaderStageCallback::FK_PixelShaderStageCallback() {
		materialType = 0;
		scene = NULL;
		mTimeMs = new f32[1];
	};


	FK_PixelShaderStageCallback::~FK_PixelShaderStageCallback() {
		materialType = 0;
		scene = NULL;
		delete mTimeMs;
	};

	FK_PixelShaderStageCallback::FK_PixelShaderStageCallback(irr::s32 new_materialType, FK_SceneGame* currentScene, FK_Stage::FK_StagePixelShader stageShader) {
		scene = currentScene;
		materialType = new_materialType;
		shader = stageShader;
		mTimeMs = new f32[1];
	};
	void FK_PixelShaderStageCallback::OnPreRender(EffectHandler* effect) {
		for each(auto param in shader.parameters) {
			if (param == FK_Stage::FK_StagePixelShader::FK_StagePixelShaderParameters::timeMs) {
				mTimeMs[0] = (f32)(scene->getIrrlichtDevice()->getTimer()->getTime());
				effect->setPostProcessingEffectConstant(materialType, "mTimeMs", mTimeMs, 1);
			}
		}
	};
	void FK_PixelShaderStageCallback::OnPostRender(EffectHandler* effect) {

	};
};
