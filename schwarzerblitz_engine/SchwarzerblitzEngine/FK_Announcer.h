#ifndef FK_ANNOUNCER_CLASS
#define FK_ANNOUNCER_CLASS

#include <SFML\Audio.hpp>
#include<string>
#include<map>

namespace fk_engine{
	class FK_Announcer{
	public:
		FK_Announcer(std::string default_voice_directory);
		~FK_Announcer();
		bool addSoundFromDefaultPath(std::string identifier, std::string filename);
		bool addSoundFullPath(std::string identifier, std::string filename);
		bool playSound(std::string identifier, float volume = 100.0f, bool forcePlay = false);
	private:
		std::string defaultVoicePath;
		std::map<std::string, sf::SoundBuffer> voiceClips;
		sf::Sound soundPlayer;
	};
}

#endif