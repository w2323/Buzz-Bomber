# Buzz Bombers - GitHub README

![Buzz Bombers](https://your-image-link.com/banner.png) <!-- Optional Project Banner -->

## 🎮 Buzz Bombers - SFML 2D Game

**Author:** Wasif Mehmood Mughal
**Roll No:** i240699
**University:** FAST University

Buzz Bombers is a 2D arcade-style game built with C++ and SFML. Players control a spray gun at the bottom of the screen to shoot incoming bees. Defeated bees drop honeycombs that a bird collects to increase the player's score. The game features level progression, collisions, enemy logic, power-ups, and file-based high score tracking.

---

## 🚀 Features

* Player-controlled shooter
* Multiple types of bees with distinct movement
* Bullet shooting with sound effects
* Dropped honeycombs and bird auto-collection
* Dynamic obstacles (flowers)
* Level progression & increasing difficulty
* Score and high score tracking
* Clean user interface with lives, level, and score display

---

## 🖥️ Demo

Live gameplay preview (coming soon)

---

## 🧱 Built With

* **C++**
* **SFML (Simple and Fast Multimedia Library)**

  * Graphics
  * Audio
  * Events

---

## 🔧 Installation & Compilation

### Prerequisites

* [SFML](https://www.sfml-dev.org/download.php) installed
* C++ Compiler (g++, clang++, etc.)
* Windows/macOS/Linux with terminal or IDE support (like VS Code, Code::Blocks)

### Compilation Instructions

#### Windows (MinGW/MSYS2 + SFML)

```bash
g++ -IC:\SFML\include -LC:\SFML\lib buzz.cpp -o buzz.exe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

#### Linux

```bash
g++ buzz.cpp -o buzz -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

#### macOS (Homebrew-installed SFML)

```bash
g++ buzz.cpp -o buzz -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

> ⚠️ Make sure all SFML DLLs or shared libraries are accessible in your runtime path.

---

## 🕹️ Controls

* **Arrow Keys:** Move left and right
* **Spacebar:** Shoot bullets
* **Escape:** Exit game

---

## 📁 Project Structure

```
BuzzBombers/
├── Textures/             # All game assets (sprites, background, UI icons)
├── Sounds/               # Sound effects
├── buzz.cpp              # Main source code
├── highscore.txt         # Saved high score
├── hives.txt             # Hive map positions
├── README.md             # This file
```

---

## 📌 To Do / Future Improvements

* Add menu music and in-game background soundtrack
* Show proper Game Over screen with stats
* Add power-ups and difficulty modes
* Introduce boss levels or timed challenges
* Transition to `std::vector` and `sf::Clock` for optimized management

---

## 👨‍💻 Author

**Wasif Mehmood Mughal**
**Roll No:** i240699
**University:** FAST University

If you enjoyed this project, feel free to ⭐ the repo or fork it to extend the gameplay!

---

## 📜 License

This project is for educational and personal portfolio purposes. No commercial use allowed without permission.

---
