#include "GameManager.h"

// depending on 'plus', menu button selected is incremented or decremented
	// this function switches buttons in menus when the user presses up or down on the dpad
void GameManager::changeSelection(int plus) {
	bool playSound = true;
	switch (this->screen)
	{
	case Screen::eMAINMENU:

		switch (this->mainMenuScreen) {
		case MainMenuScreen::eMAIN_SCREEN:
			this->menuButton = (MainMenuButton)(((int)this->menuButton + plus + 6) % 6); 
			break;
		case MainMenuScreen::eMULTIPLAYER_SCREEN:
			this->playerSelectButton = (PlayerSelectButton)(((int)this->playerSelectButton + plus + 2) % 2);
			break;
		case MainMenuScreen::eHOWTOPLAY_SCREEN:
			// do nothing, only one button
			break;
		case MainMenuScreen::eCREDITS_SCREEN:
			// do nothing, only one button 
			break;
		case MainMenuScreen::eOPTIONS_SCREEN: // in option screen
			this->optionsButton = (OptionsButton)(((int)this->optionsButton + plus + 4) % 4);
			break;

		}


		break;
	case Screen::eLOADING:
		playSound = false;
		// nothing
		break;
	case Screen::ePLAYING:
		// since we will not trigger this function unless the game is pause, we can assume we are in pause screen.
		this->pauseButton = (PauseButton)(((int)this->pauseButton + plus + 2) % 2); // there are two buttons in pause menu so mod 2
		break;
	case Screen::eGAMEOVER:
		// do nothing, only quit button
		break;
	}

	if (playSound) AudioManager::get().playSound(SFX_MENUBUTTON, 0.3f);
}

void GameManager::incrementSlider(int right) {
	if (screen == Screen::eMAINMENU) {
		if (mainMenuScreen == MainMenuScreen::eOPTIONS_SCREEN) {
			switch (optionsButton)
			{
			case OptionsButton::eBGM:
				AudioManager::get().incrementBGMVolume(right);
				AudioManager::get().playSound(SFX_INCREMENT, 0.4f);
				break;
			case OptionsButton::eSFX:
				AudioManager::get().incrementSFXVolume(right);
				AudioManager::get().playSound(SFX_INCREMENT, 0.4f);
				break;
			case OptionsButton::eFPS:
				if (!multiplayer60FPS) multiplayer60FPS = true;
				else multiplayer60FPS = false;
				AudioManager::get().playSound(SFX_INCREMENT, 0.4f);
				break;
			case OptionsButton::eBACK: // nothing
				break;
			}
		}
		else if (mainMenuScreen == MainMenuScreen::eMULTIPLAYER_SCREEN && playerSelectButton == PlayerSelectButton::eSELECTING) {
			playerNumber = (playerNumber - 2 + right + 3) % 3 + 2;
			AudioManager::get().playSound(SFX_INCREMENT, 0.4f);

		}


	}
}
// this function "clicks" on the current button
void GameManager::select() {
	bool playSound = true;

	switch (this->screen) {
	case Screen::eMAINMENU:

		switch (this->mainMenuScreen) {
		case MainMenuScreen::eMAIN_SCREEN:
			// in main screen, one of the four buttons is selected

			switch (this->menuButton) {
			case MainMenuButton::eSINGLEPLAYER:
				// start game (for now just instantly switches to rendering)
				this->playerNumber = 1;
				this->screen = Screen::eLOADING;

				break;
			case MainMenuButton::eMULTIPLAYER:
				// start game (for now just instantly switches to rendering)
				this->playerNumber = 2;
				this->mainMenuScreen = MainMenuScreen::eMULTIPLAYER_SCREEN;

				break;
			case MainMenuButton::eHOWTOPLAY:
				// go to how to play screen
				this->mainMenuScreen = MainMenuScreen::eHOWTOPLAY_SCREEN;
				break;
			case MainMenuButton::eOPTIONS:
				// go options screen
				this->mainMenuScreen = MainMenuScreen::eOPTIONS_SCREEN;
				break;
			case MainMenuButton::eCREDITS:
				// go to credits screen
				this->mainMenuScreen = MainMenuScreen::eCREDITS_SCREEN;
				break;
			case MainMenuButton::eQUIT:
				// toggle some flag to quit game that can be seen from main 
				this->quitGame = true;
				break;
			}

			break;
		case MainMenuScreen::eMULTIPLAYER_SCREEN:
			if (playerSelectButton == PlayerSelectButton::eSTART) this->screen = Screen::eLOADING;
			else playSound = false;
			break;
		case MainMenuScreen::eHOWTOPLAY_SCREEN:
			// quit the current screen, return to main screen of main menu
			this->mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
			break;
		case MainMenuScreen::eCREDITS_SCREEN:
			// quit the current screen, return to main screen of main menu
			this->mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
			break;
		case MainMenuScreen::eOPTIONS_SCREEN:
			// quit the current screen if back button is selected
			if (this->optionsButton == OptionsButton::eBACK) this->mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
			else playSound = false;
			break;
		}

		break;

	case Screen::eLOADING:
		playSound = false;
		// nothing
		break;
	case Screen::ePLAYING:
		// since we will not trigger this function unless the game is paused, we can assume we are in pause screen.
		switch (this->pauseButton) {
		case PauseButton::eRESUME:
			// resume game, i.e. unpause
			togglePause();
			break;
		case PauseButton::eQUITGAME:
			AudioManager::get().gameOver();
			// quit game, so go back to main menu.
			AudioManager::get().backToMainMenu();
			initMenu();
			break;
		}
		break;
	case Screen::eGAMEOVER:
		// do nothing, only quit button
		AudioManager::get().setCarSoundsPause(true);
		AudioManager::get().backToMainMenu();
		initMenu();
		break;
	}
	if (playSound) AudioManager::get().playSound(SFX_MENUBUTTON, 0.3f);
}

// function to return us to init menu config, when game is quit for example.
void GameManager::initMenu() {
	AudioManager::get().setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->screen = Screen::eMAINMENU;
	this->menuButton = MainMenuButton::eSINGLEPLAYER;
	this->mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
	this->pauseButton = PauseButton::eRESUME;
	this->optionsButton = OptionsButton::eBGM;
	this->playerSelectButton = PlayerSelectButton::eSELECTING;
	this->paused = false;
	winner = -1;
	playerNumber = 1;
}

// toggles pause (only if in game)
void GameManager::togglePause() {
	if (GameManager::screen == Screen::ePLAYING) {
		

		if (paused) { 
			this->paused = false;
			AudioManager::get().setCarSoundsPause(false);
		}
		else { 
			this->paused = true; 
			AudioManager::get().setCarSoundsPause(true);

		}
	}
}

////////////////////// display functions

std::string GameManager::getMultiplayerFPS() {
	if (multiplayer60FPS) return std::string("60");
	else return std::string("30");
}

std::string GameManager::printMenu() {
	std::string str = "Current State:\n";

	switch (this->screen)
	{
	case Screen::eMAINMENU:
		str = str + "Main Menu, with ";
		switch (this->mainMenuScreen) {
		case MainMenuScreen::eMAIN_SCREEN:
			str = str + "sub-screen Main, ";

			switch (this->menuButton) {
			case MainMenuButton::eSINGLEPLAYER:
				str = str + "button START selected ";

				break;
			case MainMenuButton::eHOWTOPLAY:
				str = str + "button HOW TO PLAY selected ";
				break;
			case MainMenuButton::eCREDITS:
				str = str + "button CREDITS selected ";
				break;
			case MainMenuButton::eQUIT:
				str = str + "button QUIT selected ";

				break;
			}

			break;
		case MainMenuScreen::eHOWTOPLAY_SCREEN:
			// do nothing, only one button
			str = str + "sub-screen Howto, button BACK selected ";
			break;
		case MainMenuScreen::eCREDITS_SCREEN:
			str = str + "sub-screen Credits, button BACK selected ";
			break;
		case MainMenuScreen::eOPTIONS_SCREEN:
			switch (optionsButton){
			case OptionsButton::eBGM:
				str = str + "sub-screen Options, button BGM selected ";
				break;
			case OptionsButton::eSFX:
				str = str + "sub-screen Options, button SFX selected ";
				break;
			case OptionsButton::eBACK:
				str = str + "sub-screen Options, button BACK selected ";
				break;
			}
			break;
		}

		break;
	case Screen::eLOADING:
		str = str + "Loading....";
		break;
	case Screen::ePLAYING:
		str = str + "In Game, ";
		if (this->paused) {
			str = str + "Paused, ";
			if (this->pauseButton == PauseButton::eRESUME) { str = str + "with button RESUME selected"; }
			else { str = str + "with button QUIT selected"; }
		}
		else {
			str = str + "Unpaused";
		}


		break;
	case Screen::eGAMEOVER:
		str = str + "Game Over, button BACK selected,\nPlayer  ";
		str = str + std::to_string(winner);
		str = str + " is the winner ";
		// do nothing, only quit button
		break;
	}

	return str;
}
