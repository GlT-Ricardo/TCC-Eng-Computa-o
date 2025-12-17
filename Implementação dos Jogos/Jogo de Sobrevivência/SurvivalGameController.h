/***********************************************************************
SurvivalGameController.h - Controller for a Survival animal game
***********************************************************************/

#ifndef _SurvivalGameController_h_
#define _SurvivalGameController_h_

#include "vehicle.h"
#include "../KinectProjector/KinectProjector.h"

class CSurvivalGameController
{
public:
	CSurvivalGameController();
	virtual ~CSurvivalGameController();

	void setup(std::shared_ptr<KinectProjector> const& k);
	void update();
	void drawProjectorWindow();
	void drawMainWindow(float x, float y, float width, float height);

	bool StartGame();
	bool isIdle();
	bool isInIntro();
	void startFromIntro();
	void goBackToIdle();

	void setProjectorRes(ofVec2f& PR);
	void setKinectRes(ofVec2f& KR);
	void setKinectROI(ofRectangle &KROI);

private:
	std::shared_ptr<KinectProjector> kinectProjector;

	// Game states
	enum GameState {
		STATE_IDLE,
		STATE_INTRO,
		STATE_PLAYING,
		STATE_LEVEL_COMPLETE,
		STATE_LEVEL_FAILED,
		STATE_SHOWING_RESULTS
	};

	GameState currentState;
	float gameStartTime;
	float introStartTime;
	float resultsStartTime;
	float levelTransitionStartTime;
	float introDisplayTime;
	float resultsDisplayTime;
	float levelTransitionDuration;
	bool victory;

	// SISTEMA DE NÍVEIS
	int currentLevel;
	int maxLevels;
	bool levelCompleted;
	float levelStartTime;
	float levelDuration;
	int maxSharks;

	// CONFIGURAÇÕES POR NÍVEL
	struct LevelConfig {
		int levelNumber;
		int initialFish;
		int maxSharks;
		float sharkSpawnRate;
		float duration;
		std::string levelName;
	};
	std::vector<LevelConfig> levelConfigs;

	// Game variables
	int initialFishCount;
	int fishSurvived;
	int sharksSpawned;
	float lastSharkSpawnTime;
	float sharkSpawnInterval;

	// Spawn areas
	ofVec2f projRes;
	ofVec2f kinectRes;
	ofRectangle kinectROI;
	ofRectangle projROI;

	// Animals
	std::vector<Fish> fish;
	std::vector<Shark> sharks;
	std::vector<DangerousBOID> dangerBOIDS;

	// FBO for drawing
	ofFbo fboGame;

	// Fonts
	ofTrueTypeFont gameFont;
	ofTrueTypeFont scoreFont;

	// Images
	ofImage splashScreen;

	// IMAGENS PARA EFEITOS VISUAIS
	ofImage confettiImage;
	ofImage starsImage;
	ofImage trophyImage;      // Apenas para vitória final
	ofImage bronzeMedalImage; // Medalha bronze para fase 1
	ofImage silverMedalImage; // Medalha prata para fase 2  
	ofImage goldMedalImage;   // Medalha ouro para fase 3
	ofImage fireworksImage;

	// SISTEMA DE PARTÍCULAS PARA CONFETE
	struct ConfettiParticle {
		ofVec2f position;
		ofVec2f velocity;
		ofColor color;
		float size;
		float rotation;
		float rotationSpeed;
	};
	std::vector<ConfettiParticle> confettiParticles;

	// ANIMAÇÃO DE ESTRELAS PISCANDO
	struct StarEffect {
		ofVec2f position;
		float size;
		float alpha;
		float pulseSpeed;
	};
	std::vector<StarEffect> starEffects;

	// CONTROLES DE TAMANHO E POSIÇÃO
	float starMinSize;
	float starMaxSize;
	float medalYPosition;
	float trophyYPosition;

	// FLAGS PARA CONTROLE DE EFEITOS
	bool levelCompleteEffectsGenerated;
	bool victoryEffectsGenerated;

	// Game methods
	void setupLevels();
	void applyLevelConfig(int level);
	void goToNextLevel();
	void spawnInitialFish();
	void spawnShark();
	void updateGameState();
	void checkFishSurvival();
	void drawGameInfo();
	void drawIntroScreen();
	void drawLevelCompleteScreen();
	void drawVictoryScreen();
	void drawDefeatScreen();
	void resetGame();

	// EFEITOS VISUAIS
	void generateConfettiEffects();
	void generateStarEffects();
	void generateVictoryEffects();
	void updateVisualEffects();
	void clearVisualEffects();
	void debugPrintImageStatus(); // Para debug

								  // Helper methods
	void addNewFish();
	bool setRandomFishLocation(ofVec2f &location);
};

#endif