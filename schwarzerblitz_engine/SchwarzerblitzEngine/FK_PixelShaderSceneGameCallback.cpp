#include "FK_PixelShaderSceneGameCallback.h"
#include "FK_SceneGame.h"

namespace fk_engine{

	FK_PixelShaderSceneGameCallback::FK_PixelShaderSceneGameCallback(){
		materialType = 0;
		scene = NULL;
		shaderType = FK_SceneShaderTypes::ShaderNeutral;
	};

	FK_PixelShaderSceneGameCallback::FK_PixelShaderSceneGameCallback(irr::s32 new_materialType, FK_SceneWithInput* currentScene,
		FK_SceneShaderTypes newShaderType){
		scene = currentScene;
		materialType = new_materialType;
		shaderType = newShaderType;
	};
	void FK_PixelShaderSceneGameCallback::OnPreRender(EffectHandler* effect){
		f32* activeEffect = scene->getSceneParametersForShader(shaderType);
		u32 dataSize = 1;
		if (shaderType == FK_SceneShaderTypes::ShaderRipple ||
			shaderType == FK_SceneShaderTypes::ShaderZoomBlur){
			dataSize = 4;
		}
		else if (shaderType == FK_SceneShaderTypes::ShaderBloom ||
			shaderType == FK_SceneShaderTypes::ShaderBrightPass){
			dataSize = 4;
		}
		effect->setPostProcessingEffectConstant(materialType, "mEffectParameters", activeEffect, dataSize);
	};
	void FK_PixelShaderSceneGameCallback::OnPostRender(EffectHandler* effect){

	};
};
