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