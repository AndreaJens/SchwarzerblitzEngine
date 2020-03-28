#ifndef FK_SCENECHARASELECT_EXTRA_ARCADE_ENDING_CLASS
#define FK_SCENECHARASELECT_EXTRA_ARCADE_ENDING_CLASS

#include "FK_SceneCharacterSelectExtra.h"
#include "FK_StoryFlowCluster.h"

using namespace irr;

namespace fk_engine {
	class FK_SceneCharacterSelectExtra_ArcadeEnding : public FK_SceneCharacterSelectExtra {
	public:
		FK_SceneCharacterSelectExtra_ArcadeEnding();
		FK_Scene::FK_SceneExtraType getExtraSceneType();
		FK_StoryEvent getCutsceneToPlay();
	protected:
		void loadCharacterList();
		bool checkIfEndingIsAvailable(std::string& characterPath);
		void loadSingleCharacterOutfitList(u32 characterId, FK_Character* character);
	private:
		std::map<u32, u32> availableEndingsIndexMap;
	};
}
#endif