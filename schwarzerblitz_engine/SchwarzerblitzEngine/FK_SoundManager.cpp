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

#include "FK_SoundManager.h"

namespace fk_engine{
	// constructor for the announcer
	FK_SoundManager::FK_SoundManager(std::string default_sound_directory){
		defaultSoundPath = default_sound_directory;
		int soundArraySize = 10;
		for (int i = 0; i < soundArraySize; ++i){
			soundPlayer[i] = sf::Sound();
		}
	};
	// destructor
	FK_SoundManager::~FK_SoundManager(){
		int soundArraySize = 10;
		for (int i = 0; i < soundArraySize; ++i){
			soundPlayer[i].stop();
		}
		soundClips.clear();
	};
	// add sound buffer to the announcer library (file from default path)
	bool FK_SoundManager::addSoundFromDefaultPath(std::string identifier, std::string filename){
		return addSoundFullPath(identifier, defaultSoundPath + filename);
	};
	// add a sound from a specific path
	bool FK_SoundManager::addSoundFullPath(std::string identifier, std::string filename){
		// if no file with the same identifier has been added, add the buffer to the library
		if (soundClips.count(identifier) == 0){
			sf::SoundBuffer buffer;
			bool successFlag = buffer.loadFromFile(filename);
			if (successFlag){
				soundClips[identifier] = buffer;
			}
			return successFlag;
		}
		return false;
	}
	// check if has recorded sound
	bool FK_SoundManager::hasSound(std::string identifier){
		// if there is no file in the buffer, return false
		if (soundClips.count(identifier) > 0){
			return true;
		}
		return false;
	}

	// play sound
	bool FK_SoundManager::playSound(std::string identifier, float sfx_volume, float sfx_pitch){
		// if there is no file in the buffer, return false
		if (soundClips.count(identifier) == 0){
			return false;
		}
		// find the first free sound player and play the sound
		int soundArraySize = 10;
		for (int i = 0; i < soundArraySize; ++i){
			if (soundPlayer[i].getStatus() == sf::SoundSource::Status::Playing){
				continue;
			}
			//set buffer to the sound player
			soundPlayer[i].setBuffer(soundClips[identifier]);
			soundPlayer[i].setVolume(sfx_volume);
			soundPlayer[i].setPitch(sfx_pitch);
			soundPlayer[i].play();
			return true;
		}
		return false;
	}
}