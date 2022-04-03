#include "AudioManager.h"
#include <fmod_errors.h>

#include "PVehicle.h"

void AudioManager::init(std::vector<PVehicle*>& vehicleList) {
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);

	if (result != FMOD_OK) {
		Log::error("Failed to create FMOD system");
	}

	result = system->init(2048, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK) {
		Log::error("Failed to initialize FMOD system");
	}

	this->BGMVolume = 1.0f;
	this->SFXVolume = 1.0f;
	this->masterVolume = 0.6f;
	this->muted = false;
	this->mutedBGM = false;
	this->mutedSFX = false;

	loadBackgroundSound(BGM_CLOUDS);

	loadSound(SFX_MENUBUTTON);
	loadSound(SFX_CONTROLLER_ON);
	loadSound(SFX_CONTROLLER_OFF);
	loadSound(SFX_INCREMENT);

	loadCarSound(SFX_CAR_IDLE);
	loadCarSound(SFX_CARWINDUP);
	loadCarSound(SFX_CAR_FAST);
	loadCarSound(SFX_CARWINDDOWN); 

	loadSound(SFX_CAR_HIT);
	loadSound(SFX_ITEM_COLLECT);
	loadSound(SFX_JUMP_NORMAL);
	loadSound(SFX_JUMP_MEGA);
	loadSound(SFX_DEATH);

	//loadCarIdleSound(SFX_CAR_FAST, 0.2f, 0, Utils::instance().pxToGlmVec3(vehicleList.at(0)->getPosition()));
	//loadCarIdleSound(SFX_CAR_FAST, 0.2f, 1, Utils::instance().pxToGlmVec3(vehicleList.at(1)->getPosition()));

	m_vehicleList = vehicleList;

	setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	playBackgroundMusic(BGM_CLOUDS);
}


void AudioManager::playBackgroundMusic(std::string filePath)
{
	FMOD::Sound* sound = mSounds[filePath.c_str()];

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &backgroundChannel);
	result = backgroundChannel->setVolume(this->masterVolume * BGMVolume * BGM_VOL_INIT * (float)(!mutedBGM));
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

void AudioManager::loadCarSound(std::string filePath) {
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_3D | FMOD_LOOP_NORMAL, nullptr, &sound);

	FMOD::SoundGroup* carSounds;
	system->createSoundGroup("sounds", &carSounds);
	sound->setSoundGroup(carSounds);

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

void AudioManager::loadCarIdleSound(std::string soundName, float soundVolume, int carid, glm::vec3 position) {
	FMOD::Sound* thisSound = mSounds[soundName.c_str()];
	FMOD_RESULT result = system->playSound(thisSound, nullptr, true, &carChannels[carid]);


	// set position
	position *= POSITION_SCALING;
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	FMOD_VECTOR vel = { 0.f, 0.f, 0.f };

	carChannels[carid]->set3DAttributes(&fmodPos, &vel);


	result = carChannels[carid]->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * soundVolume);
	result = carChannels[carid]->setPaused(false);
}

void AudioManager::updateCarPos(glm::vec3 position, int carid) {

	position *= POSITION_SCALING;
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	FMOD_VECTOR vel = { 0.f, 0.f, 0.f };

	carChannels[carid]->set3DAttributes(&fmodPos, &vel );



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
	position *= POSITION_SCALING;
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

	position *= POSITION_SCALING;
	//forward *= POSITION_SCALING;
	//up *= POSITION_SCALING;

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

int AudioManager::getBGMLevel(){
	return static_cast<int>(BGMVolume * 10 + 0.5f);
}

int AudioManager::getSFXLevel(){
	return static_cast<int>(SFXVolume * 10 + 0.5f);
}

void AudioManager::update(){
	system->update();
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
	backgroundChannel->setVolume(this->masterVolume * BGMVolume * BGM_VOL_INIT * (float)(!mutedBGM));
	backgroundChannel->setPaused(false);
}

void AudioManager::incrementBGMVolume(int sign) { // only pass + or - 1
	float deltaVolume = 0.1f * sign;
	setBGMVolume(BGMVolume + deltaVolume);
}
void AudioManager::incrementSFXVolume(int sign) { // only pass + or - 1
	float deltaVolume = 0.1f * sign;
	setSFXVolume(SFXVolume + deltaVolume);
}
#pragma endregion