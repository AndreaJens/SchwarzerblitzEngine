#ifndef FK_STAGEWEATHER_CLASS
#define FK_STAGEWEATHER_CLASS

#include<irrlicht.h>
#include<string>

using namespace irr;

namespace fk_engine{
	class FK_StageWeather{
	public:
		FK_StageWeather();
		FK_StageWeather(std::string textureName, u32 minParticleNumber, u32 maxParticleNumber,
			core::dimension2df minSize, core::dimension2df maxSize, f32 gravity, u32 fadingTime);
		std::string getTextureName();
		u32 getMinParticleNumber();
		u32 getMaxParticleNumber();
		core::dimension2df getMinParticleSize();
		core::dimension2df getMaxParticleSize();
		f32 getGravity();
		u32 getFadingTime();
		std::string getRaindropTextureName();
		bool isRaindropEffectActive();
		void setRaindropEffect(std::string newTextureName);
	private:
		core::dimension2df m_minSize;
		core::dimension2df m_maxSize;
		std::string m_textureName;
		u32 m_minParticleNumber;
		u32 m_maxParticleNumber;
		f32 m_gravity;
		u32 m_fadingTime;
		bool m_raindrop;
		std::string m_raindropTexture;
	};
};

#endif