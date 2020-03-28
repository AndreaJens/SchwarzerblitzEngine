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