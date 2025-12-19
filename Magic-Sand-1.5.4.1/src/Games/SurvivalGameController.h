/***********************************************************************
SurvivalGameController.h - Controller for a Survival animal game
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

#ifndef _SurvivalGameController_h_
#define _SurvivalGameController_h_

#include "vehicle.h"
#include "../KinectProjector/KinectProjector.h"

class CSurvivalGameController
{
public:
    // Construtor e destrutor
    CSurvivalGameController();        // Inicializa o controlador do jogo
    virtual ~CSurvivalGameController(); // Destrutor virtual para limpeza

    // Métodos de ciclo de vida do jogo
    void setup(std::shared_ptr<KinectProjector> const& k); // Configura dependências
    void update();                                          // Atualiza lógica do jogo a cada frame
    void drawProjectorWindow();                             // Renderiza para janela do projetor
    void drawMainWindow(float x, float y, float width, float height); // Renderiza para janela principal

    // Controle de fluxo do jogo
    bool StartGame();       // Inicia o jogo (retorna true se bem-sucedido)
    bool isIdle();          // Verifica se jogo está no estado IDLE
    bool isInIntro();       // Verifica se jogo está no estado INTRO
    void startFromIntro();  // Transiciona de INTRO para PLAYING
    void goBackToIdle();    // Retorna ao estado IDLE (reinício)

    // Configuração de resolução e áreas
    void setProjectorRes(ofVec2f& PR);      // Define resolução do projetor
    void setKinectRes(ofVec2f& KR);         // Define resolução do Kinect
    void setKinectROI(ofRectangle &KROI);   // Define região de interesse do Kinect

private:
    // Dependências externas
    std::shared_ptr<KinectProjector> kinectProjector; // Ponteiro para sistema Kinect-Projector

    // Sistema de estados do jogo
    enum GameState {
        STATE_IDLE,            // Estado inicial/inativo
        STATE_INTRO,           // Tela de introdução/apresentação
        STATE_PLAYING,         // Jogo em andamento
        STATE_LEVEL_COMPLETE,  // Nível concluído com sucesso
        STATE_LEVEL_FAILED,    // Nível falhou (não usado atualmente)
        STATE_SHOWING_RESULTS  // Exibindo resultados (vitória/derrota)
    };

    // Variáveis de estado atual
    GameState currentState;           // Estado atual do jogo
    float gameStartTime;              // Timestamp de início do jogo
    float introStartTime;             // Timestamp de início da introdução
    float resultsStartTime;           // Timestamp de início da exibição de resultados
    float levelTransitionStartTime;   // Timestamp de início da transição de nível
    float introDisplayTime;           // Duração da tela de introdução
    float resultsDisplayTime;         // Duração da tela de resultados
    float levelTransitionDuration;    // Duração da transição entre níveis
    bool victory;                     // Flag de vitória (true = vitória, false = derrota)

    // SISTEMA DE NÍVEIS
    int currentLevel;      // Nível atual (1-based)
    int maxLevels;         // Número total de níveis disponíveis
    bool levelCompleted;   // Flag indicando se nível atual foi completado
    float levelStartTime;  // Timestamp de início do nível atual
    float levelDuration;   // Duração máxima do nível atual (em segundos)
    int maxSharks;         // Número máximo de tubarões permitidos no nível

    // Estrutura de configuração por nível
    struct LevelConfig {
        int levelNumber;       // Número do nível (1, 2, 3...)
        int initialFish;       // Quantidade inicial de peixes
        int maxSharks;         // Máximo de tubarões simultâneos
        float sharkSpawnRate;  // Intervalo entre spawns de tubarões (em segundos)
        float duration;        // Duração do nível (em segundos)
        std::string levelName; // Nome descritivo do nível
    };
    std::vector<LevelConfig> levelConfigs; // Vetor com configurações de todos os níveis

    // Variáveis de estado do jogo
    int initialFishCount;       // Número inicial de peixes (do nível atual)
    int fishSurvived;           // Contador de peixes sobreviventes
    int sharksSpawned;          // Contador de tubarões spawnados
    float lastSharkSpawnTime;   // Último timestamp de spawn de tubarão
    float sharkSpawnInterval;   // Intervalo atual entre spawns (do nível)

    // Áreas de spawn e projeção
    ofVec2f projRes;           // Resolução do projetor (largura, altura)
    ofVec2f kinectRes;         // Resolução do Kinect (largura, altura)
    ofRectangle kinectROI;     // Região de interesse no Kinect (área válida)
    ofRectangle projROI;       // Região de interesse no projetor (tela completa)

    // Animais do jogo
    std::vector<Fish> fish;                   // Vetor de peixes controláveis
    std::vector<Shark> sharks;                // Vetor de tubarões inimigos
    std::vector<DangerousBOID> dangerBOIDS;   // Vetor de perigos para comportamentos

    // Sistema de renderização
    ofFbo fboGame;          // Frame Buffer Object para renderização off-screen

    // Fontes para texto
    ofTrueTypeFont gameFont;   // Fonte para títulos principais
    ofTrueTypeFont scoreFont;  // Fonte para informações de jogo

    // Sistema de imagens
    ofImage splashScreen;     // Imagem da tela de introdução/splash

    // IMAGENS PARA EFEITOS VISUAIS
    ofImage confettiImage;     // Textura de confete para celebrações
    ofImage starsImage;        // Textura de estrelas para efeitos
    ofImage trophyImage;       // Textura do troféu (vitória final)
    ofImage bronzetrophyImage;  // Textura de troféu bronze (nível 1)
    ofImage silvertrophyImage;  // Textura de troféu prata (nível 2)
    ofImage goldtrophyImage;    // Textura de troféu ouro (nível 3)
    ofImage fireworksImage;    // Textura de fogos de artifício

    // Sistema de partículas para efeitos de confete
    struct ConfettiParticle {
        ofVec2f position;       // Posição atual da partícula
        ofVec2f velocity;       // Velocidade e direção
        ofColor color;          // Cor do confete
        float size;             // Tamanho em pixels
        float rotation;         // Rotação atual em graus
        float rotationSpeed;    // Velocidade de rotação
    };
    std::vector<ConfettiParticle> confettiParticles; // Lista de partículas ativas

    // Sistema de efeitos de estrelas piscantes
    struct StarEffect {
        ofVec2f position;       // Posição da estrela
        float size;             // Tamanho da estrela
        float alpha;            // Valor alpha atual (transparência)
        float pulseSpeed;       // Velocidade da animação de pulsação
    };
    std::vector<StarEffect> starEffects; // Lista de estrelas ativas

    // CONTROLES DE TAMANHO E POSIÇÃO
    float starMinSize;      // Tamanho mínimo para estrelas
    float starMaxSize;      // Tamanho máximo para estrelas
    float trophyYPosition;   // Posição Y para troféus
    float trophyYPosition;  // Posição Y para troféu

    // FLAGS PARA CONTROLE DE EFEITOS
    bool levelCompleteEffectsGenerated; // Efeitos de nível completo já foram gerados?
    bool victoryEffectsGenerated;       // Efeitos de vitória já foram gerados?

    // Métodos de lógica do jogo (implementados no .cpp)
    void setupLevels();                 // Inicializa configurações dos níveis
    void applyLevelConfig(int level);   // Aplica configuração de um nível específico
    void goToNextLevel();               // Avança para o próximo nível
    void spawnInitialFish();            // Spawna peixes iniciais do nível
    void spawnShark();                  // Spawna um novo tubarão
    void updateGameState();             // Atualiza estado dos animais e colisões
    void checkFishSurvival();           // Verifica colisões peixe-tubarão
    void drawGameInfo();                // Renderiza HUD com informações
    void drawIntroScreen();             // Renderiza tela de introdução
    void drawLevelCompleteScreen();     // Renderiza tela de nível completo
    void drawVictoryScreen();           // Renderiza tela de vitória final
    void drawDefeatScreen();            // Renderiza tela de derrota
    void resetGame();                   // Reseta jogo para estado inicial

    // Métodos de efeitos visuais
    void generateConfettiEffects();     // Gera partículas de confete
    void generateStarEffects();         // Gera efeitos de estrelas
    void generateVictoryEffects();      // Gera efeitos especiais de vitória
    void updateVisualEffects();         // Atualiza animações de efeitos visuais
    void clearVisualEffects();          // Limpa todos os efeitos visuais
    void debugPrintImageStatus();       // Debug: imprime status das imagens carregadas

    // Métodos auxiliares
    void addNewFish();                  // Adiciona um novo peixe à cena
    bool setRandomFishLocation(ofVec2f &location); // Encontra posição válida para spawn
};

#endif
