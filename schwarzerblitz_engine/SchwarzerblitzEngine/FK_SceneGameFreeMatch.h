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
    the SFML code and binaries are subject to their licenses and are *not* released
    under GNU General Public License, version 3 or higher - they instead retain their
    own license.

    =============== Assets and resources ================
    Unless specificed otherwise in the Credits file, the assets and resources
    bundled with this engine are to be considered "all rights reserved" and
    cannot be redistributed without the owner's consent. This includes but it is
    not limited to the characters concepts / designs, the 3D models, the music,
    the sound effects, 2D and 3D illustrations, stages, icons, menu art.

    Tutorial Man, Evil Tutor, and Random:
    Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

#ifndef FK_SCENEGAMEFREEMATCH_CLASS
#define FK_SCENEGAMEFREEMATCH_CLASS

#include "FK_SceneGame.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{

	class FK_SceneGameFreeMatch : public FK_SceneGame {
	protected:
		// methods to be overloaded from subclasses
		virtual void processMatchEnd();
	public:
		FK_SceneGameFreeMatch();
		FK_Scene::FK_SceneFreeMatchType getFreeMatchType();
		void setFreeMacthType(FK_SceneFreeMatchType type);
	protected:
		void processEndOfRoundStatistics();
		void updateAdditionalSceneLogic(u32 delta_t_ms);
		void drawAdditionalHUDItems(f32 delta_t_s);
		void setupAdditionalParametersAfterLoading();
		void restartMatch();
		// end match menu
		void setupEndMatchMenu();
		void updateEndMatchMenu();
		void activateEndMatchMenu();
		void disableEndMatchMenu();
	private:
		FK_Scene::FK_SceneFreeMatchType freeMatchType;
		FK_InGameMenu* endMatchMenu;
	};
}
#endif