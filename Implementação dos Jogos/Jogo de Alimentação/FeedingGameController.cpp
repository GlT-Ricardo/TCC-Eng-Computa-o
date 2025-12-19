/***********************************************************************
FeedingGameController.cpp - Controller for a Feeding animal game
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

#include "FeedingGameController.h"

CFeedingGameController::CFeedingGameController()
{
    // Inicializa o estado atual do jogo como ocioso
    currentState = STATE_IDLE;

    // SISTEMA DE NÍVEIS
    // Define o nível atual, máximo de níveis, status de conclusão e comida alvo
    currentLevel = 1;
    maxLevels = 3;
    levelCompleted = false;
    targetFood = 5; // Quantidade de comida necessária para completar um nível

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
    medalYPosition = 100.0f; // Posição Y da medalha na tela
    trophyYPosition = 120.0f; // Posição Y do troféu na tela

    // CARREGAR IMAGEM DO SPLASH SCREEN (tela de apresentação)
    std::string splashScreenFname = "boidgame/art/FeedingGameSplashScreen.png";
    if (!splashScreen.load(splashScreenFname))
    {
        // Registra erro se não conseguir carregar a imagem
        ofLogError("CFeedingGameController") << "Could not load splash screen: " << splashScreenFname;
    }

    // INICIALIZAR FLAGS DE EFEITOS
    levelCompleteEffectsGenerated = false; // Indica se os efeitos de nível completo já foram gerados
    victoryEffectsGenerated = false;       // Indica se os efeitos de vitória já foram gerados

    // CARREGAR IMAGENS DE EFEITOS VISUAIS
    std::string confettiPath = "boidgame/art/confetti.png";
    std::string starsPath = "boidgame/art/stars.png";
    std::string trophyPath = "boidgame/art/trophy.png";
    std::string bronzeMedalPath = "boidgame/art/bronze_trophy.png";
    std::string silverMedalPath = "boidgame/art/silver_trophy.png";
    std::string goldMedalPath = "boidgame/art/gold_trophy.png";
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

    // Se não conseguir carregar as medalhas, cria fallbacks básicos
    if (!bronzeMedalImage.load(bronzeMedalPath)) {
        ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar bronze_medal.png";
        // Fallback: criar medalha bronze básica
        bronzeMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
        bronzeMedalImage.getPixels().setColor(ofColor(205, 127, 50)); // Cor bronze
        bronzeMedalImage.update();
    }

    if (!silverMedalImage.load(silverMedalPath)) {
        ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar silver_medal.png";
        // Fallback: criar medalha prata básica
        silverMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
        silverMedalImage.getPixels().setColor(ofColor(192, 192, 192)); // Cor prata
        silverMedalImage.update();
    }

    if (!goldMedalImage.load(goldMedalPath)) {
        ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar gold_medal.png";
        // Fallback: criar medalha ouro básica
        goldMedalImage.allocate(100, 100, OF_IMAGE_COLOR_ALPHA);
        goldMedalImage.getPixels().setColor(ofColor(255, 215, 0)); // Cor ouro
        goldMedalImage.update();
    }

    if (!fireworksImage.load(fireworksPath)) {
        ofLogError("CFeedingGameController") << "ERRO: Não foi possível carregar fireworks.png";
        allImagesLoaded = false;
    }

    // Avisa se algumas imagens não foram carregadas
    if (!allImagesLoaded) {
        ofLogWarning("CFeedingGameController") << "Algumas imagens de efeitos não foram carregadas. Usando fallbacks.";
    }

    // DEBUG: mostra status de carregamento de todas as imagens
    debugPrintImageStatus();

    // INICIALIZAR SISTEMA DE PARTÍCULAS
    confettiParticles.clear(); // Limpa partículas de confete
    starEffects.clear();       // Limpa efeitos de estrela
}

CFeedingGameController::~CFeedingGameController()
{
    // Destrutor - limpeza de recursos se necessário
}

void CFeedingGameController::debugPrintImageStatus()
{
    // Imprime no console o status de carregamento de cada imagem
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
    levelConfigs.clear(); // Limpa configurações anteriores

    // Formato: {nível, peixes iniciais, comida alvo, intervalo de spawn de comida, duração, nome}
    // NÍVEL 1 - FÁCIL
    levelConfigs.push_back({ 1, 2, 15, 1.0f, 10.0f, "Comeco da Aventura" });

    // NÍVEL 2 - MÉDIO
    levelConfigs.push_back({ 2, 15, 1, 1.5f, 20.0f, "Fome Aquatica" });

    // NÍVEL 3 - DIFÍCIL
    levelConfigs.push_back({ 3, 15, 1, 1.0f, 20.0f, "Hora do Banquete" });
}

void CFeedingGameController::applyLevelConfig(int level)
{
    // Verifica se o nível está dentro dos limites
    if (level < 1 || level > levelConfigs.size()) return;

    // Obtém configuração do nível especificado
    LevelConfig config = levelConfigs[level - 1];

    // Aplica configurações nas variáveis do jogo
    initialFishCount = config.initialFish;
    targetFood = config.targetFood;
    foodSpawnInterval = config.foodSpawnRate;
    levelDuration = config.duration;
    maxFoodItems = 8; // Número máximo de itens de comida na tela simultaneamente
}

void CFeedingGameController::goToNextLevel()
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

void CFeedingGameController::setup(std::shared_ptr<KinectProjector> const& k)
{
    kinectProjector = k; // Armazena referência ao KinectProjector

    // Configura fontes para texto
    gameFont.load("verdana.ttf", 45);   // Fonte para títulos
    scoreFont.load("verdana.ttf", 45);  // Fonte para pontuação

    resetGame(); // Inicializa o jogo
}

void CFeedingGameController::update()
{
    // Estado ocioso - apenas limpa o FBO
    if (currentState == STATE_IDLE) {
        fboGame.begin();
        ofClear(0, 0, 0, 0); // Limpa com transparência total
        ofSetColor(255, 255, 255, 150);
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

        // VERIFICA SE NÍVEL FOI COMPLETADO (por comida coletada)
        if (!levelCompleted && foodCollected >= targetFood) {
            levelCompleted = true;
            goToNextLevel(); // Avança para próximo nível
            return;
        }

        // VERIFICA SE TEMPO ACABOU (GAME OVER)
        if (!levelCompleted && levelElapsedTime >= levelDuration) {
            // Vai direto para tela de resultados (derrota)
            currentState = STATE_SHOWING_RESULTS;
            resultsStartTime = currentTime;
            victory = false;
            clearVisualEffects(); // Remove efeitos visuais
            return;
        }

        // SPAWN DE COMIDA (só se nível não está completo e não atingiu máximo de itens)
        if (!levelCompleted && currentTime - lastFoodSpawnTime > foodSpawnInterval && foodItems.size() < maxFoodItems) {
            spawnFood(); // Cria nova comida
            lastFoodSpawnTime = currentTime;
        }

        // Remove comida antiga (que existe há mais de 10 segundos)
        for (int i = foodItems.size() - 1; i >= 0; i--) {
            if (currentTime - foodItems[i].spawnTime > 10.0f) {
                foodItems.erase(foodItems.begin() + i);
            }
        }

        // Update do estado do jogo
        updateGameState();

        // Renderiza cena do jogo no FBO
        fboGame.begin();
        ofClear(0, 0, 0, 0); // Limpa com transparência
        
        // Desenha informações do jogo (HUD) e comida
        drawGameInfo();
        drawFoodItems();

        // Desenha animais na cena
        for (auto &f : fish) {
            f.draw(); // Desenha cada peixe
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
            
            // RESETA ESTADO PARA NOVO NÍVEL
            spawnInitialFish(); // Cria novos peixes
            foodItems.clear();  // Remove toda comida
            foodCollected = 0;  // Reseta contador
            lastFoodSpawnTime = ofGetElapsedTimef();

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

void CFeedingGameController::updateGameState()
{
    // Só atualiza estado se a imagem do Kinect estiver estável
    if (kinectProjector->isImageStabilized()) {
        // Update fish - aplica comportamentos e atualiza posição
        for (auto &f : fish) {
            // Comportamentos sem perigos (segundo jogo não tem tubarões)
            f.applyBehaviours(false, fish, std::vector<DangerousBOID>());
            f.update();
        }
        // Verifica colisões entre peixes e comida
        checkFoodCollection();
    }
}

void CFeedingGameController::checkFoodCollection()
{
    // Percorre a lista de comida de trás para frente (para evitar problemas ao remover)
    for (int i = foodItems.size() - 1; i >= 0; i--) {

        // Se o item de comida não está ativo, ignora
        if (!foodItems[i].active) continue;

        // Verifica todos os peixes no cenário
        for (auto &f : fish) {

            // Calcula a distância entre o peixe e o item de comida
            float distance = (f.getLocation() - foodItems[i].location).length();

            // Critério de coleta: se o peixe estiver próximo o bastante da comida
            if (distance < f.getSize() + 10) { // Margem de 10 pixels

                // Desativa o item de comida (foi comido)
                foodItems[i].active = false;

                // Atualiza contadores
                foodCollected++;    // Comida coletada nesta fase
                totalFood++;        // Comida total no jogo

                // Interrompe o loop de peixes, pois o item já foi comido
                break;
            }
        }
    }
}

void CFeedingGameController::spawnInitialFish()
{
    fish.clear(); // Remove peixes existentes
    // Cria quantidade inicial de peixes
    for (int i = 0; i < initialFishCount; i++) {
        addNewFish(); // Adiciona novo peixe
    }
}

void CFeedingGameController::spawnFood()
{
    ofVec2f location;
    // Tenta encontrar posição válida para spawn de comida
    if (setRandomFoodLocation(location)) {
        FoodItem food;
        food.location = location;
        food.active = true;
        food.spawnTime = ofGetElapsedTimef(); // Marca tempo de criação
        foodItems.push_back(food); // Adiciona à lista
    }
}

void CFeedingGameController::generateConfettiEffects()
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

        // Cores em tons de amarelo/laranja para combinar com tema de comida
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

        confettiParticles.push_back(p); // Adiciona à lista
    }
}

void CFeedingGameController::generateStarEffects()
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

void CFeedingGameController::generateVictoryEffects()
{
    // Gerar confete básico
    generateConfettiEffects();

    // Adicionar mais confete especial para vitória
    for (int i = 0; i < 100; i++) {
        ConfettiParticle p;
        p.position.set(ofRandom(0, projROI.width),
            ofRandom(0, projROI.height));
        p.velocity.set(ofRandom(-3, 3), ofRandom(-3, 3));
        p.color = ofColor(255, 200, 50); // Dourado para vitória (tema comida)
        p.size = ofRandom(15, 30); // Maior que confete normal
        p.rotation = ofRandom(0, 360);
        p.rotationSpeed = ofRandom(-8, 8); // Rotação mais rápida
        confettiParticles.push_back(p);
    }

    // Gerar estrelas
    generateStarEffects();
    victoryEffectsGenerated = true; // Marca que efeitos foram gerados
}

void CFeedingGameController::updateVisualEffects()
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

void CFeedingGameController::clearVisualEffects()
{
    // Limpa todas as listas de efeitos visuais
    confettiParticles.clear();
    starEffects.clear();
    // Reseta flags de geração
    levelCompleteEffectsGenerated = false;
    victoryEffectsGenerated = false;
}

void CFeedingGameController::drawFoodItems()
{
    // Desenha cada item de comida ativo na tela
    for (auto &food : foodItems) {
        if (!food.active) continue; // Ignora comida já coletada

        // Converte coordenadas Kinect para coordenadas do projetor
        ofVec2f projPos = kinectProjector->kinectCoordToProjCoord(food.location.x, food.location.y);

        ofPushMatrix();
        ofTranslate(projPos); // Move para posição da comida

        // Desenha comida como pequenos círculos amarelos
        float foodSize = 12.0f;
        ofSetColor(255, 255, 0); // Amarelo
        ofFill();
        ofDrawCircle(0, 0, foodSize);

        // Efeito de pulsação para destacar a comida
        float pulse = sin(ofGetElapsedTimef() * 5) * 2 + foodSize;
        ofSetColor(255, 165, 0, 100); // Laranja semi-transparente
        ofDrawCircle(0, 0, pulse);

        ofPopMatrix();
    }
}

void CFeedingGameController::drawGameInfo()
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
    std::string foodStr = "Comida: " + ofToString(foodCollected) + " / " + ofToString(targetFood);

    // ===========================================
    //   LINHA 1: CENTRALIZAR FASE
    // ===========================================
    float yLine1 = 80; // Altura da primeira linha
    float wLevel = gameFont.stringWidth(levelStr);
    float xLevel = (ofGetWidth() - wLevel) / 2; // Centraliza horizontalmente

    gameFont.drawString(levelStr, xLevel, yLine1);

    // ===========================================
    //   LINHA 2: TEMPO E COMIDA LADO A LADO
    // ===========================================
    float yLine2 = 140; // Altura da segunda linha (abaixo da primeira)

    // Calcula larguras dos textos
    float wTime = scoreFont.stringWidth(timeStr);
    float wFood = scoreFont.stringWidth(foodStr);
    float spacing = 100; // Espaçamento entre tempo e comida

    // Calcula posicionamento para centralizar grupo
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
    ofClear(0, 0, 0, 200); // Fundo preto semi-transparente

    // Desenha splash screen se carregada
    if (splashScreen.isAllocated()) {
        splashScreen.draw(0, 0, projROI.width, projROI.height);
    }
    else {
        // Fallback com texto caso imagem não carregue
        
        // Título centralizado
        ofSetColor(0, 0, 0); // Preto
        string title = "JOGO DE ALIMENTACAO";
        float titleW = gameFont.stringWidth(title);
        float titleX = (projROI.width - titleW) / 2;
        gameFont.drawString(title, titleX, 100);

        // Informações sobre o jogo
        string line1 = "Complete " + ofToString(maxLevels) + " fases!";
        float line1W = scoreFont.stringWidth(line1);
        float line1X = (projROI.width - line1W) / 2;
        scoreFont.drawString(line1, line1X, 180);

        string line2 = "Leve os peixes ate a comida.";
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

void CFeedingGameController::drawLevelCompleteScreen()
{
    fboGame.begin();
    ofClear(50, 50, 0, 200); // Fundo amarelo-esverdeado semi-transparente

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

    // CAMADA 2: MEDALHA DA FASE COMPLETADA
    ofPushStyle();
    ofImage* currentMedal = nullptr;
    string medalText = "";

    // Escolhe medalha baseada na fase completada
    if (currentLevel - 1 == 1) { // Fase 1 completada
        currentMedal = &bronzeMedalImage;
    }
    else if (currentLevel - 1 == 2) { // Fase 2 completada
        currentMedal = &silverMedalImage;
    }
    else if (currentLevel - 1 == 3) { // Fase 3 completada
        currentMedal = &goldMedalImage;
    }

    // Desenha medalha se disponível
    if (currentMedal && currentMedal->isAllocated()) {
        float medalSize = 120;
        float medalX = (projROI.width - medalSize) / 2; // Centraliza
        float medalY = medalYPosition;

        // Fundo semi-transparente atrás da medalha
        ofSetColor(0, 0, 0, 80);
        ofDrawCircle(medalX + medalSize / 2, medalY + medalSize / 2, medalSize * 0.7);

        // Medalha
        ofSetColor(255, 255, 255);
        currentMedal->draw(medalX, medalY, medalSize, medalSize);

        // Texto da medalha (opcional)
        if (!medalText.empty()) {
            ofSetColor(255, 255, 200);
            scoreFont.drawString(medalText, 
                (projROI.width - scoreFont.stringWidth(medalText)) / 2, 
                medalY + medalSize + 30);
        }
    }
    ofPopStyle();

    // CAMADA 3: TEXTO SEM FUNDO
    ofPushStyle();

    // Posição Y do texto (ajustada para medalha)
    float textBoxY = (currentMedal && currentMedal->isAllocated()) ? 
                     medalYPosition + 210 : 230;

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

    // Título principal com cor vibrante (amarelo para combinar com tema de comida)
    ofSetColor(255, 255, 0);
    gameFont.drawString(title, titleX, textBoxY);

    // Linha divisória decorativa sutil
    ofSetColor(255, 200, 0, 80);
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
    string foodTarget = "- " + ofToString(nextConfig.targetFood) + " Alimentos para coletar";
    float foodTargetW = scoreFont.stringWidth(foodTarget);
    float foodTargetX = (projROI.width - foodTargetW) / 2;

    // Sombra
    ofSetColor(0, 0, 0, 220);
    scoreFont.drawString(foodTarget, foodTargetX + 2, textBoxY + 201);

    // Texto principal
    ofSetColor(255, 255, 200);
    scoreFont.drawString(foodTarget, foodTargetX, textBoxY + 200);

    string fishCount = "- " + ofToString(nextConfig.initialFish) + " peixes famintos";
    float fishCountW = scoreFont.stringWidth(fishCount);
    float fishCountX = (projROI.width - fishCountW) / 2;

    // Sombra
    ofSetColor(0, 0, 0, 220);
    scoreFont.drawString(fishCount, fishCountX + 2, textBoxY + 251);

    // Texto principal
    ofSetColor(200, 200, 255);
    scoreFont.drawString(fishCount, fishCountX, textBoxY + 250);

    ofPopStyle();

    // CAMADA 4: CONTADOR NO RODAPÉ
    ofPushStyle();

    // Fundo sutil para o contador
    ofSetColor(0, 0, 0, 80);
    ofDrawRectangle(0, projROI.height - 70, projROI.width, 70);

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

void CFeedingGameController::drawVictoryScreen()
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
    gameFont.drawString(title, titleX + 3, textBoxY + 3);

    // Título principal com efeito de pulso amarelo (tema comida)
    ofSetColor(255, 255 * pulse, 0);
    gameFont.drawString(title, titleX, textBoxY);

    // Linha decorativa
    ofSetColor(255, 200, 0, 100);
    ofDrawLine(projROI.width * 0.2, textBoxY + 45,
               projROI.width * 0.8, textBoxY + 45);

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

void CFeedingGameController::drawTryAgainScreen()
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
    string levelFailed = "Fase " + ofToString(currentLevel) + " Fracassada!";
    float levelFailedW = scoreFont.stringWidth(levelFailed);
    float levelFailedX = (projROI.width - levelFailedW) / 2;

    ofSetColor(255, 255, 255); // Branco
    scoreFont.drawString(levelFailed, levelFailedX, 250);

    // Mensagem motivacional
    string motivation = "Vamos Iniciar Novamente!";
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

void CFeedingGameController::drawProjectorWindow()
{
    // Desenha o FBO na janela do projetor de acordo com o estado atual
    if (currentState == STATE_INTRO) {
        drawIntroScreen(); // Renderiza tela de introdução
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
            // Derrota: já desenhada no FBO por drawTryAgainScreen
        }
        fboGame.draw(0, 0, projROI.width, projROI.height);
    }
    else {
        // Estado PLAYING ou outros: desenha FBO normalmente
        fboGame.draw(0, 0);
    }
}

void CFeedingGameController::drawMainWindow(float x, float y, float width, float height)
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
            drawTryAgainScreen(); // Renderiza tela de derrota
        }
        fboGame.draw(x, y, width, height);
    }
    else {
        // Estado PLAYING: desenha FBO do jogo
        fboGame.draw(x, y, width, height);
    }
}

bool CFeedingGameController::StartGame()
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
    // Transição do estado INTRO para PLAYING
    if (currentState == STATE_INTRO) {
        currentState = STATE_PLAYING;
        gameStartTime = ofGetElapsedTimef();
        levelStartTime = gameStartTime;
        lastFoodSpawnTime = gameStartTime;
        spawnInitialFish(); // Cria peixes iniciais
        clearVisualEffects(); // Limpa efeitos visuais
    }
}

void CFeedingGameController::goBackToIdle()
{
    resetGame(); // Retorna ao estado inicial
}

void CFeedingGameController::setProjectorRes(ofVec2f& PR)
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

void CFeedingGameController::setKinectRes(ofVec2f& KR)
{
    kinectRes = KR; // Armazena resolução do Kinect
}

void CFeedingGameController::setKinectROI(ofRectangle &KROI)
{
    kinectROI = KROI; // Armazena região de interesse do Kinect
}

void CFeedingGameController::resetGame()
{
    // Reseta todos os estados e dados do jogo
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
    // Tenta encontrar posição válida para novo peixe
    if (setRandomFishLocation(location)) {
        auto f = Fish(kinectProjector, location, kinectROI, ofVec2f(0, 0));
        f.setup();
        fish.push_back(f); // Adiciona à lista de peixes
    }
}

bool CFeedingGameController::setRandomFishLocation(ofVec2f &location)
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

bool CFeedingGameController::setRandomFoodLocation(ofVec2f &location)
{
    // Define uma ROI maior para spawn de comida (80% da área do Kinect)
    double W = kinectROI.getWidth() * 0.80;
    double H = kinectROI.getHeight() * 0.80;
    double X = kinectROI.getLeft() + 0.10 * W;
    double Y = kinectROI.getTop() + 0.10 * H;
    ofRectangle foodROI(X, Y, W, H);

    // Tenta encontrar posição dentro da água (elevação < 0)
    bool okwater = false;
    int count = 0;
    int maxCount = 100; // Limite de tentativas
    
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
    return okwater; // Retorna true se encontrou posição válida
}
