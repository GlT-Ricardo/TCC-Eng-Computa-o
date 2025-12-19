# Trabalho de Conclusão de Curso

### **Autores: Ricardo Alves Rodrigues**
### **Orientador: Talles Marcelo Gonçalves de Andrade Barbosa**
### **Universidade: Pontifícia Universidade Católica de Goiás**
### **Trabalho de Conclusão de Curso**
### **Projeto:DESENVOLVIMENTO DE JOGOS UTILIZANDO INTERFACE TANGÍVEL**
### **Disponível em:** https://repositorio.pucgoias.edu.br/jspui/handle/123456789/10244
---

## **Descrição do Projeto**
Este projeto apresenta o desenvolvimento de dois jogos, **Jogo de Sobrevivência e Jogo da Alimentação** para o sistema 3D SandPlay, uma plataforma de interface tangível que projeta elementos virtuais sobre uma caixa de areia. Podendo ser utilizada tanto para **entretenimento** quanto como reforçador do **desenvolvimento intelectual e da coordenação motora**. De forma exploratória, o trabalho investiga a reutilização do 3D SandPlay como base para o desenvolvimento de jogos. O projeto foi inspirado e adaptado do [Projeto 3D SandPlay](https://github.com/josefoberdan/Projeto3DSandPlay) (Josef Oberdan et al.) e do  [Augmented Reality Sandbox](https://arsandbox.ucdavis.edu/) desenvolvido pela [UC Davis](http://idav.ucdavis.edu/~okreylos/ResDev/SARndbox/), sendo uma adaptação parcial do [SARndbox](https://github.com/KeckCAVES/SARndbox) de Oliver Kreylos  em [openframeworks](https://openframeworks.cc/) seguindo os termos de licença do projeto original [Magic-Sand](https://github.com/thomwolf/Magic-Sand).
O desenvolvimento dos jogos foi realizado por meio da IDE Visual Studio 2015, utilizando a linguagem de programação C++.
---
## **Requisitos do Sistema**
- Antes de iniciar, garanta que você tenha:
  - PC com placa de vídeo GeForce GTX 1070 TI ou superior.
  - Windows 10 (64 bits).
  - Microsoft Visual Studio C++ 2015.
  - Xbox 360 Kinect 3D.
  - Projetor LCD.
  - Caixa com areia branca de aquário.

## **Estrutura do Projeto**
- A estrutura foi organizada da seguinte forma:
### **Hardware:**
 - Sensor Microsoft Kinect: versão 1.0 (Microsoft, 2010).
 - Projetor multimídia LED Projetor: modelo YG-300 (Salange Technology, 2014).
 - Dimensões da caixa de areia: 45 centímetros de comprimento, 33 centímetros de largura e 7 centímetros de altura.
 - Estrutura metálica galvanizada: 90 centímetros de altura.

### **Software:**
 - Microsoft Visual Studio C++ 2015 (© Microsoft Corporation).
 - Zadig.

---

--
## **Como Rodar o Projeto**
## 🛠️ Passo 1 – Instalação dos Componentes Físicos

### 1. Conectar o Hardware
- Conecte o **Kinect** e o **Projetor** ao computador.

### 2. Instalar Drivers com Zadig
- Baixe o Zadig: [https://zadig.akeo.ie/](https://zadig.akeo.ie/)
- Execute o Zadig e:
  - Selecione no menu:
    - `Xbox Camera`
    - `Xbox Audio`
    - `Xbox Motor`
  - Escolha o driver `libusb-win32 (v1.2.6.0)`
  - Clique em **Install Driver**
  - **Repita 3 vezes**, uma para cada componente

### 3. Referência Adicional
- [Freenect GitHub](https://github.com/OpenKinect/libfreenect)
- [ofxKinect GitHub](https://github.com/ofTheo/ofxKinect)

### 4. Problemas de Detecção
- Instale o **Kinect V1 SDK 1.0** e execute novamente o Zadig, se necessário

---

## 💻 Passo 2 – Edição do Código Fonte

### 1. Instalar o Visual Studio
- [Visual Studio C++ 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145)

### 2. Baixar o Software
- [Magic-Sand (GitHub)](https://github.com/thomwolf/Magic-Sand/releases/tag/v1.5.4.1)

### 3. Adicionar OpenFrameworks
- [OpenFrameworks](https://openframeworks.cc/)

### **Projeto completo**
Os links abaixo disponibilizam o projeto completo para download, já contendo o openFrameworks, Magic-Sand e a implementação dos jogos.

https://pucdegoias-my.sharepoint.com/:u:/g/personal/20201003300528_pucgo_edu_br/IQCyIwgUQk8ZQpDDkVgaDqTeAQkxBXFY0gmbFaWr0d4GX1o?e=QtHvHZ

https://drive.google.com/file/d/1sRuz57vkgQPSCupiT6Fhi6AxWgonJP_m/view?usp=drive_link

Ao realizar o download, abra o projeto no ambiente de desenvolvimento seguindo esses passos:

  1 - Abra o **Visual Studio** e clique em **Arquivo (File)**, em seguida clique em **Abrir (Open)** e clique em **Projeto/Solução(Project/Solution)**.
  
  2 - Busque o diretório onde se encontra o projeto baixado, por exemplo:
  
  C:\Users\User\Desktop\3D_SANDPLAY COMPLETO\project\of_v0.9.8_vs_release\apps\myApps\Magic-Sand-1.5.4.1
  
  3 - Dentro da pasta **Magic-Sand-1.5.4.1** selecione o arquivo **Magic-Sand.sln** para abrir projeto.
  
  4 - Após o carregamento do projeto, abra os arquivos de edição clicando em **Arquivo (File)**, em sequida clique em **Abrir (Open)** e clique em **Arquivo... (File...)**.
  
  Busque novamente o diretorio do projeto, mas desta vez, acesse a pasta **src**, dentro dela se encontra os arquivos para edição.
  
  C:\Users\User\Desktop\3D_SANDPLAY COMPLETO\project\of_v0.9.8_vs_release\apps\myApps\Magic-Sand-1.5.4.1\src
  
  5 - Na pasta **src**, os principais arquivos para edição são: **ofApp.h e ofApp..cpp**.
  
  Nas subpastas do diretório, se encontra as pastas **Games**, **KinectProjector** e **SandSurfaceRenderer**
  
  Para modificações e implementações, foi utilizado os arquivos que se encontram na pasta **Games**, especificamente, implementando os novos arquivos dos jogos:
  
  **FeedingGameController.ccp e .h** 
  
  **SurvivalGameController.cpp e .h**

  Para modificações dos **peixes** e **tubarões** é preciso acessar dos arquivos **vehicle.cpp e .h**
  
---

## 🎯 Passo 3 – Calibrar a Aplicação

1. **Achate a areia** na caixa de areia
2. Ative a **visualização de profundidade** em:  
   `Avançado > Exibir Exibição de Profundidade do Kinect`
3. Clique em **Calibração**
4. **Desenhe a região da areia** com o mouse na visualização
5. Pressione **“Calibrar Kinect e Projetor automaticamente”**  
   - Um padrão de **xadrez** será projetado
6. **Cubra a caixa** com papelão claro quando solicitado
7. Clique em **OK** para projetar novos padrões sobre o papelão
8. Confirme que a **janela de status** indica calibração OK

---

## ⚙️ Passo 4 – Executar a Aplicação

1. Clique em **RUN**
2. Verifique:
   - **Mapa colorido com linhas de contorno** na areia
   - **FRAMERATE em torno de 30 -60 FPS**
3. Para dar inicio os jogos use comandos via teclado, com as telas 1 e 2:
  - Precionando a **tecla 1**, será iniciado o **Jogo da Alimentação**
       Neste jogo, deve-se criar canais na areia para guiar os peixes até os alimentos virtuais. Utilize o relevo da areia para formar caminhos que direcionem os peixes, coletando os alimentos dentro do tempo limite de           cada nível.
  - Precionando a **tecla 2**, será iniciado o **Jogo da Sobrevicência**
      Neste jogo, deve-se proteger os peixes virtuais dos tubarões predadores criando barreiras físicas na areia. Modele elevações e montanhas que impeçam a passagem dos tubarões, mantendo os peixes em segurança dentro do       tempo limite de cada nível .

---

## 🎮 Demonstração dos Jogos

---

### ▶️ Vídeos Demonstrativos

#### Jogo da Alimentação

<p align="left">
  <img src="https://github.com/user-attachments/assets/686e338d-3ead-4077-bd04-73201819d6c0" width="250"/>
</p>

#### Jogo da Sobrevivência

<p align="left">
  <img src="https://github.com/user-attachments/assets/d3b90953-c52a-4907-b5dd-e6291f3a9a9e" width="250"/>
</p>

---

### 🐟 Resultados — Jogo da Alimentação

| Fase 1                                                                                                   | Fase 2                                                                                                   | Fase 3                                                                                                   |
| -------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| <img src="https://github.com/user-attachments/assets/d0f2b954-f903-4c39-be66-6fd1091061a9" width="250"/> | <img src="https://github.com/user-attachments/assets/e2e716b5-fec8-4f46-8eb7-f4d84891fa5e" width="250"/> | <img src="https://github.com/user-attachments/assets/b3e10277-601e-4a28-ba8c-9fb0c27cfb5a" width="250"/> |

---

### 🦈 Resultados — Jogo da Sobrevivência

| Fase 1                                                                                                   | Fase 2                                                                                                   | Fase 3                                                                                                   |
| -------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| <img src="https://github.com/user-attachments/assets/2ef041c8-8991-45f1-86ab-071127c86504" width="250"/> | <img src="https://github.com/user-attachments/assets/70fdf837-f9c2-46a9-80c5-f0aeb63b9121" width="250"/> | <img src="https://github.com/user-attachments/assets/a68ff059-4d25-4f20-b8b4-5f61e9e4a63c" width="250"/> |

---

### ❌ Tela de Game Over

<p align="letf">
  <img src="https://github.com/user-attachments/assets/f86380e9-2970-49f3-ac53-7f81560a5453" width="350"/>
</p>



---

## 🔗 Licenças

Este projeto segue os termos da licença do repositório original [Magic-Sand](https://github.com/thomwolf/Magic-Sand).

### **Linguagem de Programação**
- **C++**: Linguagem de programação principal usada no desenvolvimento do projeto. 
  - **Licença**: GCC/Clang ou MSVC
  - **Copyright**: © Desenvolvedores do compilador (Free Software Foundation para GCC, Microsoft para MSVC)

### **Tecnologias e Bibliotecas Utilizadas**
- **openFrameworks**: Framework C++ para aplicações, usado para lidar com gráficos, áudio, vídeo e entrada de dispositivos.  
  **Licença**: MIT License  
  **Copyright**: © openFrameworks contributors

- **ofxKinect**: Addon do openFrameworks para comunicação com sensores Kinect.  
  **Licença**: MIT License  
  **Copyright**: © Theo Watson e contribuidores do addon

- **libfreenect**: Biblioteca usada para acessar os dados do Kinect (driver alternativo).  
  **Licença**: Apache License 2.0  
  **Copyright**: © OpenKinect community
  
  ---

### **Ferramentas**
- **Visual Studio Code 2015**: IDE utilizada para compilar e rodar o projeto no Windows.
  - **Licença**: Licença proprietária gratuita para uso pessoal e educacional (Community Edition)
  - **Copyright**: © Microsoft Corporation
  
- **Zadig**: Ferramenta usada para instalar drivers USB, necessária para fazer o Kinect funcionar.
  - **Licença**: GPL v3
  - **Copyright**: © Akeo Consulting

---




### **Github Magic Sand:** https://github.com/thomwolf/Magic-Sand/blob/master/README.md
### **Github 3D Sandplay:** https://github.com/josefoberdan/Projeto3DSandPlay
