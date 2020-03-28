#ifndef FK_SCENEOPTIONSEMBEDDED_CLASS
#define FK_SCENEOPTIONSEMBEDDED_CLASS

#include "FK_SceneOptions.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine {
	class FK_SceneOptionsEmbedded : public FK_SceneOptions {
	public:
		void drawAll();
	protected:
		void setupMenuItems();
		void drawBackground();
		void drawCaptions();
		void setupBGM();
	};
}

#endif