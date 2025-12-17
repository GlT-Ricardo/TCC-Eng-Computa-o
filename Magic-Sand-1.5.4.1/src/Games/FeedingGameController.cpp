/***********************************************************************
FeedingGameController.cpp - Controller for a Feeding animal game
***********************************************************************/

#include "FeedingGameController.h"

CFeedingGameController::CFeedingGameController()
{
	currentState = STATE_IDLE;

	// SISTEMA DE NÍVEIS
	currentLevel = 1;
	maxLevels = 3;
	levelCompleted = false;
	targetFood = 5;

	// CONFIGURAÇÃO DOS NÍVEIS
	setupLevels();

	// Usa configuração do nível 1 inicialmente
	applyLevelConfig(1);

	resultsDisplayTime = 5.0f;
	introDisplayTime = 10.0f;
	levelTransitionDuration = 5.0f;

	// CONTROLES DE TAMANHO E POSIÇÃO
	starMinSize = 40.0f;     // Tamanho mínimo das estrelas
	starMaxSize = 100.0f;    // Tamanho máximo das estrelas
	medalYPosition = 100.0f; // Posição Y da medalha (Level Complete)
	trophyYPosition = 120.0f; // Posição Y do troféu (Vitória Final)

							  // CARREGAR IMAGEM
	std::string splashScreenFname = "boidgame/art/FeedingGameSplashScreen.png";
	if (!splashScreen.load(splashScreenFname))
	{
		ofLogError("CFeedingGameController") << "Could not load splash screen: " << splashScreenFname;
	}

	// INICIALIZAR FLAGS DE EFEITOS
	levelCompleteEffectsGenerated = false;
	victoryEffectsGenerated = false;

	// CARREGAR IMAGENS DE EFEITOS VISUAIS
	std::string confettiPath = "boidgame/art/confetti.png";
	std::string starsPath = "boidgame/art/stars.png";
	std::string trophyPath = "boidgame/art/trophy.png";
	std::string bronzeMedalPath = "boidgame/art/bronze_medal.png";
	std::string silverMedalPath = "boidgame/art/silver_medal.png";
	std::string goldMedalPath = "boidgame/art/gold_medal.png";
	std::string fireworksPath = "boidgame/art/fireworks.png";

	// Tentar carregar as imagens
	bool allImagesLoaded = true;

	if (!confettiImage.load(confettiPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar confetti.png";
		allImagesLoaded = false;
	}

	if (!starsImage.load(starsPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar stars.png";
		allImagesLoaded = false;
	}

	if (!trophyImage.load(trophyPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar trophy.png";
		allImagesLoaded = false;
	}

	if (!bronzeMedalImage.load(bronzeMedalPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar bronze_medal.png";
		// Fallback: criar medalha bronze básica
		bronzeMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		bronzeMedalImage.getPixels().setColor(ofColor(205, 127, 50)); // Bronze
		bronzeMedalImage.update();
	}

	if (!silverMedalImage.load(silverMedalPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar silver_medal.png";
		// Fallback: criar medalha prata básica
		silverMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		silverMedalImage.getPixels().setColor(ofColor(192, 192, 192)); // Prata
		silverMedalImage.update();
	}

	if (!goldMedalImage.load(goldMedalPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar gold_medal.png";
		// Fallback: criar medalha ouro básica
		goldMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		goldMedalImage.getPixels().setColor(ofColor(255, 215, 0)); // Ouro
		goldMedalImage.update();
	}

	if (!fireworksImage.load(fireworksPath)) {
		ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar fireworks.png";
		allImagesLoaded = false;
	}

	if (!allImagesLoaded) {
		ofLogWarning("CFeedingGameController") << "Algumas imagens de efeitos não foram carregadas. Usando fallbacks.";
	}

	// DEBUG: mostrar status das imagens
	debugPrintImageStatus();

	// INICIALIZAR SISTEMA DE PARTÍCULAS
	confettiParticles.clear();
	starEffects.clear();
}

CFeedingGameController::~CFeedingGameController()
{
}

void CFeedingGameController::debugPrintImageStatus()
{
	std::cout << "=== Status das Imagens de Efeitos (Feeding Game) ===" << std::endl;
	std::cout << "Splash Screen: " << (splashScreen.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Confetti: " << (confettiImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Stars: " << (starsImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Trophy: " << (trophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Bronze Medal: " << (bronzeMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Silver Medal: " << (silverMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Gold Medal: " << (goldMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Fireworks: " << (fireworksImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "==================================================" << std::endl;
}

void CFeedingGameController::setupLevels()
{
	levelConfigs.clear();

	// 1 == Fase; 12 == Quant de Peixes; 	// 5 == Quant de Comida; 3.0f == Tempo de spawn da comida
	// 65.0f == Tempo de Duração da Fase.

	// NÍVEL 1 - FÁCIL
	levelConfigs.push_back({ 1, 10, 1, 3.0f, 20.0f, "Comeco da Aventura" });

	// NÍVEL 2 - MÉDIO
	levelConfigs.push_back({ 2, 10, 1, 3.5f, 20.0f, "Fome Aquatica" });

	// NÍVEL 3 - DIFÍCIL
	levelConfigs.push_back({ 3, 10, 1, 4.0f, 20.0f, "Hora do Banquete" });
}

void CFeedingGameController::applyLevelConfig(int level)
{
	if (level < 1 || level > levelConfigs.size()) return;

	LevelConfig config = levelConfigs[level - 1];

	initialFishCount = config.initialFish;
	targetFood = config.targetFood;
	foodSpawnInterval = config.foodSpawnRate;
	levelDuration = config.duration;
	maxFoodItems = 8;
}

void CFeedingGameController::goToNextLevel()
{
	currentLevel++;

	if (currentLevel > maxLevels) {
		currentState = STATE_SHOWING_RESULTS;
		resultsStartTime = ofGetElapsedTimef();
		victory = true;
		// Gerar efeitos de vitória final
		generateVictoryEffects();
		return;
	}

	currentState = STATE_LEVEL_COMPLETE;
	levelTransitionStartTime = ofGetElapsedTimef();
	// Resetar flag para gerar novos efeitos
	levelCompleteEffectsGenerated = false;
}

void CFeedingGameController::setup(std::shared_ptr<KinectProjector> const& k)
{
	kinectProjector = k;

	// Setup fonts
	gameFont.load("verdana.ttf", 45);
	scoreFont.load("verdana.ttf", 45);

	resetGame();
}

void CFeedingGameController::update()
{
	if (currentState == STATE_IDLE) {
		fboGame.begin();
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 150);
		fboGame.end();
		return;
	}

	if (currentState == STATE_INTRO) {
		float currentTime = ofGetElapsedTimef();
		if (currentTime - introStartTime > introDisplayTime) {
			startFromIntro();
		}
		return;
	}

	if (currentState == STATE_PLAYING) {
		float currentTime = ofGetElapsedTimef();
		float levelElapsedTime = currentTime - levelStartTime;

		// VERIFICA SE NÍVEL FOI COMPLETADO (por comida)
		if (!levelCompleted && foodCollected >= targetFood) {
			levelCompleted = true;
			goToNextLevel();
			return;
		}

		// VERIFICA SE TEMPO ACABOU (GAME OVER)
		if (!levelCompleted && levelElapsedTime >= levelDuration) {
			// Vai direto para tela de resultados
			currentState = STATE_SHOWING_RESULTS;
			resultsStartTime = currentTime;
			victory = false;
			clearVisualEffects();
			return;
		}

		// SPAWN DE COMIDA (só se nível não está completo)
		if (!levelCompleted && currentTime - lastFoodSpawnTime > foodSpawnInterval && foodItems.size() < maxFoodItems) {
			spawnFood();
			lastFoodSpawnTime = currentTime;
		}

		// Remove old food items
		for (int i = foodItems.size() - 1; i >= 0; i--) {
			if (currentTime - foodItems[i].spawnTime > 10.0f) {
				foodItems.erase(foodItems.begin() + i);
			}
		}

		// Update game state
		updateGameState();

		// Draw para FBO
		fboGame.begin();
		ofClear(0, 0, 0, 0);
		drawGameInfo();
		drawFoodItems();

		// Draw animals
		for (auto &f : fish) {
			f.draw();
		}
		fboGame.end();
	}

	// Estados de transição
	if (currentState == STATE_LEVEL_COMPLETE) {
		// Atualizar efeitos visuais
		updateVisualEffects();

		float currentTime = ofGetElapsedTimef();
		if (currentTime - levelTransitionStartTime > levelTransitionDuration) {
			applyLevelConfig(currentLevel);
			levelCompleted = false;
			levelStartTime = ofGetElapsedTimef();
			spawnInitialFish();
			foodItems.clear();
			foodCollected = 0;
			lastFoodSpawnTime = ofGetElapsedTimef();

			// Limpar efeitos visuais
			clearVisualEffects();
			levelCompleteEffectsGenerated = false;

			currentState = STATE_PLAYING;
		}
		return;
	}

	if (currentState == STATE_SHOWING_RESULTS) {
		// Atualizar efeitos visuais se for vitória
		if (victory) {
			updateVisualEffects();
		}

		float currentTime = ofGetElapsedTimef();
		if (currentTime - resultsStartTime > resultsDisplayTime) {
			resetGame();
		}
	}
}

void CFeedingGameController::updateGameState()
{
	if (kinectProjector->isImageStabilized()) {
		// Update fish
		for (auto &f : fish) {
			f.applyBehaviours(false, fish, std::vector<DangerousBOID>());
			f.update();
		}
		checkFoodCollection();
	}
}

void CFeedingGameController::checkFoodCollection()
{
	// Percorre a lista de comida de trás para frente.
	for (int i = foodItems.size() - 1; i >= 0; i--) {

		// Se o item de comida não está ativo, simplesmente ignore.
		// "active" indica se o item ainda existe.
		if (!foodItems[i].active) continue;

		// Verifica todos os peixes no cenário.
		for (auto &f : fish) {

			// Calcula a distância entre o peixe e o item de comida.
			// getLocation() retorna posição do peixe.
			// foodItems[i].location é a posição da comida.
			// length() retorna distância final.
			float distance = (f.getLocation() - foodItems[i].location).length();

			// Regra de coleta:
			// Se o peixe estiver próximo o bastante da comida,
			// considerando o tamanho do peixe + uma margem (10px),
			// então ele "come" a comida.
			if (distance < f.getSize() + 10) {

				// Desativa o item de comida (ele não será desenhado nem processado mais).
				foodItems[i].active = false;

				// Contabiliza comida coletada nesta fase.
				foodCollected++;

				// Contabiliza comida total (para estatísticas globais).
				totalFood++;

				// Para o loop dos peixes, pois o item já foi comido por um deles.
				break;
			}
		}
	}
}


void CFeedingGameController::spawnInitialFish()
{
	fish.clear();
	for (int i = 0; i < initialFishCount; i++) {
		addNewFish();
	}
}

void CFeedingGameController::spawnFood()
{
	ofVec2f location;
	if (setRandomFoodLocation(location)) {
		FoodItem food;
		food.location = location;
		food.active = true;
		food.spawnTime = ofGetElapsedTimef();
		foodItems.push_back(food);
	}
}

void CFeedingGameController::generateConfettiEffects()
{
	confettiParticles.clear();

	int particleCount = 150; // Quantidade de confetes
	for (int i = 0; i < particleCount; i++) {
		ConfettiParticle p;
		// Evitar spawn na área central (onde fica o texto)
		float spawnX = ofRandom(0, projROI.width);
		float spawnY;

		// 70% dos confetes spawnam na parte inferior
		if (ofRandom(1.0) < 0.7) {
			spawnY = ofRandom(projROI.height * 0.6, projROI.height + 20);
		}
		else {
			spawnY = ofRandom(-100, projROI.height * 0.3);
		}

		p.position.set(spawnX, spawnY);
		p.velocity.set(ofRandom(-1, 1), ofRandom(1, 3)); // Velocidade mais lenta

														 // Cores mais suaves (tons de amarelo/laranja para combinar com o tema de comida)
		int colorType = ofRandom(4);
		switch (colorType) {
		case 0: p.color = ofColor(255, 200, 50); break;  // Amarelo dourado
		case 1: p.color = ofColor(255, 150, 50); break;  // Laranja
		case 2: p.color = ofColor(255, 255, 100); break; // Amarelo claro
		case 3: p.color = ofColor(255, 100, 50); break;  // Laranja avermelhado
		}

		p.size = ofRandom(8, 15); // Tamanho menor
		p.rotation = ofRandom(0, 360);
		p.rotationSpeed = ofRandom(-3, 3); // Rotação mais lenta

		confettiParticles.push_back(p);
	}
}

void CFeedingGameController::generateStarEffects()
{
	starEffects.clear();

	int starCount = 15;
	for (int i = 0; i < starCount; i++) {
		StarEffect star;

		// Posicionar estrelas nas bordas, evitando o centro
		float posX, posY;
		if (ofRandom(1.0) < 0.5) {
			// Borda lateral
			posX = ofRandom(1.0) < 0.5 ? ofRandom(0, 100) : ofRandom(projROI.width - 100, projROI.width);
			posY = ofRandom(50, projROI.height - 200);
		}
		else {
			// Borda superior/inferior
			posX = ofRandom(100, projROI.width - 100);
			posY = ofRandom(1.0) < 0.5 ? ofRandom(0, 80) : ofRandom(projROI.height - 200, projROI.height - 100);
		}

		star.position.set(posX, posY);
		star.size = ofRandom(starMinSize, starMaxSize); // Usando variáveis de controle
		star.alpha = ofRandom(80, 180); // Mais transparente
		star.pulseSpeed = ofRandom(0.3, 1.5); // Pulsação mais lenta

		starEffects.push_back(star);
	}
}

void CFeedingGameController::generateVictoryEffects()
{
	// Gerar confete básico
	generateConfettiEffects();

	// Adicionar mais confete para vitória
	for (int i = 0; i < 100; i++) {
		ConfettiParticle p;
		p.position.set(ofRandom(0, projROI.width),
			ofRandom(0, projROI.height));
		p.velocity.set(ofRandom(-3, 3), ofRandom(-3, 3));
		p.color = ofColor(255, 200, 50); // Dourado para vitória
		p.size = ofRandom(15, 30);
		p.rotation = ofRandom(0, 360);
		p.rotationSpeed = ofRandom(-8, 8);
		confettiParticles.push_back(p);
	}

	// Gerar estrelas
	generateStarEffects();
	victoryEffectsGenerated = true;
}

void CFeedingGameController::updateVisualEffects()
{
	// Atualizar partículas de confete
	for (auto& p : confettiParticles) {
		p.position += p.velocity;
		p.rotation += p.rotationSpeed;

		// Gravidade
		p.velocity.y += 0.1;

		// Reduz velocidade com resistência do ar
		p.velocity *= 0.99;

		// Se sair da tela, resetar posição
		if (p.position.y > projROI.height + 50) {
			p.position.y = -50;
			p.position.x = ofRandom(0, projROI.width);
		}
	}

	// Atualizar estrelas piscantes
	float currentTime = ofGetElapsedTimef();
	for (auto& star : starEffects) {
		star.alpha = 150 + 105 * sin(currentTime * star.pulseSpeed);
	}
}

void CFeedingGameController::clearVisualEffects()
{
	confettiParticles.clear();
	starEffects.clear();
	levelCompleteEffectsGenerated = false;
	victoryEffectsGenerated = false;
}

void CFeedingGameController::drawFoodItems()
{
	for (auto &food : foodItems) {
		if (!food.active) continue;

		ofVec2f projPos = kinectProjector->kinectCoordToProjCoord(food.location.x, food.location.y);

		ofPushMatrix();
		ofTranslate(projPos);

		// Draw food as small circles
		float foodSize = 12.0f;
		ofSetColor(255, 255, 0);
		ofFill();
		ofDrawCircle(0, 0, foodSize);

		// Pulsing effect
		float pulse = sin(ofGetElapsedTimef() * 5) * 2 + foodSize;
		ofSetColor(255, 165, 0, 100);
		ofDrawCircle(0, 0, pulse);

		ofPopMatrix();
	}
}

void CFeedingGameController::drawGameInfo()
{
	float currentTime = ofGetElapsedTimef();
	float levelElapsedTime = currentTime - levelStartTime;
	float levelTimeLeft = levelDuration - levelElapsedTime;

	ofSetColor(0, 0, 0);

	// INFORMAÇÕES DO NÍVEL
	LevelConfig config = levelConfigs[currentLevel - 1];

	// Textos
	std::string levelStr = "Fase " + ofToString(currentLevel) + ": " + config.levelName;
	std::string timeStr = "Tempo: " + ofToString((int)levelTimeLeft) + "s";
	std::string foodStr = "Comida: " + ofToString(foodCollected) + " / " + ofToString(targetFood);

	// ===========================================
	//   LINHA 1: CENTRALIZAR FASE
	// ===========================================
	float yLine1 = 80; // Altura da primeira linha
	float wLevel = gameFont.stringWidth(levelStr);
	float xLevel = (ofGetWidth() - wLevel) / 2;

	gameFont.drawString(levelStr, xLevel, yLine1);

	// ===========================================
	//   LINHA 2: TEMPO E COMIDA LADO A LADO
	// ===========================================
	float yLine2 = 140; // Altura da segunda linha (abaixo da primeira)

	float wTime = scoreFont.stringWidth(timeStr);
	float wFood = scoreFont.stringWidth(foodStr);
	float spacing = 100; // Espaçamento entre tempo e comida

	float totalWidth = wTime + wFood + spacing;
	float startX = (ofGetWidth() - totalWidth) / 2;

	// Desenhar Tempo
	scoreFont.drawString(timeStr, startX, yLine2);

	// Desenhar Comida
	scoreFont.drawString(foodStr, startX + wTime + spacing, yLine2);
}

void CFeedingGameController::drawIntroScreen()
{
	fboGame.begin();
	ofClear(0, 0, 0, 200);

	if (splashScreen.isAllocated()) {
		splashScreen.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		// Fallback com texto
		// Título centralizado
		ofSetColor(0, 0, 0);
		string title = "JOGO DE ALIMENTACAO";
		float titleW = gameFont.stringWidth(title);
		float titleX = (projROI.width - titleW) / 2;
		gameFont.drawString(title, titleX, 100);

		// Linha 1
		string line1 = "Complete " + ofToString(maxLevels) + " fases!";
		float line1W = scoreFont.stringWidth(line1);
		float line1X = (projROI.width - line1W) / 2;
		scoreFont.drawString(line1, line1X, 180);

		// Linha 2
		string line2 = "Leve os peixes ate a comida.";
		float line2W = scoreFont.stringWidth(line2);
		float line2X = (projROI.width - line2W) / 2;
		scoreFont.drawString(line2, line2X, 230);
	}

	// Contador - Centralizar
	float timeLeft = introDisplayTime - (ofGetElapsedTimef() - introStartTime);
	string countdown = "O jogo comeca em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	ofSetColor(0, 0, 0);
	scoreFont.drawString(countdown, countdownX, projROI.height - 100);

	// Instrução - Centralizar
	string instruction = "Pressione ESPACO para comecar agora.";
	float instructionW = scoreFont.stringWidth(instruction);
	float instructionX = (projROI.width - instructionW) / 2;
	scoreFont.drawString(instruction, instructionX, projROI.height - 50);

	fboGame.end();
}

void CFeedingGameController::drawLevelCompleteScreen()
{
	fboGame.begin();
	ofClear(50, 50, 0, 200);

	// GERAR EFEITOS APENAS UMA VEZ
	if (!levelCompleteEffectsGenerated) {
		generateConfettiEffects();
		generateStarEffects();
		levelCompleteEffectsGenerated = true;
	}

	// HABILITAR BLEND PARA TRANSPARÊNCIA
	ofEnableAlphaBlending();

	// CAMADA 1: EFEITOS DE FUNDO (mais suaves)
	ofPushStyle();

	// Confete de fundo (mais transparente)
	for (const auto& p : confettiParticles) {
		if (p.position.y > projROI.height * 0.4) {
			ofPushMatrix();
			ofTranslate(p.position);
			ofRotate(p.rotation);

			if (confettiImage.isAllocated()) {
				ofSetColor(p.color, 100);
				confettiImage.draw(-p.size / 2, -p.size / 2, p.size, p.size);
			}
			else {
				ofSetColor(p.color, 80);
				ofDrawRectangle(-p.size / 2, -p.size / 2, p.size, p.size);
			}

			ofPopMatrix();
		}
	}

	// Estrelas aumentadas
	for (const auto& star : starEffects) {
		// Colocar estrelas apenas nas bordas
		if (star.position.x < 100 || star.position.x > projROI.width - 100 ||
			star.position.y < 100 || star.position.y > projROI.height - 200) {
			if (starsImage.isAllocated()) {
				ofSetColor(255, 255, 255, star.alpha * 0.6);
				starsImage.draw(star.position - ofVec2f(star.size / 2, star.size / 2),
					star.size, star.size);
			}
			else {
				ofSetColor(255, 255, 0, star.alpha * 0.6);
				ofDrawCircle(star.position, star.size / 2);
			}
		}
	}
	ofPopStyle();

	// CAMADA 2: MEDALHA DA FASE COMPLETADA
	ofPushStyle();
	ofImage* currentMedal = nullptr;
	string medalText = "";

	// Escolher medalha baseada na fase completada
	if (currentLevel - 1 == 1) { // Fase 1 completada
		currentMedal = &bronzeMedalImage;
		medalText = "TROFEU BRONZE";
	}
	else if (currentLevel - 1 == 2) { // Fase 2 completada
		currentMedal = &silverMedalImage;
		medalText = "TROFEU PRATA";
	}
	else if (currentLevel - 1 == 3) { // Fase 3 completada
		currentMedal = &goldMedalImage;
		medalText = "TROFEU OURO";
	}

	if (currentMedal && currentMedal->isAllocated()) {
		float medalSize = 120;
		float medalX = (projROI.width - medalSize) / 2;
		float medalY = medalYPosition;

		// Fundo semi-transparente atrás da medalha
		ofSetColor(0, 0, 0, 80);
		ofDrawCircle(medalX + medalSize / 2, medalY + medalSize / 2, medalSize * 0.7);

		// Medalha
		ofSetColor(255, 255, 255);
		currentMedal->draw(medalX, medalY, medalSize, medalSize);

		// Texto da medalha
		if (!medalText.empty()) {
			ofSetColor(255, 255, 200);
			scoreFont.drawString(medalText, (projROI.width - scoreFont.stringWidth(medalText)) / 2, medalY + medalSize + 30);
		}
	}
	ofPopStyle();

	// CAMADA 3: TEXTO SEM FUNDO
	ofPushStyle();

	// Posição Y do texto (ajustada para medalha)
	float textBoxY = (currentMedal && currentMedal->isAllocated()) ? medalYPosition + 210 : 230;

	LevelConfig currentConfig = levelConfigs[currentLevel - 2];
	LevelConfig nextConfig = levelConfigs[currentLevel - 1];

	// Título com sombra mais forte para melhor legibilidade
	string title = "FASE " + ofToString(currentLevel - 1) + " SUPERADA!";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	// Sombra do título (mais forte para compensar falta de fundo)
	ofSetColor(0, 0, 0, 220);
	gameFont.drawString(title, titleX + 3, textBoxY + 3);

	// Título principal com cor vibrante (amarelo para combinar com comida)
	ofSetColor(255, 255, 0);
	gameFont.drawString(title, titleX, textBoxY);

	// Linha divisória decorativa sutil
	ofSetColor(255, 200, 0, 80);
	ofDrawLine(projROI.width * 0.2, textBoxY + 45,
		projROI.width * 0.8, textBoxY + 45);

	// Comida coletada
	string collected = "Voce coletou " + ofToString(foodCollected) + " alimentos!";
	float collectedW = scoreFont.stringWidth(collected);
	float collectedX = (projROI.width - collectedW) / 2;

	// Sombra do texto
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(collected, collectedX + 2, textBoxY + 91);

	// Texto principal
	ofSetColor(255, 255, 255);
	scoreFont.drawString(collected, collectedX, textBoxY + 90);

	// Próxima fase
	string nextLevel = "Proximo: " + nextConfig.levelName;
	float nextLevelW = scoreFont.stringWidth(nextLevel);
	float nextLevelX = (projROI.width - nextLevelW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(nextLevel, nextLevelX + 2, textBoxY + 151);

	// Texto principal
	ofSetColor(144, 238, 144);
	scoreFont.drawString(nextLevel, nextLevelX, textBoxY + 150);

	// Preparação
	string prepare = "Prepare-se para:";
	float prepareW = scoreFont.stringWidth(prepare);
	float prepareX = (projROI.width - prepareW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(prepare, prepareX + 2, textBoxY + 211);

	// Texto principal
	ofSetColor(255, 255, 0);
	scoreFont.drawString(prepare, prepareX, textBoxY + 210);

	// Comidas para coletar
	string foodTarget = "- " + ofToString(nextConfig.targetFood) + " Alimentos para coletar";
	float foodTargetW = scoreFont.stringWidth(foodTarget);
	float foodTargetX = (projROI.width - foodTargetW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(foodTarget, foodTargetX + 2, textBoxY + 261);

	// Texto principal
	ofSetColor(255, 255, 200);
	scoreFont.drawString(foodTarget, foodTargetX, textBoxY + 260);

	// Peixes famintos
	string fishCount = "- " + ofToString(nextConfig.initialFish) + " peixes famintos";
	float fishCountW = scoreFont.stringWidth(fishCount);
	float fishCountX = (projROI.width - fishCountW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(fishCount, fishCountX + 2, textBoxY + 311);

	// Texto principal
	ofSetColor(200, 200, 255);
	scoreFont.drawString(fishCount, fishCountX, textBoxY + 310);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ
	ofPushStyle();

	// Fundo sutil para o contador
	ofSetColor(0, 0, 0, 80);
	ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

	float timeLeft = levelTransitionDuration - (ofGetElapsedTimef() - levelTransitionStartTime);
	string countdown = "Proxima fase em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	// Sombra do contador
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(countdown, countdownX + 1, projROI.height - 21);

	// Contador principal
	ofSetColor(255, 255, 255);
	scoreFont.drawString(countdown, countdownX, projROI.height - 22);

	ofPopStyle();

	ofDisableAlphaBlending();
	fboGame.end();
}

void CFeedingGameController::drawVictoryScreen()
{
	fboGame.begin();
	ofClear(0, 50, 0, 200);

	// GERAR EFEITOS DE VITÓRIA APENAS UMA VEZ
	if (!victoryEffectsGenerated) {
		generateVictoryEffects();
	}

	// HABILITAR BLEND PARA TRANSPARÊNCIA
	ofEnableAlphaBlending();

	// CAMADA 1: EFEITOS DE FUNDO
	ofPushStyle();

	// Confete
	for (const auto& p : confettiParticles) {
		ofPushMatrix();
		ofTranslate(p.position);
		ofRotate(p.rotation);

		if (confettiImage.isAllocated()) {
			ofSetColor(p.color);
			confettiImage.draw(-p.size / 2, -p.size / 2, p.size, p.size);
		}
		else {
			ofSetColor(p.color, 200);
			ofDrawRectangle(-p.size / 2, -p.size / 2, p.size, p.size);
		}

		ofPopMatrix();
	}

	// Estrelas
	for (const auto& star : starEffects) {
		if (starsImage.isAllocated()) {
			ofSetColor(255, 255, 255, star.alpha * 0.8);
			starsImage.draw(star.position - ofVec2f(star.size / 2, star.size / 2),
				star.size, star.size);
		}
		else {
			ofSetColor(255, 255, 0, star.alpha * 0.8);
			ofDrawCircle(star.position, star.size / 2);
		}
	}

	// Fogos de artifício
	if (fireworksImage.isAllocated()) {
		float time = ofGetElapsedTimef();
		for (int i = 0; i < 5; i++) {
			float x = 100 + i * 200;
			float y = 80 + 50 * sin(time * 2 + i);
			float size = 80 + 20 * sin(time * 3 + i);
			float alpha = 150 + 105 * sin(time * 4 + i);

			ofSetColor(255, 255, 255, alpha);
			fireworksImage.draw(x, y, size, size);
		}
	}
	ofPopStyle();

	// CAMADA 2: TROFÉU GRANDE PARA VITÓRIA FINAL
	ofPushStyle();
	if (trophyImage.isAllocated()) {
		float trophySize = 180; // Troféu grande
		float trophyX = (projROI.width - trophySize) / 2;
		float trophyY = trophyYPosition; // Posição ajustável

										 // Troféu com efeito de pulso
		float pulse = 0.8 + 0.2 * sin(ofGetElapsedTimef() * 3);
		ofSetColor(255, 255, 255);
		trophyImage.draw(trophyX, trophyY, trophySize, trophySize);
	}
	ofPopStyle();

	// CAMADA 3: TEXTO DE VITÓRIA (SEM FUNDOS)
	ofPushStyle();

	// Posição Y do texto
	float textBoxY = trophyImage.isAllocated() ? trophyYPosition + 230 : 150;

	// Título com efeito de pulso
	string title = "VITORIA!";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	float pulse = 0.5 + 0.5 * sin(ofGetElapsedTimef() * 3);

	// Sombra para melhor legibilidade
	ofSetColor(0, 0, 0, 220);
	gameFont.drawString(title, titleX + 3, textBoxY + 3);

	// Título principal (amarelo para combinar com tema de comida)
	ofSetColor(255, 255 * pulse, 0);
	gameFont.drawString(title, titleX, textBoxY);

	// Linha decorativa
	ofSetColor(255, 200, 0, 100);
	ofDrawLine(projROI.width * 0.2, textBoxY + 45,
		projROI.width * 0.8, textBoxY + 45);

	// Mensagem de conclusão
	string completed = "Todas as " + ofToString(maxLevels) + " fases concluidas!";
	float completedW = scoreFont.stringWidth(completed);
	float completedX = (projROI.width - completedW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(completed, completedX + 2, textBoxY + 72);

	// Texto principal
	ofSetColor(255, 255, 200);
	scoreFont.drawString(completed, completedX, textBoxY + 70);

	// Total de comida
	string totalFoodStr = "Total de alimentos coletados: " + ofToString(totalFood);
	float totalFoodW = scoreFont.stringWidth(totalFoodStr);
	float totalFoodX = (projROI.width - totalFoodW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(totalFoodStr, totalFoodX + 2, textBoxY + 142);

	// Texto principal
	ofSetColor(200, 255, 200);
	scoreFont.drawString(totalFoodStr, totalFoodX, textBoxY + 140);

	// Mensagem de parabéns
	string congrats = "Parabens, voce alimentou todos os peixes!";
	float congratsW = scoreFont.stringWidth(congrats);
	float congratsX = (projROI.width - congratsW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(congrats, congratsX + 2, textBoxY + 212);

	// Texto principal
	ofSetColor(255, 200, 100);
	scoreFont.drawString(congrats, congratsX, textBoxY + 210);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ (SEM FUNDO)
	ofPushStyle();

	float timeLeft = resultsDisplayTime - (ofGetElapsedTimef() - resultsStartTime);
	string countdown = "Fim de jogo em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	// Sombra do contador
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(countdown, countdownX + 1, projROI.height - 21);

	// Contador principal
	ofSetColor(255, 255, 255);
	scoreFont.drawString(countdown, countdownX, projROI.height - 22);

	ofPopStyle();

	ofDisableAlphaBlending();
	fboGame.end();
}

void CFeedingGameController::drawTryAgainScreen()
{
	fboGame.begin();
	ofClear(50, 0, 0, 200);

	// Título centralizado
	string title = "GAME OVER";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	ofSetColor(255, 100, 100);
	gameFont.drawString(title, titleX, 150);

	// Mensagem da fase centralizada
	string levelFailed = "Fase " + ofToString(currentLevel) + " Fracassada!";
	float levelFailedW = scoreFont.stringWidth(levelFailed);
	float levelFailedX = (projROI.width - levelFailedW) / 2;

	ofSetColor(255, 255, 255);
	scoreFont.drawString(levelFailed, levelFailedX, 250);

	// Coletas centralizada
	string foodStats = "Alimentos coletados: " + ofToString(foodCollected) + " / " + ofToString(targetFood);
	float foodStatsW = scoreFont.stringWidth(foodStats);
	float foodStatsX = (projROI.width - foodStatsW) / 2;
	scoreFont.drawString(foodStats, foodStatsX, 300);

	// Motivação centralizada
	string motivation = "Vamos Tentar novamente!";
	float motivationW = scoreFont.stringWidth(motivation);
	float motivationX = (projROI.width - motivationW) / 2;

	ofSetColor(255, 255, 0);
	scoreFont.drawString(motivation, motivationX, 350);

	// Dica centralizada
	string dica = "Dica: Crie caminhos para levar os peixes a comida !";
	float dicaW = scoreFont.stringWidth(dica);
	float dicaX = (projROI.width - dicaW) / 2;

	ofSetColor(255, 255, 0);
	scoreFont.drawString(dica, dicaX, 400);

	// Contador centralizado
	float timeLeft = resultsDisplayTime - (ofGetElapsedTimef() - resultsStartTime);
	string countdown = "Fim de jogo em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	ofSetColor(255, 255, 255, 150);
	scoreFont.drawString(countdown, countdownX, projROI.height - 50);

	fboGame.end();
}

void CFeedingGameController::drawProjectorWindow()
{
	if (currentState == STATE_INTRO) {
		drawIntroScreen();
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else if (currentState == STATE_LEVEL_COMPLETE) {
		//drawLevelCompleteScreen();
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else if (currentState == STATE_SHOWING_RESULTS) {
		if (victory) {
			//drawVictoryScreen();
		}
		else {
			//drawTryAgainScreen();
		}
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		fboGame.draw(0, 0);
	}
}

void CFeedingGameController::drawMainWindow(float x, float y, float width, float height)
{
	if (currentState == STATE_INTRO) {
		drawIntroScreen();
		fboGame.draw(x, y, width, height);
	}
	else if (currentState == STATE_LEVEL_COMPLETE) {
		drawLevelCompleteScreen();
		fboGame.draw(x, y, width, height);
	}
	else if (currentState == STATE_SHOWING_RESULTS) {
		if (victory) {
			drawVictoryScreen();
		}
		else {
			drawTryAgainScreen();
		}
		fboGame.draw(x, y, width, height);
	}
	else {
		fboGame.draw(x, y, width, height);
	}
}

bool CFeedingGameController::StartGame()
{
	if (currentState != STATE_IDLE) return false;

	currentState = STATE_INTRO;
	introStartTime = ofGetElapsedTimef();

	// RESETA SISTEMA DE NÍVEIS
	currentLevel = 1;
	levelCompleted = false;
	applyLevelConfig(currentLevel);

	// LIMPAR EFEITOS VISUAIS
	clearVisualEffects();

	return true;
}

bool CFeedingGameController::isIdle()
{
	return currentState == STATE_IDLE;
}

bool CFeedingGameController::isInIntro()
{
	return currentState == STATE_INTRO;
}

void CFeedingGameController::startFromIntro()
{
	if (currentState == STATE_INTRO) {
		currentState = STATE_PLAYING;
		gameStartTime = ofGetElapsedTimef();
		levelStartTime = gameStartTime;
		lastFoodSpawnTime = gameStartTime;
		spawnInitialFish();
		// Limpar efeitos visuais
		clearVisualEffects();
	}
}

void CFeedingGameController::goBackToIdle()
{
	resetGame();
}

void CFeedingGameController::setProjectorRes(ofVec2f& PR)
{
	projRes = PR;
	fboGame.allocate(projRes.x, projRes.y, GL_RGBA);
	fboGame.begin();
	ofClear(0, 0, 0, 255);
	fboGame.end();

	projROI = ofRectangle(0, 0, projRes.x, projRes.y);
}

void CFeedingGameController::setKinectRes(ofVec2f& KR)
{
	kinectRes = KR;
}

void CFeedingGameController::setKinectROI(ofRectangle &KROI)
{
	kinectROI = KROI;
}

void CFeedingGameController::resetGame()
{
	currentState = STATE_IDLE;
	foodCollected = 0;
	totalFood = 0;
	foodItems.clear();
	fish.clear();

	// LIMPAR EFEITOS VISUAIS
	clearVisualEffects();

	// RESETA NÍVEIS
	currentLevel = 1;
	levelCompleted = false;
	applyLevelConfig(currentLevel);
}

void CFeedingGameController::addNewFish()
{
	ofVec2f location;
	if (setRandomFishLocation(location)) {
		auto f = Fish(kinectProjector, location, kinectROI, ofVec2f(0, 0));
		f.setup();
		fish.push_back(f);
	}
}

bool CFeedingGameController::setRandomFishLocation(ofVec2f &location)
{
	double W = kinectROI.getWidth() * 0.60;
	double H = kinectROI.getHeight() * 0.60;
	double X = kinectROI.getLeft() + 0.20 * W;
	double Y = kinectROI.getTop() + 0.20 * H;
	ofRectangle fishROI(X, Y, W, H);

	bool okwater = false;
	int count = 0;
	int maxCount = 100;
	while (!okwater && count < maxCount) {
		count++;
		float x = ofRandom(fishROI.getLeft(), fishROI.getRight());
		float y = ofRandom(fishROI.getTop(), fishROI.getBottom());
		bool insideWater = kinectProjector->elevationAtKinectCoord(x, y) < 0;
		if (insideWater) {
			location = ofVec2f(x, y);
			okwater = true;
		}
	}
	return okwater;
}

bool CFeedingGameController::setRandomFoodLocation(ofVec2f &location)
{
	double W = kinectROI.getWidth() * 0.80;
	double H = kinectROI.getHeight() * 0.80;
	double X = kinectROI.getLeft() + 0.10 * W;
	double Y = kinectROI.getTop() + 0.10 * H;
	ofRectangle foodROI(X, Y, W, H);

	bool okwater = false;
	int count = 0;
	int maxCount = 100;
	while (!okwater && count < maxCount) {
		count++;
		float x = ofRandom(foodROI.getLeft(), foodROI.getRight());
		float y = ofRandom(foodROI.getTop(), foodROI.getBottom());
		bool insideWater = kinectProjector->elevationAtKinectCoord(x, y) < 0;
		if (insideWater) {
			location = ofVec2f(x, y);
			okwater = true;
		}
	}
	return okwater;
}