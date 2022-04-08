#pragma once
#include "AudioManager.h"

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
	eSINGLEPLAYER,		// start button
	eMULTIPLAYER,		// start button
	eHOWTOPLAY,			// button to screen where we explain how the game works
	eOPTIONS,			// idek what options we should have yet
	eCREDITS,			// button to credits screen
	eQUIT				// button that quits game
};

// enum for the main menu subscreens
enum class MainMenuScreen {
	eMAIN_SCREEN,
	eMULTIPLAYER_SCREEN,
	eHOWTOPLAY_SCREEN,
	eCREDITS_SCREEN,
	eOPTIONS_SCREEN
};

enum class OptionsButton {
	eBGM,
	eSFX,
	eBACK
};
enum class PlayerSelectButton {
	eSELECTING,
	eSTART
};

// enum for which button is selected when in pause screen
enum class PauseButton
{
	eRESUME,
	eQUITGAME
};

class GameManager {
public:

	// Singleton class 
	static GameManager& get() {
		static GameManager instance;
		return instance;
	}
	GameManager(GameManager const&) = delete;
	void operator=(GameManager const&) = delete;


	Screen screen = Screen::eMAINMENU;
	MainMenuButton menuButton = MainMenuButton::eSINGLEPLAYER;
	MainMenuScreen mainMenuScreen = MainMenuScreen::eMAIN_SCREEN;
	PauseButton pauseButton = PauseButton::eRESUME;
	OptionsButton optionsButton = OptionsButton::eBGM;
	PlayerSelectButton playerSelectButton = PlayerSelectButton::eSELECTING;
	bool paused = false;
	bool quitGame = false;

	int winner;
	int playerNumber;

	void changeSelection(int plus);
	void incrementSlider(int right);
	void select();
	void initMenu();
	void togglePause();
	std::string printMenu();

private:
	GameManager() {} // private constructor for singleton
};