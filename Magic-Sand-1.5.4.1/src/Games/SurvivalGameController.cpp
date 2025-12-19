/***********************************************************************
SurvivalGameController.cpp - Controller for a Survival animal game
MIT License

Copyright (c) 2025 GlT-Ricardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***********************************************************************/

#include "SurvivalGameController.h"

CSurvivalGameController::CSurvivalGameController()
{
	// Inicializa o estado atual do jogo como ocioso
	currentState = STATE_IDLE;

	// SISTEMA DE NÍVEIS
	// Define o nível atual, máximo de níveis, status de conclusão e máximo de tubarões
	currentLevel = 1;
	maxLevels = 3;
	levelCompleted = false;
	maxSharks = 3;

	// Configura os parâmetros de cada nível do jogo
	setupLevels();

	// Aplica as configurações do nível 1 no início do jogo
	applyLevelConfig(1);

	// Define os tempos de exibição para diferentes telas
	resultsDisplayTime = 5.0f;      // Tempo para mostrar resultados
	introDisplayTime = 10.0f;       // Tempo da tela de introdução
	levelTransitionDuration = 5.0f; // Tempo de transição entre níveis

									// CONTROLES DE TAMANHO E POSIÇÃO
	starMinSize = 40.0f;     // Tamanho mínimo das estrelas (efeitos visuais)
	starMaxSize = 100.0f;    // Tamanho máximo das estrelas
	trophyYPosition = 100.0f; // Posição Y da troféu na tela
	trophyYPosition = 120.0f; // Posição Y do troféu na tela

							  // CARREGAR IMAGEM DO SPLASH SCREEN (tela de apresentação)
	std::string splashScreenFname = "boidgame/art/SurvivalGameSplashScreen.png";
	if (!splashScreen.load(splashScreenFname))
	{
		// Registra erro se não conseguir carregar a imagem
		ofLogError("CSurvivalGameController") << "Could not load splash screen: " << splashScreenFname;
	}

	// INICIALIZAR FLAGS DE EFEITOS
	levelCompleteEffectsGenerated = false; // Indica se os efeitos de nível completo já foram gerados
	victoryEffectsGenerated = false;       // Indica se os efeitos de vitória já foram gerados

										   // CARREGAR IMAGENS DE EFEITOS VISUAIS
	std::string confettiPath = "boidgame/art/confetti.png";
	std::string starsPath = "boidgame/art/stars.png";
	std::string trophyPath = "boidgame/art/trophy.png";
	std::string bronzetrophyPath = "boidgame/art/bronze_trophy.png";
	std::string silvertrophyPath = "boidgame/art/silver_trophy.png";
	std::string goldtrophyPath = "boidgame/art/gold_trophy.png";
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

	// Se não conseguir carregar as troféu, cria fallbacks básicos
	if (!bronzetrophyImage.load(bronzetrophyPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar bronze_trophy.png";
		// Fallback: criar troféu bronze básica
		bronzetrophyImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		bronzetrophyImage.getPixels().setColor(ofColor(205, 127, 50)); // Cor bronze
		bronzetrophyImage.update();
	}

	if (!silvertrophyImage.load(silvertrophyPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar silver_trophy.png";
		// Fallback: criar troféu prata básica
		silvertrophyImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		silvertrophyImage.getPixels().setColor(ofColor(192, 192, 192)); // Cor prata
		silvertrophyImage.update();
	}

	if (!goldtrophyImage.load(goldtrophyPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar gold_trophy.png";
		// Fallback: criar troféu ouro básica
		goldtrophyImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
		goldtrophyImage.getPixels().setColor(ofColor(255, 215, 0)); // Cor ouro
		goldtrophyImage.update();
	}

	if (!fireworksImage.load(fireworksPath)) {
		ofLogError("CSurvivalGameController") << "ERRO: Não foi possível carregar fireworks.png";
		allImagesLoaded = false;
	}

	// Avisa se algumas imagens não foram carregadas
	if (!allImagesLoaded) {
		ofLogWarning("CSurvivalGameController") << "Algumas imagens de efeitos não foram carregadas. Usando fallbacks.";
	}

	// DEBUG: mostra status de carregamento de todas as imagens
	debugPrintImageStatus();

	// INICIALIZAR SISTEMA DE PARTÍCULAS
	confettiParticles.clear(); // Limpa partículas de confete
	starEffects.clear();       // Limpa efeitos de estrela
}

CSurvivalGameController::~CSurvivalGameController()
{
	// Destrutor - limpeza de recursos se necessário
}

void CSurvivalGameController::debugPrintImageStatus()
{
	// Imprime no console o status de carregamento de cada imagem
	std::cout << "=== Status das Imagens de Efeitos ===" << std::endl;
	std::cout << "Confetti: " << (confettiImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Stars: " << (starsImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Trophy: " << (trophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Bronze trophy: " << (bronzetrophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Silver trophy: " << (silvertrophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Gold trophy: " << (goldtrophyImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "Fireworks: " << (fireworksImage.isAllocated() ? "OK" : "FALHA") << std::endl;
	std::cout << "=====================================" << std::endl;
}

void CSurvivalGameController::setupLevels()
{
	levelConfigs.clear(); // Limpa configurações anteriores

						  // Formato: {nível, peixes iniciais, tubarões máximos, intervalo de spawn, duração, nome}
						  // NÍVEL 1 - FÁCIL
	levelConfigs.push_back({ 1, 0, 1, 7.0f, 10.0f, "A Calmaria Inicial" });

	// NÍVEL 2 - MÉDIO
	levelConfigs.push_back({ 2, 10, 1, 5.0f, 10.0f, "O Perigo se Aproxima" });

	// NÍVEL 3 - DIFÍCIL
	levelConfigs.push_back({ 3, 10, 1, 3.0f, 10.0f, "A Furia dos Tubaroes" });
}

void CSurvivalGameController::applyLevelConfig(int level)
{
	// Verifica se o nível está dentro dos limites
	if (level < 1 || level > levelConfigs.size()) return;

	// Obtém configuração do nível especificado
	LevelConfig config = levelConfigs[level - 1];

	// Aplica configurações nas variáveis do jogo
	initialFishCount = config.initialFish;
	maxSharks = config.maxSharks;
	sharkSpawnInterval = config.sharkSpawnRate;
	levelDuration = config.duration;
}

void CSurvivalGameController::goToNextLevel()
{
	currentLevel++; // Incrementa para o próximo nível

					// Verifica se completou todos os níveis
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
	kinectProjector = k; // Armazena referência ao KinectProjector

						 // Configura fontes para texto
	gameFont.load("verdana.ttf", 42);   // Fonte para títulos
	scoreFont.load("verdana.ttf", 42);  // Fonte para pontuação

	resetGame(); // Inicializa o jogo
}

void CSurvivalGameController::update()
{
	// Estado ocioso - apenas limpa o FBO
	if (currentState == STATE_IDLE) {
		fboGame.begin();
		ofClear(0, 0, 0, 0); // Limpa com transparência total
		ofSetColor(0, 0, 0, 0);
		fboGame.end();
		return;
	}

	// Estado de introdução - verifica se tempo acabou
	if (currentState == STATE_INTRO) {
		float currentTime = ofGetElapsedTimef();
		if (currentTime - introStartTime > introDisplayTime) {
			startFromIntro(); // Começa o jogo após introdução
		}
		return;
	}

	// Estado de jogo ativo
	if (currentState == STATE_PLAYING) {
		float currentTime = ofGetElapsedTimef();
		float levelElapsedTime = currentTime - levelStartTime;

		// VERIFICA SE NÍVEL FOI COMPLETADO (tempo esgotado)
		if (!levelCompleted && levelElapsedTime >= levelDuration) {
			levelCompleted = true;
			goToNextLevel(); // Avança para próximo nível
			return;
		}

		// VERIFICA SE TODOS PEIXES MORRERAM (GAME OVER)
		if (fish.empty()) {
			// Vai direto para tela de derrota
			currentState = STATE_SHOWING_RESULTS;
			resultsStartTime = currentTime;
			victory = false;
			clearVisualEffects(); // Remove efeitos visuais
			return;
		}

		// SPAWN DE TUBARÕES (só se nível não está completo)
		if (!levelCompleted && currentTime - lastSharkSpawnTime > sharkSpawnInterval) {
			spawnShark(); // Cria novo tubarão
			lastSharkSpawnTime = currentTime;
		}

		// Update do estado do jogo
		updateGameState();

		// Renderiza cena do jogo no FBO
		fboGame.begin();
		ofClear(0, 0, 0, 0); // Limpa com transparência

							 // Desenha informações do jogo (HUD)
		drawGameInfo();

		// Desenha animais na cena
		for (auto &f : fish) {
			f.draw(); // Desenha cada peixe
		}
		for (auto &s : sharks) {
			s.draw(); // Desenha cada tubarão
		}
		fboGame.end();
	}

	// Estado de nível completo
	if (currentState == STATE_LEVEL_COMPLETE) {
		// Atualizar efeitos visuais (confete, estrelas, etc.)
		updateVisualEffects();

		float currentTime = ofGetElapsedTimef();
		// Verifica se tempo de transição acabou
		if (currentTime - levelTransitionStartTime > levelTransitionDuration) {
			// PREPARA PRÓXIMO NÍVEL
			applyLevelConfig(currentLevel);
			levelCompleted = false;
			levelStartTime = ofGetElapsedTimef();

			// RESETA ANIMAIS PARA NOVO NÍVEL
			spawnInitialFish(); // Cria novos peixes
			sharks.clear();     // Remove todos tubarões
			dangerBOIDS.clear(); // Limpa perigos
			sharksSpawned = 0;  // Reseta contador de tubarões
			lastSharkSpawnTime = ofGetElapsedTimef();

			// Limpar efeitos visuais
			clearVisualEffects();
			levelCompleteEffectsGenerated = false;

			currentState = STATE_PLAYING; // Volta para estado de jogo
		}
		return;
	}

	// Estado mostrando resultados (vitória/derrota)
	if (currentState == STATE_SHOWING_RESULTS) {
		// Atualizar efeitos visuais se for vitória
		if (victory) {
			updateVisualEffects();
		}

		float currentTime = ofGetElapsedTimef();
		// Verifica se tempo de exibição acabou
		if (currentTime - resultsStartTime > resultsDisplayTime) {
			resetGame(); // Reinicia o jogo
		}
	}
}

void CSurvivalGameController::updateGameState()
{
	// Só atualiza estado se a imagem do Kinect estiver estável
	if (kinectProjector->isImageStabilized()) {
		// Update fish - aplica comportamentos e atualiza posição
		for (auto &f : fish) {
			f.applyBehaviours(false, fish, dangerBOIDS);
			f.update();
		}

		// Update sharks - aplica comportamentos e atualiza posição
		dangerBOIDS.clear();
		for (auto &s : sharks) {
			s.applyBehaviours(fish);
			s.update();
			// Adiciona tubarão como perigo para os peixes evitarem
			dangerBOIDS.push_back(DangerousBOID(s.getLocation(), s.getVelocity(), s.getSize() * 4));
		}

		// Verifica colisões entre peixes e tubarões
		checkFishSurvival();
	}
}

void CSurvivalGameController::checkFishSurvival()
{
	// Percorre os peixes de trás para frente (para evitar problemas ao remover)
	for (int i = fish.size() - 1; i >= 0; i--) {

		// Verifica todos os tubarões do cenário
		for (auto &shark : sharks) {

			// Calcula a distância entre o peixe e o tubarão
			float distance = (fish[i].getLocation() - shark.getLocation()).length();

			// Critério de colisão: se a distância for menor que a soma dos raios
			if (distance < (fish[i].getSize() + shark.getSize()) / 2) {

				// Remove o peixe devorado do vetor
				fish.erase(fish.begin() + i);

				// Atualiza o número de peixes sobreviventes
				fishSurvived--;

				// Interrompe o loop de tubarões, pois o peixe já foi removido
				break;
			}
		}
	}
}

void CSurvivalGameController::spawnInitialFish()
{
	fish.clear(); // Remove peixes existentes
				  // Cria quantidade inicial de peixes
	for (int i = 0; i < initialFishCount; i++) {
		addNewFish(); // Adiciona novo peixe
	}
	fishSurvived = initialFishCount; // Inicializa contador
}

void CSurvivalGameController::spawnShark()
{
	// VERIFICA SE JÁ ATINGIU O LIMITE MÁXIMO
	if (sharks.size() >= maxSharks) {
		return; // Não spawna mais tubarões
	}

	ofVec2f location;
	// Tenta encontrar posição válida para spawn
	if (setRandomFishLocation(location)) {
		// Cria novo tubarão na posição encontrada
		auto s = Shark(kinectProjector, location, kinectROI, ofVec2f(0, 0));
		s.setup();
		sharks.push_back(s);
		sharksSpawned++; // Incrementa contador
	}
}

void CSurvivalGameController::drawGameInfo()
{
	float currentTime = ofGetElapsedTimef();
	float levelElapsedTime = currentTime - levelStartTime;
	float levelTimeLeft = levelDuration - levelElapsedTime;

	ofSetColor(0, 0, 0); // Cor preta para texto

						 // Obtém configuração do nível atual
	LevelConfig config = levelConfigs[currentLevel - 1];

	// Textos para exibição no HUD
	std::string levelStr = "Fase " + ofToString(currentLevel) + ": " + config.levelName;
	std::string timeStr = "Tempo: " + ofToString((int)levelTimeLeft) + "s";
	std::string fishStr = "Peixes: " + ofToString(fishSurvived) + " / " + ofToString(initialFishCount);
	std::string sharkStr = "Tubaroes: " + ofToString(sharks.size()) + " / " + ofToString(maxSharks);

	// ===========================================
	//   LINHA 1: CENTRALIZAR FASE
	// ===========================================
	float yLine1 = 80; // Altura da primeira linha
	float wLevel = gameFont.stringWidth(levelStr);
	float xLevel = (ofGetWidth() - wLevel) / 2; // Centraliza horizontalmente

	gameFont.drawString(levelStr, xLevel, yLine1);

	// ===========================================
	//   LINHA 2: TEMPO, PEIXES E TUBARÕES LADO A LADO
	// ===========================================
	float yLine2 = 140; // Altura da segunda linha
	float spacing = 60; // Espaçamento entre elementos

						// Calcula larguras dos textos
	float wTime = scoreFont.stringWidth(timeStr);
	float wFish = scoreFont.stringWidth(fishStr);
	float wShark = scoreFont.stringWidth(sharkStr);

	// Calcula posicionamento para centralizar grupo
	float totalWidth = wTime + wFish + wShark + spacing * 2;
	float startX = (ofGetWidth() - totalWidth) / 2;

	// Desenhar Tempo
	scoreFont.drawString(timeStr, startX, yLine2);

	// Desenhar Peixes
	scoreFont.drawString(fishStr, startX + wTime + spacing, yLine2);

	// Desenhar Tubarões
	scoreFont.drawString(sharkStr, startX + wTime + wFish + spacing * 2, yLine2);

	// Linha divisória opcional (visual)
	ofSetColor(255, 255, 255, 50); // Branco semi-transparente
	ofDrawLine(0, yLine1 + 20, ofGetWidth(), yLine1 + 20);
}

void CSurvivalGameController::drawIntroScreen()
{
	fboGame.begin();
	ofClear(0, 0, 0, 200); // Fundo preto semi-transparente

						   // Desenha splash screen se carregada
	if (splashScreen.isAllocated()) {
		splashScreen.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		// Fallback com texto caso imagem não carregue

		// Título centralizado
		string title = "JOGO DE SOBREVIVENCIA";
		float titleW = gameFont.stringWidth(title);
		float titleX = (projROI.width - titleW) / 2;

		ofSetColor(255, 255, 255); // Branco
		gameFont.drawString(title, titleX, 100);

		// Informações sobre o jogo
		string line1 = "Complete " + ofToString(maxLevels) + " fases!";
		float line1W = scoreFont.stringWidth(line1);
		float line1X = (projROI.width - line1W) / 2;
		scoreFont.drawString(line1, line1X, 180);

		string line2 = "Proteja os peixes dos tubaroes.";
		float line2W = scoreFont.stringWidth(line2);
		float line2X = (projROI.width - line2W) / 2;
		scoreFont.drawString(line2, line2X, 230);
	}

	// Contador regressivo - Centralizado
	float timeLeft = introDisplayTime - (ofGetElapsedTimef() - introStartTime);
	string countdown = "O jogo inicia em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	ofSetColor(0, 0, 0); // Preto
	scoreFont.drawString(countdown, countdownX, projROI.height - 100);

	fboGame.end();
}

void CSurvivalGameController::generateConfettiEffects()
{
	confettiParticles.clear(); // Limpa partículas existentes

	int particleCount = 150; // Quantidade de confetes
	for (int i = 0; i < particleCount; i++) {
		ConfettiParticle p;

		// Define posição inicial aleatória
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

														 // Escolhe cor aleatória para o confete
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

		confettiParticles.push_back(p); // Adiciona à lista
	}
}

void CSurvivalGameController::generateStarEffects()
{
	starEffects.clear(); // Limpa estrelas existentes

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
		star.size = ofRandom(starMinSize, starMaxSize); // Tamanho aleatório
		star.alpha = ofRandom(80, 180); // Transparência variável
		star.pulseSpeed = ofRandom(0.3, 1.5); // Velocidade de pulsação

		starEffects.push_back(star); // Adiciona à lista
	}
}

void CSurvivalGameController::generateVictoryEffects()
{
	// Gerar confete básico
	generateConfettiEffects();

	// Adicionar mais confete especial para vitória
	for (int i = 0; i < 100; i++) {
		ConfettiParticle p;
		p.position.set(ofRandom(0, projROI.width),
			ofRandom(0, projROI.height));
		p.velocity.set(ofRandom(-3, 3), ofRandom(-3, 3));
		p.color = ofColor(ofRandom(200, 255), ofRandom(200, 255), 50); // Dourado
		p.size = ofRandom(15, 30); // Maior que confete normal
		p.rotation = ofRandom(0, 360);
		p.rotationSpeed = ofRandom(-8, 8); // Rotação mais rápida
		confettiParticles.push_back(p);
	}

	// Gerar estrelas
	generateStarEffects();
	victoryEffectsGenerated = true; // Marca que efeitos foram gerados
}

void CSurvivalGameController::updateVisualEffects()
{
	// Atualizar partículas de confete
	for (auto& p : confettiParticles) {
		p.position += p.velocity; // Move partícula
		p.rotation += p.rotationSpeed; // Rotaciona

									   // Gravidade - puxa para baixo
		p.velocity.y += 0.1;

		// Reduz velocidade com resistência do ar
		p.velocity *= 0.99;

		// Se sair da parte inferior da tela, resetar para o topo
		if (p.position.y > projROI.height + 50) {
			p.position.y = -50;
			p.position.x = ofRandom(0, projROI.width);
		}
	}

	// Atualizar estrelas piscantes (efeito de pulsação)
	float currentTime = ofGetElapsedTimef();
	for (auto& star : starEffects) {
		// Usa seno para criar efeito de pulsação
		star.alpha = 150 + 105 * sin(currentTime * star.pulseSpeed);
	}
}

void CSurvivalGameController::clearVisualEffects()
{
	// Limpa todas as listas de efeitos visuais
	confettiParticles.clear();
	starEffects.clear();
	// Reseta flags de geração
	levelCompleteEffectsGenerated = false;
	victoryEffectsGenerated = false;
}

void CSurvivalGameController::drawLevelCompleteScreen()
{
	fboGame.begin();
	ofClear(0, 50, 50, 200); // Fundo azul-esverdeado semi-transparente

							 // GERAR EFEITOS APENAS UMA VEZ (para otimização)
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
		if (p.position.y > projROI.height * 0.4) { // Só desenha na parte inferior
			ofPushMatrix();
			ofTranslate(p.position); // Move para posição da partícula
			ofRotate(p.rotation);    // Rotaciona

			if (confettiImage.isAllocated()) {
				ofSetColor(p.color, 100); // Cor com transparência
				confettiImage.draw(-p.size / 2, -p.size / 2, p.size, p.size);
			}
			else {
				// Fallback: quadrado colorido
				ofSetColor(p.color, 80);
				ofDrawRectangle(-p.size / 2, -p.size / 2, p.size, p.size);
			}

			ofPopMatrix();
		}
	}

	// Estrelas aumentadas (apenas nas bordas)
	for (const auto& star : starEffects) {
		// Verifica se está nas bordas da tela
		if (star.position.x < 100 || star.position.x > projROI.width - 100 ||
			star.position.y < 100 || star.position.y > projROI.height - 200) {
			if (starsImage.isAllocated()) {
				ofSetColor(255, 255, 255, star.alpha * 0.6);
				starsImage.draw(star.position - ofVec2f(star.size / 2, star.size / 2),
					star.size, star.size);
			}
			else {
				// Fallback: círculo amarelo
				ofSetColor(255, 255, 0, star.alpha * 0.6);
				ofDrawCircle(star.position, star.size / 2);
			}
		}
	}
	ofPopStyle();

	// CAMADA 2: troféu DA FASE COMPLETADA
	ofPushStyle();
	ofImage* currenttrophy = nullptr;
	string trophyText = "";

	// Escolhe troféu baseada na fase completada
	if (currentLevel - 1 == 1) { // Fase 1 completada
		currenttrophy = &bronzetrophyImage;
	}
	else if (currentLevel - 1 == 2) { // Fase 2 completada
		currenttrophy = &silvertrophyImage;
	}
	else if (currentLevel - 1 == 3) { // Fase 3 completada
		currenttrophy = &goldtrophyImage;
	}

	// Desenha troféu se disponível
	if (currenttrophy && currenttrophy->isAllocated()) {
		float trophySize = 120;
		float trophyX = (projROI.width - trophySize) / 2; // Centraliza
		float trophyY = trophyYPosition;

		// Fundo semi-transparente atrás da troféu
		ofSetColor(0, 0, 0, 80);
		ofDrawCircle(trophyX + trophySize / 2, trophyY + trophySize / 2, trophySize * 0.7);

		// troféu
		ofSetColor(255, 255, 255);
		currenttrophy->draw(trophyX, trophyY, trophySize, trophySize);

		// Texto da troféu (opcional)
		if (!trophyText.empty()) {
			ofSetColor(255, 255, 200);
			scoreFont.drawString(trophyText,
				(projROI.width - scoreFont.stringWidth(trophyText)) / 2,
				trophyY + trophySize + 30);
		}
	}
	ofPopStyle();

	// CAMADA 3: TEXTO SEM FUNDO (mais limpo)
	ofPushStyle();

	// Posição Y do texto (ajustada para troféu)
	float textBoxY = (currenttrophy && currenttrophy->isAllocated()) ?
		trophyYPosition + 210 : 230;

	// Obtém configurações do nível atual e próximo
	LevelConfig currentConfig = levelConfigs[currentLevel - 2];
	LevelConfig nextConfig = levelConfigs[currentLevel - 1];

	// Título com sombra
	string title = "FASE " + ofToString(currentLevel - 1) + " SUPERADA!";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	// Sombra do título (para legibilidade)
	ofSetColor(0, 0, 0, 220);
	gameFont.drawString(title, titleX + 3, textBoxY + 3);

	// Título principal com cor vibrante
	ofSetColor(0, 255, 255);
	gameFont.drawString(title, titleX, textBoxY);

	// Linha divisória decorativa sutil
	ofSetColor(0, 255, 255, 80);
	ofDrawLine(projROI.width * 0.2, textBoxY + 45,
		projROI.width * 0.8, textBoxY + 45);

	// Informações sobre próxima fase
	string prepare = "Prepare-se para:";
	float prepareW = scoreFont.stringWidth(prepare);
	float prepareX = (projROI.width - prepareW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(prepare, prepareX + 2, textBoxY + 151);

	// Texto principal
	ofSetColor(255, 255, 0);
	scoreFont.drawString(prepare, prepareX, textBoxY + 150);

	// Detalhes da próxima fase
	string fishProtect = "- " + ofToString(nextConfig.initialFish) + " peixes para proteger";
	float fishProtectW = scoreFont.stringWidth(fishProtect);
	float fishProtectX = (projROI.width - fishProtectW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(fishProtect, fishProtectX + 2, textBoxY + 201);

	// Texto principal
	ofSetColor(200, 255, 200);
	scoreFont.drawString(fishProtect, fishProtectX, textBoxY + 200);

	string maxSharks = "- " + ofToString(nextConfig.maxSharks) + " predadores";
	float maxSharksW = scoreFont.stringWidth(maxSharks);
	float maxSharksX = (projROI.width - maxSharksW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(maxSharks, maxSharksX + 2, textBoxY + 251);

	// Texto principal
	ofSetColor(200, 200, 255);
	scoreFont.drawString(maxSharks, maxSharksX, textBoxY + 250);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ
	ofPushStyle();

	// Fundo para o contador (para melhor visibilidade)
	ofSetColor(0, 0, 0, 120);
	ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

	// Linha decorativa acima do contador
	ofSetColor(255, 255, 255, 80);
	ofDrawLine(0, projROI.height - 70, projROI.width, projROI.height - 70);

	// Calcula e desenha contador regressivo
	float timeLeft = levelTransitionDuration - (ofGetElapsedTimef() - levelTransitionStartTime);
	string countdown = "Fase seguinte em: " + ofToString((int)timeLeft) + "s";
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
	ofClear(0, 50, 0, 200); // Fundo verde escuro semi-transparente

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
			// Fallback: quadrado colorido
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
			// Fallback: círculo amarelo
			ofSetColor(255, 255, 0, star.alpha * 0.8);
			ofDrawCircle(star.position, star.size / 2);
		}
	}

	// Fogos de artifício animados
	if (fireworksImage.isAllocated()) {
		float time = ofGetElapsedTimef();
		for (int i = 0; i < 5; i++) {
			float x = 100 + i * 200;
			float y = 80 + 50 * sin(time * 2 + i); // Movimento vertical
			float size = 80 + 20 * sin(time * 3 + i); // Pulsação de tamanho
			float alpha = 150 + 105 * sin(time * 4 + i); // Pulsação de transparência

			ofSetColor(255, 255, 255, alpha);
			fireworksImage.draw(x, y, size, size);
		}
	}
	ofPopStyle();

	// CAMADA 2: TROFÉU GRANDE PARA VITÓRIA FINAL
	ofPushStyle();
	if (trophyImage.isAllocated()) {
		float trophySize = 180; // Troféu grande
		float trophyX = (projROI.width - trophySize) / 2; // Centralizado
		float trophyY = trophyYPosition;

		ofSetColor(255, 255, 255);
		trophyImage.draw(trophyX, trophyY, trophySize, trophySize);
	}
	ofPopStyle();

	// CAMADA 3: TEXTO DE VITÓRIA
	ofPushStyle();

	// Posição Y do texto (ajustada para troféu)
	float textBoxY = trophyImage.isAllocated() ? trophyYPosition + 230 : 150;

	// Título com efeito de pulso
	string title = "EXCELENTE!";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	float pulse = 0.5 + 0.5 * sin(ofGetElapsedTimef() * 3); // Valor oscilante 0-1

															// Sombra para melhor legibilidade
	ofSetColor(0, 0, 0, 220);
	gameFont.drawString(title, titleX + 2, textBoxY + 2);

	// Título principal com efeito de pulso verde
	ofSetColor(0, 255 * pulse, 0);
	gameFont.drawString(title, titleX, textBoxY);

	// Mensagem de conclusão
	string completed = "Todas as " + ofToString(maxLevels) + " fases finalizadas!";
	float completedW = scoreFont.stringWidth(completed);
	float completedX = (projROI.width - completedW) / 2;

	// Sombra
	ofSetColor(0, 0, 0, 220);
	scoreFont.drawString(completed, completedX + 2, textBoxY + 72);

	// Texto principal
	ofSetColor(255, 255, 200);
	scoreFont.drawString(completed, completedX, textBoxY + 70);

	ofPopStyle();

	// CAMADA 4: CONTADOR NO RODAPÉ
	ofPushStyle();

	// Fundo sutil para o contador
	ofSetColor(0, 0, 0, 80);
	ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

	// Calcula e desenha contador
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
	ofClear(50, 0, 0, 200); // Fundo vermelho escuro semi-transparente

							// Título centralizado
	string title = "GAME OVER";
	float titleW = gameFont.stringWidth(title);
	float titleX = (projROI.width - titleW) / 2;

	ofSetColor(255, 100, 100); // Vermelho claro
	gameFont.drawString(title, titleX, 150);

	// Mensagem da fase centralizada
	string levelFailed = "Fase " + ofToString(currentLevel) + " Fracassada";
	float levelFailedW = scoreFont.stringWidth(levelFailed);
	float levelFailedX = (projROI.width - levelFailedW) / 2;

	ofSetColor(255, 255, 255); // Branco
	scoreFont.drawString(levelFailed, levelFailedX, 250);

	// Mensagem motivacional
	string motivation = "Vamos Iniciar Novamente!!";
	float motivationW = scoreFont.stringWidth(motivation);
	float motivationX = (projROI.width - motivationW) / 2;

	ofSetColor(255, 255, 0); // Amarelo
	scoreFont.drawString(motivation, motivationX, 350);

	// Contador centralizado
	float timeLeft = resultsDisplayTime - (ofGetElapsedTimef() - resultsStartTime);
	string countdown = "Fim de jogo em: " + ofToString((int)timeLeft) + "s";
	float countdownW = scoreFont.stringWidth(countdown);
	float countdownX = (projROI.width - countdownW) / 2;

	ofSetColor(255, 255, 255, 150); // Branco semi-transparente
	scoreFont.drawString(countdown, countdownX, projROI.height - 50);

	fboGame.end();
}

void CSurvivalGameController::drawProjectorWindow()
{
	// Desenha o FBO na janela do projetor de acordo com o estado atual
	if (currentState == STATE_INTRO) {
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else if (currentState == STATE_LEVEL_COMPLETE) {
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else if (currentState == STATE_SHOWING_RESULTS) {
		if (victory) {
			// Vitória: já desenhada no FBO por drawVictoryScreen
		}
		else {
			// Derrota: já desenhada no FBO por drawDefeatScreen
		}
		fboGame.draw(0, 0, projROI.width, projROI.height);
	}
	else {
		// Estado PLAYING ou outros: desenha FBO normalmente
		fboGame.draw(0, 0);
	}
}

void CSurvivalGameController::drawMainWindow(float x, float y, float width, float height)
{
	// Desenha o FBO na janela principal de acordo com o estado atual
	if (currentState == STATE_INTRO) {
		drawIntroScreen(); // Renderiza tela de introdução
		fboGame.draw(x, y, width, height);
	}
	else if (currentState == STATE_LEVEL_COMPLETE) {
		drawLevelCompleteScreen(); // Renderiza tela de nível completo
		fboGame.draw(x, y, width, height);
	}
	else if (currentState == STATE_SHOWING_RESULTS) {
		if (victory) {
			drawVictoryScreen(); // Renderiza tela de vitória
		}
		else {
			drawDefeatScreen(); // Renderiza tela de derrota
		}
		fboGame.draw(x, y, width, height);
	}
	else {
		// Estado PLAYING: desenha FBO do jogo
		fboGame.draw(x, y, width, height);
	}
}

bool CSurvivalGameController::StartGame()
{
	// Inicia o jogo apenas se estiver no estado IDLE
	if (currentState != STATE_IDLE) return false;

	currentState = STATE_INTRO;
	introStartTime = ofGetElapsedTimef();

	// RESETA SISTEMA DE NÍVEIS
	currentLevel = 1;
	levelCompleted = false;
	applyLevelConfig(currentLevel);

	// LIMPAR EFEITOS VISUAIS
	clearVisualEffects();

	return true; // Sucesso
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
	// Transição do estado INTRO para PLAYING
	if (currentState == STATE_INTRO) {
		currentState = STATE_PLAYING;
		gameStartTime = ofGetElapsedTimef();
		levelStartTime = gameStartTime;
		lastSharkSpawnTime = gameStartTime;
		spawnInitialFish(); // Cria peixes iniciais
		clearVisualEffects(); // Limpa efeitos visuais
	}
}

void CSurvivalGameController::goBackToIdle()
{
	resetGame(); // Retorna ao estado inicial
}

void CSurvivalGameController::setProjectorRes(ofVec2f& PR)
{
	projRes = PR; // Armazena resolução do projetor
				  // Aloca FBO com a resolução do projetor
	fboGame.allocate(projRes.x, projRes.y, GL_RGBA);
	fboGame.begin();
	ofClear(0, 0, 0, 255); // Limpa com preto
	fboGame.end();

	// Define ROI do projetor (toda a tela)
	projROI = ofRectangle(0, 0, projRes.x, projRes.y);
}

void CSurvivalGameController::setKinectRes(ofVec2f& KR)
{
	kinectRes = KR; // Armazena resolução do Kinect
}

void CSurvivalGameController::setKinectROI(ofRectangle &KROI)
{
	kinectROI = KROI; // Armazena região de interesse do Kinect
}

void CSurvivalGameController::resetGame()
{
	// Reseta todos os estados e dados do jogo
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
	// Tenta encontrar posição válida para novo peixe
	if (setRandomFishLocation(location)) {
		auto f = Fish(kinectProjector, location, kinectROI, ofVec2f(0, 0));
		f.setup();
		fish.push_back(f); // Adiciona à lista de peixes
	}
}

bool CSurvivalGameController::setRandomFishLocation(ofVec2f &location)
{
	// Define uma ROI menor dentro da ROI do Kinect para spawn de peixes
	double W = kinectROI.getWidth() * 0.60;
	double H = kinectROI.getHeight() * 0.60;
	double X = kinectROI.getLeft() + 0.20 * W;
	double Y = kinectROI.getTop() + 0.20 * H;
	ofRectangle fishROI(X, Y, W, H);

	// Tenta encontrar posição dentro da água (elevação < 0)
	bool okwater = false;
	int count = 0;
	int maxCount = 100; // Limite de tentativas

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
	return okwater; // Retorna true se encontrou posição válida
}
