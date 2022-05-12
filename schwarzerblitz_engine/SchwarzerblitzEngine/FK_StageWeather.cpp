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

#include"FK_StageWeather.h"

using namespace irr;

namespace fk_engine{

	FK_StageWeather::FK_StageWeather(){
		m_maxParticleNumber = 1;
		m_minParticleNumber = 1;
		m_maxSize = core::dimension2df(0.1f, 0.1f);
		m_minSize = core::dimension2df(0.1f, 0.1f);
		m_textureName = std::string();
		m_raindrop = false;
		m_raindropTexture = std::string();
		m_fadingTime = 0;
		m_gravity = -0.25f;
	}
	FK_StageWeather::FK_StageWeather(std::string textureName, u32 minParticleNumber, u32 maxParticleNumber,
		core::dimension2df minSize, core::dimension2df maxSize, f32 gravity, u32 fadingTime) : FK_StageWeather(){
		m_textureName = textureName;
		m_minParticleNumber = minParticleNumber;
		m_maxParticleNumber = maxParticleNumber;
		m_minSize = minSize;
		m_maxSize = maxSize;
		m_gravity = gravity;
		m_fadingTime = fadingTime;
	}
	std::string FK_StageWeather::getTextureName(){
		return m_textureName;
	}
	u32 FK_StageWeather::getMinParticleNumber(){
		return m_minParticleNumber;
	}
	u32 FK_StageWeather::getMaxParticleNumber(){
		return m_maxParticleNumber;
	}
	core::dimension2df FK_StageWeather::getMinParticleSize(){
		return m_minSize;
	}
	core::dimension2df FK_StageWeather::getMaxParticleSize(){
		return m_maxSize;
	}
	f32 FK_StageWeather::getGravity(){
		return m_gravity;
	}
	u32 FK_StageWeather::getFadingTime(){
		return m_fadingTime;
	}
	std::string FK_StageWeather::getRaindropTextureName(){
		return m_raindropTexture;
	}
	bool FK_StageWeather::isRaindropEffectActive(){
		return m_raindrop;
	}
	void FK_StageWeather::setRaindropEffect(std::string newTextureName){
		m_raindrop = true;
		m_raindropTexture = newTextureName;
	}
};