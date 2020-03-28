#ifndef FK_BATTLE_SOUND_MANAGER_CLASS
#define FK_BATTLE_SOUND_MANAGER_CLASS

#include <SFML\Audio.hpp>
#include<string>
#include<map>

namespace fk_engine{
	class FK_SoundManager{
	public:
		FK_SoundManager(std::string default_voice_directory);
		~FK_SoundManager();
		bool addSoundFromDefaultPath(std::string identifier, std::string filename);
		bool addSoundFullPath(std::string identifier, std::string filename);
		bool playSound(std::string identifier, float volume = 100.0f, float pitch = 1.0f);
		bool hasSound(std::string identifier);
	private:
		std::string defaultSoundPath;
		std::map<std::string, sf::SoundBuffer> soundClips;
		sf::Sound soundPlayer[10];
	};
}

#endif