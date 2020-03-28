#include "FK_StageFog.h"

using namespace irr;

namespace fk_engine{
	FK_StageFog::FK_StageFog(){
		m_fogType = video::E_FOG_TYPE::EFT_FOG_LINEAR;
		m_color = video::SColor(128, 0, 0, 0);
		m_rangeMin = 50.f;
		m_rangeMax = 500.f;
		m_density = 0.1f;
	}
	FK_StageFog::FK_StageFog(video::SColor color, video::E_FOG_TYPE type, f32 rangeMin, f32 rangeMax, f32 density){
		m_fogType = type;
		m_color = color;
		m_rangeMin = rangeMin;
		m_rangeMax = rangeMax;
		m_density = density;
	}
	void FK_StageFog::setFog(video::IVideoDriver*& driver){
		driver->setFog(m_color, m_fogType, m_rangeMin, m_rangeMax, m_density, false, true);
	};
}