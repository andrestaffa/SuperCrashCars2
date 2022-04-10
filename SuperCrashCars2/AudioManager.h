#pragma once

#include <fmod.hpp>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Log.h"
#include "Utils.h"
//#include "PVehicle.h"


// music
#define BGM_CLOUDS "audio/bgm/clouds.wav"

// sfx
#define SFX_MENUBUTTON "audio/sfx/buttonclick.wav"
#define SFX_CONTROLLER_ON "audio/sfx/controller_on.wav"
#define SFX_CONTROLLER_OFF "audio/sfx/controller_off.wav"
#define SFX_INCREMENT "audio/sfx/increment.wav"

#define SFX_CAR_IDLE "audio/carsounds/car_long/idle.wav"
#define SFX_CARWINDUP "audio/carsounds/car_long/windup.wav"
#define SFX_CAR_FAST "audio/carsounds/car_long/loop.wav"
#define	SFX_CARWINDDOWN "audio/carsounds/car_long/winddown.wav"

#define SFX_CAR_HIT "audio/sfx/hit.wav"
#define	SFX_ITEM_COLLECT "audio/sfx/item.wav"
#define SFX_DEATH "audio/sfx/death.wav"
#define SFX_JUMP_NORMAL "audio/sfx/jump.wav"
#define	SFX_JUMP_MEGA "audio/sfx/megajump.wav"

class PVehicle;

enum class CarAudioState {
	eIDLE,
	eACCELERATING,
	eLOOP,
	eDECELERATING
};

class AudioManager{

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

	void init(std::vector<PVehicle*>& vehicleList);
	void playBackgroundMusic(std::string filePath);
	void refreshBGMVolume();
	void incrementBGMVolume(int sign);
	void incrementSFXVolume(int sign);
	void loadBackgroundSound(std::string filePath);
	void loadCarSound(std::string filePath, bool looping);
	void playSound(std::string soundName, float soundVolume);
	void playSound(std::string soundName, glm::vec3 position, float soundVolume);
	void setMasterVolume(float newVolume);
	void setBGMVolume(float newVolume);
	void setSFXVolume(float newVolume);
	void toggleBGMMute();
	void toggleSFXMute();

	bool getBGMMute();
	bool getSFXMute();
	int getBGMLevel();
	int getSFXLevel();

	void update();

	void setListenerPosition(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	

	FMOD::System* system;
	

	void startCarSounds();
	void updateCarSounds();
	void setCarSoundsPause(bool pause);

private:
	AudioManager() {}

	std::unordered_map<std::string, FMOD::Sound*> mSounds;

	FMOD::Channel* backgroundChannel;
	float masterVolume, BGMVolume, SFXVolume, unmutedVolume;

	void loadSound(std::string filePath);



	float clampVol(float vol);

	bool muted, mutedSFX, mutedBGM;
	const float BGM_VOL_INIT = 0.2f;
	const float POSITION_SCALING = 0.08f;
	const float CAR_SOUNDS_VOLUME = 0.1f;

	FMOD::Channel* carChannels[4]; // because of this, max out at 4 cars.
	CarAudioState audioState[4]; // corresponds to the carids

	std::vector<PVehicle*> m_vehicleList;
	
};

