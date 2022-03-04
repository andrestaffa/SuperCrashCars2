#pragma once

#include <fmod.hpp>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Log.h"

//#define SOUND_WOOF "./res/sounds/environmentSounds/woof.ogg"

// music
#define BGM_CLOUDS "audio/bgm/clouds.wav"

// sfx
#define SFX_MENUBUTTON "audio/sfx/buttonclick.wav"

#define SFX_CARWINDUP "audio/carsounds/car_long/windup.wav"
#define SFX_CAR_FAST "audio/carsounds/car_long/maxspeed.wav"
#define	SFX_CARWINDDOWN "audio/carsounds/car_long/winddown.wav"

#define SFX_CAR_HIT "audio/sfx/hit.wav"


class AudioManager
{
public:

	// START SINGLETON STUFF
	static AudioManager& get() {
		static AudioManager instance;
		return instance;
	}

	// these are all needed to make sure we don't get copies of the renderer singleton
	AudioManager(AudioManager const&) = delete;
	void operator=(AudioManager const&) = delete;
	// END SINGLETON STUFF

	void init();
	void playBackgroundMusic(std::string filePath);
	void updateBackgroundChannelVolume();
	void loadBackgroundSound(std::string filePath);
	void playSound(std::string soundName, float soundVolume);
	void playSound(std::string soundName, glm::vec3 position, float soundVolume);
	void adjustVolume(float dVolume);
	void setVolume(float newVolume);
	void muteToggle();

	bool getMuteStatus();

	void setListenerPosition(glm::vec3 position, glm::vec3 forward, glm::vec3 up);

	FMOD::System* system;
	
	// The master volume
	float volume, unmutedVolume;


private:
	AudioManager() {}

	std::unordered_map<std::string, FMOD::Sound*> mSounds;

	FMOD::Channel* backgroundChannel;

	void loadSound(std::string filePath);
	bool muted;
	
};

