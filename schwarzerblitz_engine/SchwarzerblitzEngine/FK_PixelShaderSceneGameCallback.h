#ifndef FK_PIXELSHADERSEPIA_CALLBACKWRAPPER
#define FK_PIXELSHADERSEPIA_CALLBACKWRAPPER

#include "EffectHandler.h"

namespace fk_engine{
	class FK_SceneWithInput;

	const enum FK_SceneShaderTypes{
		ShaderNeutral,
		ShaderSepia,
		ShaderRipple,
		ShaderZoomBlur,
		ShaderBloom,
		ShaderBrightPass,
	};

	class FK_PixelShaderSceneGameCallback : public IPostProcessingRenderCallback
	{
	public:
		FK_PixelShaderSceneGameCallback();
		FK_PixelShaderSceneGameCallback(irr::s32 new_materialType, FK_SceneWithInput* currentScene, FK_SceneShaderTypes newShaderType);
		virtual void OnPreRender(EffectHandler* effect);
		virtual void OnPostRender(EffectHandler* effect);
	private:
		FK_SceneWithInput* scene;
		irr::s32 materialType;
		FK_SceneShaderTypes shaderType;
	};
}

#endif