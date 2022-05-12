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
