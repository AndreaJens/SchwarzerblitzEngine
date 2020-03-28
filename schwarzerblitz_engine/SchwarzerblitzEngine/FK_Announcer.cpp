#include "FK_Announcer.h"

namespace fk_engine{
	// constructor for the announcer
	FK_Announcer::FK_Announcer(std::string default_voice_directory){
		defaultVoicePath = default_voice_directory;
	};
	// destructor
	FK_Announcer::~FK_Announcer(){
		soundPlayer.stop();
		voiceClips.clear();
	};
	// add sound buffer to the announcer library (file from default path)
	bool FK_Announcer::addSoundFromDefaultPath(std::string identifier, std::string filename){
		return addSoundFullPath(identifier, defaultVoicePath + filename);
	};
	// add a sound from a specific path
	bool FK_Announcer::addSoundFullPath(std::string identifier, std::string filename){
		// if no file with the same identifier has been added, add the buffer to the library
		if (voiceClips.count(identifier) == 0){
			sf::SoundBuffer buffer;
			bool successFlag = buffer.loadFromFile(filename);
			if (successFlag){
				voiceClips[identifier] = buffer;
			}
			return successFlag;
		}
		return false;
	}
	// play sound if no sound is playing
	bool FK_Announcer::playSound(std::string identifier, float sfx_volume, bool forcePlay){
		// if a clip is already playing, return false
		if (soundPlayer.getStatus() == sf::SoundSource::Status::Playing){
			if (forcePlay){
				soundPlayer.stop();
			}
			else{
				return false;
			}
		}
		// if there is no file in the buffer, return false
		if (voiceClips.count(identifier) == 0){
			return false;
		}
		//set buffer to the sound player
		soundPlayer.setBuffer(voiceClips[identifier]);
		soundPlayer.setVolume(sfx_volume);
		soundPlayer.play();
		return true;
	}
}