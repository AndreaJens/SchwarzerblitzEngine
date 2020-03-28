#ifndef FK_STAGEFOG_CLASS
#define FK_STAGEFOG_CLASS

#include<irrlicht.h>

using namespace irr;

namespace fk_engine{
	class FK_StageFog{
	public:
		FK_StageFog();
		FK_StageFog(video::SColor color, video::E_FOG_TYPE type, f32 rangeMin, f32 rangeMax, f32 density);
		void setFog(video::IVideoDriver*& driver);
	private:
		video::SColor m_color;
		video::E_FOG_TYPE m_fogType;
		f32 m_rangeMin;
		f32 m_rangeMax;
		f32 m_density;
	};
};

#endif