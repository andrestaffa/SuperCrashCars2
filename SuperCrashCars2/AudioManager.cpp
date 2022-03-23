#include "AudioManager.h"
#include <fmod_errors.h>

void AudioManager::init() {
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);

	if (result != FMOD_OK) {
		Log::error("Failed to create FMOD system");
	}

	result = system->init(2048, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK) {
		Log::error("Failed to initialize FMOD system");
	}

	this->BGMVolume = BGM_VOL_INIT * 1.f;
	this->SFXVolume = 1.0f;
	this->masterVolume = 0.6f;
	this->muted = false;
	this->mutedBGM = false;
	this->mutedSFX = false;

	loadBackgroundSound(BGM_CLOUDS);

	loadSound(SFX_MENUBUTTON);
	loadSound(SFX_CAR_HIT);
	loadSound(SFX_ITEM_COLLECT);
	loadSound(SFX_JUMP_NORMAL);
	loadSound(SFX_JUMP_MEGA);
	loadSound(SFX_DEATH);


	setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	playBackgroundMusic(BGM_CLOUDS);
}


void AudioManager::playBackgroundMusic(std::string filePath)
{
	FMOD::Sound* sound = mSounds[filePath.c_str()];

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &backgroundChannel);
	result = backgroundChannel->setVolume(this->masterVolume * BGMVolume * (float)(!mutedBGM));
	result = backgroundChannel->setPaused(false);
}


void AudioManager::loadBackgroundSound(std::string filePath)
{
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &sound);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}

	// save sound pointer to map
	mSounds[filePath] = sound;
}

void AudioManager::loadSound(std::string filePath) {
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_3D, nullptr, &sound);

	FMOD::SoundGroup* sounds;
	system->createSoundGroup("sounds", &sounds);
	sound->setSoundGroup(sounds);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}

	// save sound pointer to map
	mSounds[filePath] = sound;
}

void AudioManager::playSound(std::string soundName, float soundVolume) {
	FMOD::Sound* sound = mSounds[soundName];

	FMOD::Channel* channel;

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &channel);
	result = channel->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * soundVolume);
	result = channel->setPaused(false);
}

void AudioManager::playSound(std::string soundName, glm::vec3 position, float soundVolume) {
	FMOD::Sound* sound = mSounds[soundName];

	FMOD::Channel* channel;

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &channel);

	// set position
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	channel->set3DAttributes(&fmodPos, nullptr);

	result = channel->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * soundVolume);
	result = channel->setPaused(false);
}

void AudioManager::setListenerPosition(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	FMOD_VECTOR fmodForward = {
		forward.x,
		forward.y,
		forward.z
	};

	FMOD_VECTOR fmodUp = {
		up.x,
		up.y,
		up.z
	};

	system->set3DListenerAttributes(0, &fmodPos, nullptr, &fmodForward, &fmodUp);
}



#pragma region volumeControl
void AudioManager::setMasterVolume(float newVolume) {
	this->masterVolume = newVolume;
	this->masterVolume = clampVol(this->masterVolume);
	refreshBGMVolume();
}

void AudioManager::setBGMVolume(float newVolume) {
	this->BGMVolume = newVolume;
	this->BGMVolume = clampVol(this->BGMVolume);
	refreshBGMVolume();
}
void AudioManager::setSFXVolume(float newVolume) {
	this->SFXVolume = newVolume;
	this->SFXVolume = clampVol(this->SFXVolume);
}

void AudioManager::toggleBGMMute() {
	if (mutedBGM) mutedBGM = false;
	else mutedBGM = true;
	refreshBGMVolume();
}
void AudioManager::toggleSFXMute() {
	if (mutedSFX) mutedSFX = false;
	else mutedSFX = true;
}

bool AudioManager::getBGMMute(){
	return this->mutedBGM;
}
bool AudioManager::getSFXMute(){
	return this->mutedSFX;
}

float AudioManager::clampVol(float vol) {
	if (vol > 1.0f)
		vol = 1.0f;
	else if (vol < 0.0f)
		vol = 0.0f;
	return vol;
}

void AudioManager::refreshBGMVolume()
{
	backgroundChannel->setPaused(true);
	backgroundChannel->setVolume(this->masterVolume * BGMVolume * (float)(!mutedBGM));
	backgroundChannel->setPaused(false);
}
#pragma endregion