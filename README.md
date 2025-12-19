# Trabalho de Conclusão de Curso

### **Autores: Ricardo Alves Rodrigues**
### **Orientador: Talles Marcelo Gonçalves de Andrade Barbosa**
### **Universidade: Pontifícia Universidade Católica de Goiás**
### **Trabalho de Conclusão de Curso**
### **Projeto:DESENVOLVIMENTO DE JOGOS UTILIZANDO INTERFACE TANGÍVEL**

---

## **Descrição do Projeto**
Neste projeto é apresentado o desenvolvimento de jogos para o  sistema ARSandplay chamado de 3D Sandplay. Ele disponibiliza uma **interface tangível** que pode ser utilizada para **entreternimento** ou  como **reforçador** para o **desenvolvimento intelectual** e da **coordenação motora**. De forma exploratória, lança-se a hipótese reulizar o 3D Sandplay para a realização de desenvolvimento dos jogos. Este projeto foi inspirado e adaptado do [Augmented Reality Sandbox](https://arsandbox.ucdavis.edu/) desenvolvido pela [UC Davis](http://idav.ucdavis.edu/~okreylos/ResDev/SARndbox/). É uma adaptação parcial do projeto [SARndbox](https://github.com/KeckCAVES/SARndbox) de Oliver Kreylos em [openframeworks](https://openframeworks.cc/) e também é uma adaptação do [Projeto 3D SandPlay](https://github.com/josefoberdan/Projeto3DSandPlay) de  Josef  Oberdan, et al. Este projeto segue os termos da licença do repositório original [Magic-Sand](https://github.com/thomwolf/Magic-Sand).

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


## **Como Rodar o Projeto**
## 🧰 Passo 1 – Instalação dos Componentes Físicos

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

### 3. Adicionar OpenFrameworks (Opcional)
- [OpenFrameworks](https://openframeworks.cc/)

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

## 🟢 Passo 4 – Executar a Aplicação

1. Clique em **RUN**
2. Verifique:
   - **Mapa colorido com linhas de contorno** na areia
   - **FRAMERATE em torno de 30 -60 FPS**
3. Para dar inicio os jogos use comandos via teclado, com as telas 1 e 2:
  - Precionando a **tecla 1**, será iniciado o **Jogo da Alimentação**
  - Precionando a **tecla 2**, será iniciado o **Jogo da Sobrevicência**

---

## 📸 Resultado

div align="center"
img src="https://github.com/user-attachments/assets/607098f0-c5aa-4bce-b436-5c3c1e76d3fb" /
/div
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
