/***********************************************************************
SurvivalGameController.cpp - Controller for a Survival animal game
***********************************************************************/

#include "SurvivalGameController.h"

CSurvivalGameController::CSurvivalGameController()
{
	currentState = STATE_IDLE;

	// SISTEMA DE NÍVEIS
	currentLevel = 1;
	maxLevels = 3;
	levelCompleted = false;
	maxSharks = 3;

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

							  // CARREGAR IMAGEM DO SPLASH SCREEN
	std::string splashScreenFname = "boidgame/art/SurvivalGameSplashScreen.png";
	if (!splashScreen.load(splashScreenFname))
	{
		ofLogError("CSurvivalGameController") << "Could not load splash screen: " << splashScreenFname;
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
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar confetti.png";
		allImagesLoaded = false;
	}

	if (!starsImage.load(starsPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar stars.png";
		allImagesLoaded = false;
	}

	if (!trophyImage.load(trophyPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar trophy.png";
		allImagesLoaded = false;
	}

	if (!bronzeMedalImage.load(bronzeMedalPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar bronze_medal.png";
		// Fallback: criar medalha bronze básica
		bronzeMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		bronzeMedalImage.getPixels().setColor(ofColor(205, 127, 50)); // Bronze
		bronzeMedalImage.update();
	}

	if (!silverMedalImage.load(silverMedalPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar silver_medal.png";
		// Fallback: criar medalha prata básica
		silverMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		silverMedalImage.getPixels().setColor(ofColor(192, 192, 192)); // Prata
		silverMedalImage.update();
	}

	if (!goldMedalImage.load(goldMedalPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar gold_medal.png";
		// Fallback: criar medalha ouro básica
		goldMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		goldMedalImage.getPixels().setColor(ofColor(255, 215, 0)); // Ouro
		goldMedalImage.update();
	}

	if (!fireworksImage.load(fireworksPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar fireworks.png";
		allImagesLoaded = false;
	}

	if (!allImagesLoaded) {
		ofLogWarning("CSurvivalGameController") << "Algumas imagens de efeitos não foram carregadas. Usando fallbacks.";
	}

	// DEBUG: mostrar status das imagens
	debugPrintImageStatus();

	// INICIALIZAR SISTEMA DE PARTÍCULAS
	confettiParticles.clear();
	starEffects.clear();
}

CSurvivalGameController::~CSurvivalGameController()
{
}

void CSurvivalGameController::debugPrintImageStatus()
{
	std::cout << "=== Status das Imagens de Efeitos ===" << std::endl;
	std::cout << "Confetti: " << (confettiImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Stars: " << (starsImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Trophy: " << (trophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Bronze Medal: " << (bronzeMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Silver Medal: " << (silverMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Gold Medal: " << (goldMedalImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Fireworks: " << (fireworksImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "=====================================" << std::endl;
}

void CSurvivalGameController::setupLevels()
{
	levelConfigs.clear();

	// 1 == Fase; 12 == Quant de Peixes; 	// 5 == Quant de Comida; 3.0f == Tempo de spawn da comida
	// 65.0f == Tempo de Duração da Fase.

	// NÍVEL 1 - FÁCIL
	levelConfigs.push_back({ 1, 10, 1, 7.0f, 10.0f, "A Calmaria Inicial" });

	// NÍVEL 2 - MÉDIO
	levelConfigs.push_back({ 2, 10, 1, 5.0f, 10.0f, "O Perigo se Aproxima" });

	// NÍVEL 3 - DIFÍCIL
	levelConfigs.push_back({ 3, 10, 1, 3.0f, 10.0f, "A Furia dos Tubaroes" });
}

void CSurvivalGameController::applyLevelConfig(int level)
{
	if (level < 1 || level > levelConfigs.size()) return;

	LevelConfig config = levelConfigs[level - 1];

	initialFishCount = config.initialFish;
	maxSharks = config.maxSharks;
	sharkSpawnInterval = config.sharkSpawnRate;
	levelDuration = config.duration;
}

void CSurvivalGameController::goToNextLevel()
{
	currentLevel++;

	if (currentLevel > maxLevels) {
		// JOGO COMPLETO - TODOS NÍVEIS CONCLUÍDOS
		currentState = STATE_SHOWING_RESULTS;
		resultsStartTime = ofGetElapsedTimef();
		victory = true;
		// Gerar efeitos de vitória final
		generateVictoryEffects();
		return;
	}

	// VAI PARA TELA DE LEVEL COMPLETE
	currentState = STATE_LEVEL_COMPLETE;
	levelTransitionStartTime = ofGetElapsedTimef();
	// Resetar flag para gerar novos efeitos
	levelCompleteEffectsGenerated = false;
}

void CSurvivalGameController::setup(std::shared_ptr<KinectProjector> const& k)
{
	kinectProjector = k;

	// Setup fonts
	gameFont.load("verdana.ttf", 42);
	scoreFont.load("verdana.ttf", 42);

	resetGame();
}

void CSurvivalGameController::update()
{
	if (currentState == STATE_IDLE) {
		fboGame.begin();
		ofClear(0, 0, 0, 0);
		ofSetColor(0, 0, 0, 0);
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

		// VERIFICA SE NÍVEL FOI COMPLETADO
		if (!levelCompleted && levelElapsedTime >= levelDuration) {
			levelCompleted = true;
			goToNextLevel();
			return;
		}

		// VERIFICA SE TODOS PEIXES MORRERAM (GAME OVER)
		if (fish.empty()) {
			// Vai direto para tela de derrota
			currentState = STATE_SHOWING_RESULTS;
			resultsStartTime = currentTime;
			victory = false;
			clearVisualEffects();
			return;
		}

		// SPAWN DE TUBARÕES (só se nível não está completo)
		if (!levelCompleted && currentTime - lastSharkSpawnTime > sharkSpawnInterval) {
			spawnShark();
			lastSharkSpawnTime = currentTime;
		}

		// Update do jogo
		updateGameState();

		// Draw para FBO
		fboGame.begin();
		ofClear(0, 0, 0, 0);
		drawGameInfo();

		// Draw animals
		for (auto &f : fish) {
			f.draw();
		}
		for (auto &s : sharks) {
			s.draw();
		}
		fboGame.end();
	}

	// Estado de nível completo
	if (currentState == STATE_LEVEL_COMPLETE) {
		// Atualizar efeitos visuais
		updateVisualEffects();

		float currentTime = ofGetElapsedTimef();
		if (currentTime - levelTransitionStartTime > levelTransitionDuration) {
			// PREPARA PRÓXIMO NÍVEL
			applyLevelConfig(currentLevel);
			levelCompleted = false;
			levelStartTime = ofGetElapsedTimef();

			// RESETA ANIMAIS PARA NOVO NÍVEL
			spawnInitialFish();
			sharks.clear();
			dangerBOIDS.clear();
			sharksSpawned = 0;
			lastSharkSpawnTime = ofGetElapsedTimef();

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

void CSurvivalGameController::updateGameState()
{
	if (kinectProjector->isImageStabilized()) {
		// Update fish
		for (auto &f : fish) {
			f.applyBehaviours(false, fish, dangerBOIDS);
			f.update();
		}

		// Update sharks
		dangerBOIDS.clear();
		for (auto &s : sharks) {
			s.applyBehaviours(fish);
			s.update();
			dangerBOIDS.push_back(DangerousBOID(s.getLocation(), s.getVelocity(), s.getSize() * 4));
		}

		checkFishSurvival();
	}
}

void CSurvivalGameController::checkFishSurvival()
{
	// Percorre os peixes de trás para frente.
	for (int i = fish.size() - 1; i >= 0; i--) {

		// Verifica todos os tubarões do cenário.
		for (auto &shark : sharks) {

			// Calcula a distância entre o peixe e o tubarão.
			// getLocation() retorna um vetor de posição.
			// length() devolve a distância resultante.
			float distance = (fish[i].getLocation() - shark.getLocation()).length();

			// Critério de colisão:
			// Se a distância entre os centros for menor que a metade
			// da soma dos tamanhos, considera que o tubarão alcançou o peixe.
			if (distance < (fish[i].getSize() + shark.getSize()) / 2) {

				// Remove o peixe devorado do vetor.
				fish.erase(fish.begin() + i);

				// Atualiza o número de peixes sobreviventes.
				fishSurvived--;

				// Interrompe o loop de tubarões, pois o peixe já foi removido.
				break;
			}
		}
	}
}


void CSurvivalGameController::spawnInitialFish()
{
	fish.clear();
	for (int i = 0; i < initialFishCount; i++) {
		addNewFish();
	}
	fishSurvived = initialFishCount;
}

void CSurvivalGameController::spawnShark()
{
	// VERIFICA SE JÁ ATINGIU O LIMITE MÁXIMO
	if (sharks.size() >= maxSharks) {
		return; // Não spawna mais tubarões
	}

	ofVec2f location;
	if (setRandomFishLocation(location)) {
		auto s = Shark(kinectProjector, location, kinectROI, ofVec2f(0, 0));
		s.setup();
		sharks.push_back(s);
		sharksSpawned++;
	}
}

void CSurvivalGameController::drawGameInfo()
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
	std::string fishStr = "Peixes: " + ofToString(fishSurvived) + " / " + ofToString(initialFishCount);
	std::string sharkStr = "Tubaroes: " + ofToString(sharks.size()) + " / " + ofToString(maxSharks);

	// ===========================================
	//   LINHA 1: CENTRALIZAR FASE
	// ===========================================
	float yLine1 = 80; // Altura da primeira linha
	float wLevel = gameFont.stringWidth(levelStr);
	float xLevel = (ofGetWidth() - wLevel) / 2;

	gameFont.drawString(levelStr, xLevel, yLine1);

	// ===========================================
	//   LINHA 2: TEMPO, PEIXES E TUBARÕES LADO A LADO
	// ===========================================
	float yLine2 = 140; // Altura da segunda linha
	float spacing = 60; // Espaçamento entre elementos

	float wTime = scoreFont.stringWidth(timeStr);
	float wFish = scoreFont.stringWidth(fishStr);
	float wShark = scoreFont.stringWidth(sharkStr);

	float totalWidth = wTime + wFish + wShark + spacing * 2;
	float startX = (ofGetWidth() - totalWidth) / 2;

	// Desenhar Tempo
	scoreFont.drawString(timeStr, startX, yLine2);

	// Desenhar Peixes
	scoreFont.drawString(fishStr, startX + wTime + spacing, yLine2);

	// Desenhar Tubarões
	scoreFont.drawString(sharkStr, startX + wTime + wFish + spacing * 2, yLine2);

	// Linha divisória opcional
	ofSetColor(255, 255, 255, 50);
	ofDrawLine(0, yLine1 + 20, ofGetWidth(), yLine1 + 20);
}

void CSurvivalGameController::drawIntroScreen()
{
	fboGame.begin();
	ofClear(0, 0, 0, 200);

	if (splashScreen.isAllocated()) {
		splashScreen.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		// Fallback com texto
		// Título centralizado
		string title = "JOGO DE SOBREVIVENCIA";
		float titleW = gameFont.stringWidth(title);
		float titleX = (projROI.width - titleW) / 2;

		ofSetColor(255, 255, 255);
		gameFont.drawString(title, titleX, 100);

		// Linha 1
		string line1 = "Complete " + ofToString(maxLevels) + " fases!";
		float line1W = scoreFont.stringWidth(line1);
		float line1X = (projROI.width - line1W) / 2;
		scoreFont.drawString(line1, line1X, 180);

		// Linha 2
		string line2 = "Proteja os peixes dos tubaroes.";
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

void CSurvivalGameController::generateConfettiEffects()
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

														 // Cores mais suaves
		int colorType = ofRandom(5);
		switch (colorType) {
		case 0: p.color = ofColor(255, 100, 100); break;  // Vermelho suave
		case 1: p.color = ofColor(100, 255, 100); break;  // Verde suave
		case 2: p.color = ofColor(100, 100, 255); break;  // Azul suave
		case 3: p.color = ofColor(255, 255, 100); break; // Amarelo suave
		case 4: p.color = ofColor(200, 200, 255); break; // Branco azulado
		}

		p.size = ofRandom(8, 15); // Tamanho menor
		p.rotation = ofRandom(0, 360);
		p.rotationSpeed = ofRandom(-3, 3); // Rotação mais lenta

		confettiParticles.push_back(p);
	}
}

void CSurvivalGameController::generateStarEffects()
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

void CSurvivalGameController::generateVictoryEffects()
{
	// Gerar confete básico
	generateConfettiEffects();

	// Adicionar mais confete para vitória
	for (int i = 0; i < 100; i++) {
		ConfettiParticle p;
		p.position.set(ofRandom(0, projROI.width),
			ofRandom(0, projROI.height));
		p.velocity.set(ofRandom(-3, 3), ofRandom(-3, 3));
		p.color = ofColor(ofRandom(200, 255), ofRandom(200, 255), 50); // Dourado
		p.size = ofRandom(15, 30);
		p.rotation = ofRandom(0, 360);
		p.rotationSpeed = ofRandom(-8, 8);
		confettiParticles.push_back(p);
	}

	// Gerar estrelas
	generateStarEffects();
	victoryEffectsGenerated = true;
}

void CSurvivalGameController::updateVisualEffects()
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

void CSurvivalGameController::clearVisualEffects()
{
	confettiParticles.clear();
	starEffects.clear();
	levelCompleteEffectsGenerated = false;
	victoryEffectsGenerated = false;
}

void CSurvivalGameController::drawLevelCompleteScreen()
{
	fboGame.begin();
	ofClear(0, 50, 50, 200);

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

		// Fundo semi-transparente atrás da medalha (mantido apenas para a medalha)
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

	// CAMADA 3: TEXTO SEM FUNDO (mais limpo)
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

	// Título principal com cor vibrante
	ofSetColor(0, 255, 255);
	gameFont.drawString(title, titleX, textBoxY);

	// Linha divisória decorativa sutil
	ofSetColor(0, 255, 255, 80);
	ofDrawLine(projROI.width * 0.2, textBoxY + 45,
		projROI.width * 0.8, textBoxY + 45);

	// Peixes salvos
	string saved = "Voce salvou " + ofToString(fishSurvived) + " peixes!";
	float savedW = scoreFont.stringWidth(saved);
	float savedX = (projROI.width - savedW) / 2;

	// Sombra do texto
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(saved, savedX + 2, textBoxY + 91);

	// Texto principal
	ofSetColor(255, 255, 255);
	scoreFont.drawString(saved, savedX, textBoxY + 90);

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

	// Peixes para proteger
	string fishProtect = "- " + ofToString(nextConfig.initialFish) + " peixes para proteger";
	float fishProtectW = scoreFont.stringWidth(fishProtect);
	float fishProtectX = (projROI.width - fishProtectW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(fishProtect, fishProtectX + 2, textBoxY + 261);

	// Texto principal
	ofSetColor(200, 255, 200);
	scoreFont.drawString(fishProtect, fishProtectX, textBoxY + 260);

	// Máximo de tubarões
	string maxSharks = "- " + ofToString(nextConfig.maxSharks) + " Maximo de tubaroes";
	float maxSharksW = scoreFont.stringWidth(maxSharks);
	float maxSharksX = (projROI.width - maxSharksW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(maxSharks, maxSharksX + 2, textBoxY + 311);

	// Texto principal
	ofSetColor(200, 200, 255);
	scoreFont.drawString(maxSharks, maxSharksX, textBoxY + 310);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ
	ofPushStyle();

	// Fundo para o contador (mantido para melhor visibilidade)
	ofSetColor(0, 0, 0, 120);
	ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

	// Linha decorativa acima do contador
	ofSetColor(255, 255, 255, 80);
	ofDrawLine(0, projROI.height - 70, projROI.width, projROI.height - 70);

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

void CSurvivalGameController::drawVictoryScreen()
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
	gameFont.drawString(title, titleX + 2, textBoxY + 2);

	// Título principal
	ofSetColor(0, 255 * pulse, 0);
	gameFont.drawString(title, titleX, textBoxY);

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

	// Peixes salvos
	string fishSaved = "Total de peixes salvos: " + ofToString(fishSurvived);
	float fishSavedW = scoreFont.stringWidth(fishSaved);
	float fishSavedX = (projROI.width - fishSavedW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(fishSaved, fishSavedX + 2, textBoxY + 142);

	// Texto principal
	ofSetColor(200, 255, 200);
	scoreFont.drawString(fishSaved, fishSavedX, textBoxY + 140);

	// Mensagem de parabéns
	string congrats = "Parabens, voce e um heroi dos mares!";
	float congratsW = scoreFont.stringWidth(congrats);
	float congratsX = (projROI.width - congratsW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(congrats, congratsX + 2, textBoxY + 212);

	// Texto principal
	ofSetColor(255, 200, 100);
	scoreFont.drawString(congrats, congratsX, textBoxY + 210);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ (mantido fundo sutil apenas para o contador)
	ofPushStyle();

	// Fundo sutil para o contador (opcional, pode remover se quiser)
	ofSetColor(0, 0, 0, 80);
	ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

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

void CSurvivalGameController::drawDefeatScreen()
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
	string levelFailed = "Fase " + ofToString(currentLevel) + " Fracassada";
	float levelFailedW = scoreFont.stringWidth(levelFailed);
	float levelFailedX = (projROI.width - levelFailedW) / 2;

	ofSetColor(255, 255, 255);
	scoreFont.drawString(levelFailed, levelFailedX, 250);

	// Peixes vivos centralizado
	string fishAlive = ofToString(fishSurvived) + " Peixes Vivos";
	float fishAliveW = scoreFont.stringWidth(fishAlive);
	float fishAliveX = (projROI.width - fishAliveW) / 2;
	scoreFont.drawString(fishAlive, fishAliveX, 300);

	// Motivação centralizada
	string motivation = "Vamos Tentar Novamente!!";
	float motivationW = scoreFont.stringWidth(motivation);
	float motivationX = (projROI.width - motivationW) / 2;

	ofSetColor(255, 255, 0);
	scoreFont.drawString(motivation, motivationX, 350);

	// Dica centralizada
	string tip = "Dica: Crie zonas seguras com barreiras de areia.";
	float tipW = scoreFont.stringWidth(tip);
	float tipX = (projROI.width - tipW) / 2;

	ofSetColor(200, 200, 255);
	scoreFont.drawString(tip, tipX, 400);

	// Contador centralizado
	float timeLeft = resultsDisplayTime - (ofGetElapsedTimef() - resultsStartTime);
	string countdown = "Fim de jogo em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	ofSetColor(255, 255, 255, 150);
	scoreFont.drawString(countdown, countdownX, projROI.height - 50);

	fboGame.end();
}

void CSurvivalGameController::drawProjectorWindow()
{
	if (currentState == STATE_INTRO) {
		//drawIntroScreen();
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
			//drawDefeatScreen();
		}
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		fboGame.draw(0, 0);
	}
}

void CSurvivalGameController::drawMainWindow(float x, float y, float width, float height)
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
			drawDefeatScreen();
		}
		fboGame.draw(x, y, width, height);
	}
	else {
		fboGame.draw(x, y, width, height);
	}
}

bool CSurvivalGameController::StartGame()
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

bool CSurvivalGameController::isIdle()
{
	return currentState == STATE_IDLE;
}

bool CSurvivalGameController::isInIntro()
{
	return currentState == STATE_INTRO;
}

void CSurvivalGameController::startFromIntro()
{
	if (currentState == STATE_INTRO) {
		currentState = STATE_PLAYING;
		gameStartTime = ofGetElapsedTimef();
		levelStartTime = gameStartTime;
		lastSharkSpawnTime = gameStartTime;
		spawnInitialFish();
		// Limpar efeitos visuais
		clearVisualEffects();
	}
}

void CSurvivalGameController::goBackToIdle()
{
	resetGame();
}

void CSurvivalGameController::setProjectorRes(ofVec2f& PR)
{
	projRes = PR;
	fboGame.allocate(projRes.x, projRes.y, GL_RGBA);
	fboGame.begin();
	ofClear(0, 0, 0, 255);
	fboGame.end();

	projROI = ofRectangle(0, 0, projRes.x, projRes.y);
}

void CSurvivalGameController::setKinectRes(ofVec2f& KR)
{
	kinectRes = KR;
}

void CSurvivalGameController::setKinectROI(ofRectangle &KROI)
{
	kinectROI = KROI;
}

void CSurvivalGameController::resetGame()
{
	currentState = STATE_IDLE;
	fishSurvived = 0;
	sharksSpawned = 0;
	sharks.clear();
	fish.clear();
	dangerBOIDS.clear();

	// LIMPAR EFEITOS VISUAIS
	clearVisualEffects();

	// RESETA NÍVEIS
	currentLevel = 1;
	levelCompleted = false;
	applyLevelConfig(currentLevel);
}

void CSurvivalGameController::addNewFish()
{
	ofVec2f location;
	if (setRandomFishLocation(location)) {
		auto f = Fish(kinectProjector, location, kinectROI, ofVec2f(0, 0));
		f.setup();
		fish.push_back(f);
	}
}

bool CSurvivalGameController::setRandomFishLocation(ofVec2f &location)
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