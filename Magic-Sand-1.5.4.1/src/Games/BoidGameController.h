/***********************************************************************
BoidGameController.h - Controller for a Sandbox animal game
Copyright (c) 2016-2017 Thomas Wolf and Rasmus R. Paulsen (people.compute.dtu.dk/rapa)
***********************************************************************/

#ifndef _BoidGameController_h_
#define _BoidGameController_h_

#include "vehicle.h"
#include "../KinectProjector/KinectProjector.h"

class CBoidGameController
{
public:
	CBoidGameController();
	virtual ~CBoidGameController();

	void setup(std::shared_ptr<KinectProjector> const& k);
	void update();
	void drawProjectorWindow();
	void drawMainWindow(float x, float y, float width, float height);

	bool StartGame(int difficulty);
	bool StartSeekMotherGame();
	bool isIdle();

	void setProjectorRes(ofVec2f& PR);
	void setKinectRes(ofVec2f& KR);
	void setKinectROI(ofRectangle &KROI);
	void setDebug(bool flag);
	void setupGui();

private:
	std::shared_ptr<KinectProjector> kinectProjector;

	void onButtonEvent(ofxDatGuiButtonEvent e);
	void onToggleEvent(ofxDatGuiToggleEvent e);
	void onSliderEvent(ofxDatGuiSliderEvent e);
	void UpdateGUI();
	void updateBOIDS();
	void PlayAndShowCountDown(int resultTime);
	void DrawScoresOnFBO();
	void DrawFinalScoresOnFBO();
	void ComputeScores();
	bool InitiateGameSequence();
	bool CreateSplashScreen();
	void SetupGameSequence();
	void addNewFish();
	void addNewShark();
	void addNewRabbit();
	bool addMotherFish();
	bool addMotherRabbit();
	bool setRandomVehicleLocation(ofRectangle area, bool liveInWater, ofVec2f & location);
	void drawVehicles();
	void drawMotherFish();
	void drawMotherRabbit();

	std::string DataBaseDir;
	int GameDifficulty;
	ofVec2f projRes;
	ofVec2f kinectRes;
	ofRectangle kinectROI;
	ofRectangle projROI;

	ofTrueTypeFont scoreFont;
	ofTrueTypeFont scoreFontSmall;
	ofTrueTypeFont nameFont;
	std::string ScoreText;
	std::string HiScoreText;

	bool doFlippedDrawing;
	float LastTimeEvent;

	enum eGameState
	{
		GAME_STATE_IDLE,
		GAME_STATE_SHOWSPLASHSCREEN,
		GAME_STATE_PLAYANDSHOWCOUNTDOWN,
		GAME_STATE_SHOWINTERMIDEATERESULT,
		GAME_STATE_SHOWFINALRESULT
	};

	ofImage splashScreen;
	std::vector<eGameState> GameSequence;
	std::vector<int> GameSequenceTimings;
	int CurrentGameSequence;

	bool debugOn;
	std::string debugBaseDir;

	// FBOs
	ofFbo fboVehicles;

	// Animals
	std::vector<Fish> fish;
	std::vector<Rabbit> rabbits;
	std::vector<Shark> sharks;
	std::vector<DangerousBOID> dangerBOIDS;

	// Mothers
	ofPoint motherFish;
	ofPoint motherRabbit;
	bool showMotherFish;
	bool showMotherRabbit;
	float motherPlatformSize;

	// Scores
	double Player1Score;
	double Player2Score;
	double Player1Food;
	double Player2Food;
	double Player1Skins;
	double Player2Skins;

	// GUI
	ofxDatGui* gui;
};

#endif