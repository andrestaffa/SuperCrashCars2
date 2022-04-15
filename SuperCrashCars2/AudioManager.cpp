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
	this->masterVolume = 0.8f;
	this->muted = false;
	this->mutedBGM = false;
	this->mutedSFX = false;

	loadBackgroundSound(BGM_CLOUDS, true);

	//loadBackgroundSound(BGM_INTRO, false);
	loadBackgroundSound(BGM_INTRO_LONG, false);
	loadBackgroundSound(BGM_LOOP, true);
	//loadBackgroundSound(BGM_PIANO_INTRO, false);
	loadBackgroundSound(BGM_PIANO_LOOP, true);
	loadBackgroundSound(BGM_BATTLE, true);

	loadSound(SFX_MENUBUTTON);
	loadSound(SFX_CONTROLLER_ON);
	loadSound(SFX_CONTROLLER_OFF);
	loadSound(SFX_INCREMENT);

	loadCarSound(SFX_CAR_IDLE, true);
	loadCarSound(SFX_CARWINDUP, false);
	loadCarSound(SFX_CAR_FAST, true);
	loadCarSound(SFX_CARWINDDOWN, false);	

	loadCarSound(SFX_CAR_BOOST_START, false);
	loadCarSound(SFX_CAR_BOOST_LOOP, true);
	loadCarSound(SFX_CAR_BOOST_END, false);

	loadSound(SFX_CAR_HIT);
	loadSound(SFX_ITEM_COLLECT);
	loadSound(SFX_JUMP_NORMAL);
	loadSound(SFX_JUMP_MEGA);
	loadSound(SFX_DEATH);

	m_vehicleList = vehicleList;

	setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	playBackgroundMusic(BGM_INTRO_LONG, 1.2);

	bgmState = BGMState::MENU_INTRO;

}


void AudioManager::playBackgroundMusic(std::string filePath, float soundVolume)
{
	FMOD::Sound* sound = mSounds[filePath.c_str()];

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &backgroundChannel);
	result = backgroundChannel->setVolume(this->masterVolume * BGMVolume * BGM_VOL_INIT * (float)(!mutedBGM) * soundVolume);
	result = backgroundChannel->setPaused(false);
}


void AudioManager::loadBackgroundSound(std::string filePath, bool looping)
{
	FMOD_MODE mode;
	if (looping) mode = FMOD_LOOP_NORMAL;
	else mode = FMOD_LOOP_OFF;
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_2D | mode, nullptr, &sound);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}

	// save sound pointer to map
	mSounds[filePath] = sound;
}	

void AudioManager::loadCarSound(std::string filePath, bool looping) {
	FMOD_MODE mode;
	if (looping) mode = FMOD_LOOP_NORMAL;
	else mode = FMOD_LOOP_OFF;

	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_3D | mode | FMOD_3D_LINEARROLLOFF, nullptr, &sound);

	FMOD::SoundGroup* carSounds;
	system->createSoundGroup("sounds", &carSounds);
	sound->setSoundGroup(carSounds);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}
	sound->set3DMinMaxDistance(1.f * POSITION_SCALING, 40.f *POSITION_SCALING);
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

void AudioManager::startCarSounds() {
	int carid;
	glm::vec3 position;
	FMOD::Sound* thisSound = mSounds[std::string(SFX_CAR_IDLE).c_str()];
	FMOD_RESULT result;
	for (PVehicle* carPtr : m_vehicleList) {
		carid = carPtr->carid;
		thisSound = mSounds[std::string(SFX_CAR_IDLE).c_str()];
		result = system->playSound(thisSound, nullptr, true, &carDrivingChannels[carid]);

		// set position
		position = Utils::instance().pxToGlmVec3(carPtr->getPosition());
		position *= POSITION_SCALING;
		FMOD_VECTOR fmodPos = { position.x, position.y,	position.z };
		FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
		carDrivingChannels[carid]->set3DAttributes(&fmodPos, &vel);
		audioState[carid] = DrivingState::eIDLE;
		result = carDrivingChannels[carid]->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * CAR_SOUNDS_VOLUME);
		result = carDrivingChannels[carid]->setPaused(false);
	}


}

void AudioManager::setCarSoundsPause(bool pause) {
	int carid;
	for (PVehicle* carPtr : m_vehicleList) {
		carid = carPtr->carid;
		boostEndChannels[carid]->setPaused(pause);
		carBoostChannels[carid]->setPaused(pause);
		carDrivingChannels[carid]->setPaused(pause);
	}
}

void AudioManager::updateCarSounds() {
	unsigned int soundPosition;
	bool isPlaying;
	int carid;
	glm::vec3 position;
	FMOD_RESULT result;
	for (PVehicle* carPtr : m_vehicleList) {
		carid = carPtr->carid;
		if (carPtr->accelerating && !carPtr->getVehicleInAir()) {
			switch (audioState[carid]){
			case DrivingState::eIDLE:
				audioState[carid] = DrivingState::eACCELERATING;
				result = system->playSound(mSounds[std::string(SFX_CARWINDUP).c_str()], nullptr, true, &carDrivingChannels[carid]);
				break;
			case DrivingState::eACCELERATING:
				carDrivingChannels[carid]->isPlaying(&isPlaying);
				if (!isPlaying) { // if finished accelerating, switch to loop
					audioState[carid] = DrivingState::eLOOP;
					result = system->playSound(mSounds[std::string(SFX_CAR_FAST).c_str()], nullptr, true, &carDrivingChannels[carid]);
				}
				break;
			case DrivingState::eLOOP:
				break;
			case DrivingState::eDECELERATING:
				carDrivingChannels[carid]->getPosition(&soundPosition, FMOD_TIMEUNIT_PCM);
				audioState[carid] = DrivingState::eACCELERATING;
				result = system->playSound(mSounds[std::string(SFX_CARWINDUP).c_str()], nullptr, true, &carDrivingChannels[carid]);
				carDrivingChannels[carid]->setPosition(55957 - soundPosition, FMOD_TIMEUNIT_PCM);

				break;
			default:
				break;
			}
		}
		else {
			switch (audioState[carid]) {
			case DrivingState::eIDLE:
				break;
			case DrivingState::eACCELERATING:
				carDrivingChannels[carid]->getPosition(&soundPosition, FMOD_TIMEUNIT_PCM);
				audioState[carid] = DrivingState::eDECELERATING;
				result = system->playSound(mSounds[std::string(SFX_CARWINDDOWN).c_str()], nullptr, true, &carDrivingChannels[carid]);
				carDrivingChannels[carid]->setPosition(55957 - soundPosition, FMOD_TIMEUNIT_PCM);
				break;
			case DrivingState::eLOOP:
				audioState[carid] = DrivingState::eDECELERATING;
				result = system->playSound(mSounds[std::string(SFX_CARWINDDOWN).c_str()], nullptr, true, &carDrivingChannels[carid]);
				break;
			case DrivingState::eDECELERATING:
				carDrivingChannels[carid]->isPlaying(&isPlaying);
				if (!isPlaying) { // if finished accelerating, switch to loop
					audioState[carid] = DrivingState::eIDLE;
					result = system->playSound(mSounds[std::string(SFX_CAR_IDLE).c_str()], nullptr, true, &carDrivingChannels[carid]);
				}

				break;
			default:
				break;
			}
		}

		// if boost controller button is held and the boost meter is more than 100
		if (carPtr->vehicleParams.boosting && carPtr->vehicleParams.boost) {
			boostEndChannels[carid]->stop();
			switch (boostState[carid]) {
			case BoostingState::eNOT_BOOSTING:
				boostState[carid] = BoostingState::eACCELERATING;
				result = system->playSound(mSounds[std::string(SFX_CAR_BOOST_START).c_str()], nullptr, true, &carBoostChannels[carid]);
				 
				break;
			case BoostingState::eACCELERATING:
				carBoostChannels[carid]->isPlaying(&isPlaying);
				if (!isPlaying) { // if finished accelerating, switch to loop
					boostState[carid] = BoostingState::eLOOP;
					result = system->playSound(mSounds[std::string(SFX_CAR_BOOST_LOOP).c_str()], nullptr, true, &carBoostChannels[carid]);
				}
				break;
			case BoostingState::eACCELERATING_PAUSE:
				boostState[carid] = BoostingState::eACCELERATING;
				carBoostChannels[carid]->setPaused(false);

				break;
			case BoostingState::eLOOP:
				break;
			case BoostingState::eLOOP_PAUSE:
				boostState[carid] = BoostingState::eLOOP;


				break;
			}
		
		}
		else {
			switch (boostState[carid]) {
			case BoostingState::eNOT_BOOSTING:

				break;
			case BoostingState::eACCELERATING:
					boostState[carid] = BoostingState::eACCELERATING_PAUSE;
					boostTimestamp[carid] = steady_clock::now();
					carBoostChannels[carid]->setPaused(true);
					result = system->playSound(mSounds[std::string(SFX_CAR_BOOST_END).c_str()], nullptr, true, &boostEndChannels[carid]);
				
				break;
			case BoostingState::eACCELERATING_PAUSE:
				carBoostChannels[carid]->isPlaying(&isPlaying);

				if (isPlaying) {
					boostState[carid] = BoostingState::eNOT_BOOSTING;
				}
				break;
			case BoostingState::eLOOP:
				boostState[carid] = BoostingState::eLOOP_PAUSE;
				boostTimestamp[carid] = steady_clock::now();
				carBoostChannels[carid]->setPaused(true);
				result = system->playSound(mSounds[std::string(SFX_CAR_BOOST_END).c_str()], nullptr, true, &boostEndChannels[carid]);
				break;
			case BoostingState::eLOOP_PAUSE:
				boostEndChannels[carid]->isPlaying(&isPlaying);

				if (!isPlaying) {
					boostState[carid] = BoostingState::eNOT_BOOSTING;
				}

				break;
			}
		}


		// update the channel positions
		position = Utils::instance().pxToGlmVec3(carPtr->getPosition());
		position *= POSITION_SCALING;
		FMOD_VECTOR fmodPos = { position.x, position.y,	position.z };
		FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
		carDrivingChannels[carid]->set3DAttributes(&fmodPos, &vel);	
		boostEndChannels[carid]->set3DAttributes(&fmodPos, &vel);
		carBoostChannels[carid]->set3DAttributes(&fmodPos, &vel);
		result = carDrivingChannels[carid]->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * CAR_SOUNDS_VOLUME);
		result = boostEndChannels[carid]->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * CAR_SOUNDS_VOLUME);
		result = carBoostChannels[carid]->setVolume(this->masterVolume * SFXVolume * (float)(!mutedSFX) * CAR_SOUNDS_VOLUME);
		result = carDrivingChannels[carid]->setPaused(false);
		result = boostEndChannels[carid]->setPaused(false);
		if ((boostState[carid] != BoostingState::eACCELERATING_PAUSE) && (boostState[carid] != BoostingState::eLOOP_PAUSE)) {
			result = carBoostChannels[carid]->setPaused(false);
		}



	}


}

void AudioManager::updateBGM() {
	bool isPlaying;
	backgroundChannel->isPlaying(&isPlaying);
	switch (bgmState){
	case BGMState::MENU_INTRO:
		if (!isPlaying) {
			playBackgroundMusic(BGM_LOOP, 1.2f);
			bgmState = BGMState::MENU_LOOP;
		}
		break;
	case BGMState::MENU_LOOP:
		break;
	case BGMState::INGAME:
		break;
	case BGMState::GAMEOVER_LOOP:
		break;
	default:
		break;
	}
}

void AudioManager::flipBGM() {
	unsigned int soundPosition;
	FMOD_RESULT result;
	backgroundChannel->getPosition(&soundPosition, FMOD_TIMEUNIT_PCM);
	backgroundChannel->stop();

	switch (bgmState) {
	case BGMState::MENU_LOOP:
		playBackgroundMusic(BGM_PIANO_LOOP, 1.2f);
		backgroundChannel->setPosition(soundPosition, FMOD_TIMEUNIT_PCM);
		break;
	case BGMState::GAMEOVER_LOOP:
		playBackgroundMusic(BGM_LOOP, 1.2f);
		backgroundChannel->setPosition(soundPosition, FMOD_TIMEUNIT_PCM);
		break;
	default:
		Log::debug("Calling flipBgm() in WRONG");
		// should not happen
		break;
	}
}

void AudioManager::startGame() {
	bgmState = BGMState::INGAME;
	backgroundChannel->stop();
	playBackgroundMusic(BGM_BATTLE, 0.8f);

}

void AudioManager::gameOver() {
	bgmState = BGMState::GAMEOVER_LOOP;
	backgroundChannel->stop();
	setCarSoundsPause(true);
	for (int i = 0; i < 4; i++) {
		carDrivingChannels[i]->stop();
		boostEndChannels[i]->stop(); // corresponds to the carids
		carBoostChannels[i]->stop(); // because of this, max out at 4 cars.
	}

	playBackgroundMusic(BGM_PIANO_LOOP, 1.2f);
}

void AudioManager::backToMainMenu() {
	flipBGM();

	for (int i = 0; i < 4; i++) {
		carDrivingChannels[i]->stop();
		boostEndChannels[i]->stop(); // corresponds to the carids
		carBoostChannels[i]->stop(); // because of this, max out at 4 cars.
	}

	bgmState = BGMState::MENU_LOOP;

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