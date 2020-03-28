#ifndef FK_PIXELSHADERSTAGE_CALLBACKWRAPPER
#define FK_PIXELSHADERSTAGE_CALLBACKWRAPPER
#include "FK_Stage.h"

#include "EffectHandler.h"

namespace fk_engine {
	class FK_SceneGame;

	class FK_PixelShaderStageCallback : public IPostProcessingRenderCallback
	{
	public:
		FK_PixelShaderStageCallback();
		~FK_PixelShaderStageCallback();
		FK_PixelShaderStageCallback(irr::s32 new_materialType, FK_SceneGame* currentScene, FK_Stage::FK_StagePixelShader stageShader);
		virtual void OnPreRender(EffectHandler* effect);
		virtual void OnPostRender(EffectHandler* effect);
	private:
		FK_SceneGame* scene;
		FK_Stage::FK_StagePixelShader shader;
		irr::s32 materialType;
		f32* mTimeMs;
	};
}

#endif