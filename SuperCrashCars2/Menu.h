#pragma once

// the major states game could be in anytime
enum class Screen {
	eMAINMENU,		// currently in main menu
	eLOADING,		// currently in loading screen 
	ePLAYING,		// game is currenly running
	eGAMEOVER		// game is currently in gameover screen
	//eCARSELECT,	// we don't have car selection yet
};
// enum for which button is selected when in main menu
enum class MainMenuButton
{
	eSTART,		// start button
	eHOWTOPLAY, // button to screen where we explain how the game works
	//eOPTIONS, // idek what options we should have yet
	eCREDITS,	// button to credits screen
	eQUIT		// button that quits game
};

// enum for the main menu subscreens
enum class MainMenuScreen {
	eMAIN_SCREEN,
	eHOWTOPLAY_SCREEN,
	eCREDITS_SCREEN
};

// enum for which button is selected when in pause screen
enum class PauseButton
{
	eRESUME,
	eQUITGAME
};

class Menu {
public:
	static inline Screen screen = Screen::eMAINMENU;
	static inline MainMenuButton menuButton = MainMenuButton::eSTART;
	static inline MainMenuScreen mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
	static inline PauseButton pauseButton = PauseButton::eRESUME;
	static inline bool paused = false;
	static inline bool quitGame = false;
	static inline bool startFlag = false;

	// depending on 'plus', menu button selected is incremented or decremented
	// this function switches buttons in menus when the user presses up or down on the dpad
	static void changeSelection(int plus) {
		switch (Menu::screen)
		{
		case Screen::eMAINMENU:

			switch (Menu::mainMenuScreen) {
			case MainMenuScreen::eMAIN_SCREEN:
				// in main screen, there is four buttons
				Menu::menuButton = (MainMenuButton)(((int)Menu::menuButton + plus + 4) % 4); // there are four buttons in main menu so mod 4
				break;
			case MainMenuScreen::eHOWTOPLAY_SCREEN:
				// do nothing, only one button
				break;
			case MainMenuScreen::eCREDITS_SCREEN:
				// do nothing, only one button 
				break;
			}


			break;
		case Screen::eLOADING:
			// nothing
			break;
		case Screen::ePLAYING:
			// since we will not trigger this function unless the game is pause, we can assume we are in pause screen.
			Menu::pauseButton = (PauseButton)(((int)Menu::pauseButton + plus + 2) % 2); // there are two buttons in pause menu so mod 2
			break;
		case Screen::eGAMEOVER:
			// do nothing, only quit button
			break;
		}


	}

	// this function "clicks" on the current button
	static void select() {

		// if (Menu::screen != Screen::eLOADING)  playSound();
		// we will play a sound here after the sound system is implemented

		switch (Menu::screen) {
		case Screen::eMAINMENU:
			
			switch (Menu::mainMenuScreen) {
			case MainMenuScreen::eMAIN_SCREEN:
				// in main screen, one of the four buttons is selected

				switch (Menu::menuButton) {
				case MainMenuButton::eSTART:
					// start game (for now just instantly switches to rendering)
					Menu::screen = Screen::ePLAYING;
					Menu::startFlag = true;

					break;
				case MainMenuButton::eHOWTOPLAY:
					// go to how to play screen
					Menu::mainMenuScreen = MainMenuScreen::eHOWTOPLAY_SCREEN;
					break;
				case MainMenuButton::eCREDITS:
					// go to credits screen
					Menu::mainMenuScreen = MainMenuScreen::eCREDITS_SCREEN;
					break;
				case MainMenuButton::eQUIT:
					// toggle some flag to quit game that can be seen from main 
					Menu::quitGame = true;
					break;
				}

				break;
			case MainMenuScreen::eHOWTOPLAY_SCREEN:
				// quit the current screen, return to main screen of main menu
				Menu::mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
				break;
			case MainMenuScreen::eCREDITS_SCREEN:
				// quit the current screen, return to main screen of main menu
				Menu::mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
				break;
			}


			break;

		case Screen::eLOADING:
			// nothing
			break;
		case Screen::ePLAYING:
			// since we will not trigger this function unless the game is paused, we can assume we are in pause screen.
			switch (Menu::pauseButton) {
			case PauseButton::eRESUME:
				// resume game, i.e. unpause
				Menu::togglePause();
				break;
			case PauseButton::eQUITGAME:
				// quit game, so go back to main menu.
				initMenu();
				break;
			}
			break;
		case Screen::eGAMEOVER:
			// do nothing, only quit button
			initMenu();
			break;
		}

	}

	// function to return us to init menu config, when game is quit for example.
	static void initMenu() {
		Menu::screen = Screen::eMAINMENU;
		Menu::menuButton = MainMenuButton::eSTART;
		Menu::mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
		Menu::pauseButton = PauseButton::eRESUME;
		Menu::paused = false;
	}

	// toggles pause (only if in game)
	static void togglePause() {
		if (Menu::screen == Screen::ePLAYING) {
			if (paused) Menu::paused = false;
			else Menu::paused = true;
		}
	}

	////////////////////// display functions

	static std::string printMenu() {
		std::string str = "Current Screen: \n";
		switch (Menu::screen)
		{
		case Screen::eMAINMENU:
			str = str + "Main Menu, with ";
			switch (Menu::mainMenuScreen) {
			case MainMenuScreen::eMAIN_SCREEN:
				str = str + "sub-screen Main, ";

				switch (Menu::menuButton) {
				case MainMenuButton::eSTART:
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
				str = str + "sub-screen Credits button BACK selected ";
				break;
			}

			break;
		case Screen::eLOADING:
			str = str + "Loading Screen";
			break;
		case Screen::ePLAYING:
			str = str + "In Game, ";
			if (Menu::paused) {
				str = str + "Paused, ";
				if (Menu::pauseButton == PauseButton::eRESUME)	{str = str + "with button RESUME selected";}
				else											{str = str + "with button QUIT selected";}
			}
			else {
				str = str + "Unpaused";
			}
			break;
		case Screen::eGAMEOVER:
			str = str + "Game Over, button BACK selected ";
			// do nothing, only quit button
			break;
		}

		return str;
	}

};