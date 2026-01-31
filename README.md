# Luca Clicker

Luca Clicker is an idle-incremental simulation game inspired by AdVenture Capitalist based on the Luca pastry shop. 
You start with nothing but ambition and a pretzel.
Sell items, earn money, and build your empire one click at a time.
Each sale earns you income, which can be reinvested into upgrades and automation systems to scale your profits.
Players begin with a single unlocked food item.

## How to Play

The game is played using the mouse.

*   **Main Menu:** When you start the game, you can choose to start a **New Game** or load a **Saved Game**.
*   **Selling Items:**
    *   On the left side of the screen, you'll see a list of pastries you can sell.
    *   Click the **UNLOCK** button to make an item available for selling.
    *   Once unlocked, the button changes to **SELL**. Click it to start selling the item.
    * A progress bar will show you the state of the sell.
*   **Upgrading:**
    *   Next to the sell button, you'll find the **UPGRADE** button.
    *   Click it to level up your item, which makes it sell for more money.
*   **Automating Deliveries:**
    *   To the right of the "UPGRADE" button, you will find a "DELIVERY" button.
    *   Click this button to pay a one-time fee and purchase an automated delivery service for that item.
    *   Once purchased, the item will be sold automatically, earning you income without needing to click the "SELL" button.
*   **Using Beverages:**
    *   On the right side of the screen, you can find beverages.
    *   Just like pastries, you first need to **UNLOCK** them.
    *   Once unlocked, you can **USE** them to get a temporary income boost.
*   **Earning Money:** Your total money is displayed at the top of the screen.
*   **Quitting:** Simply close the game window. Your progress will be saved automatically.

## To Implement
  
- All the features of the game using the clicker UI.
- Idle progress while the game is off.

## Tema 3

#### Cerințe
- [x] 2 șabloane de proiectare (design patterns)
- [x] o clasă șablon cu sens; minim **2 instanțieri**
  - [x] preferabil și o funcție șablon (template) cu sens; minim 2 instanțieri
- [ ] minim 85% din codul propriu să fie C++
<!-- - [ ] o specializare pe funcție/clasă șablon -->
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.3` sau `v1.0`
- [ ] code review #3 2 proiecte

## Assets

All the assets used in this project are stored in the `assets` directory. They are categorized into `audio`, `font`, and `textures`.

### Pastries


| Item | Filename        | Source |
|------|-----------------|--------|
| **Pretzel** | `Pretzel.png`   | [Flaticon](https://www.flaticon.com/free-icon/pretzel_11615204) |
| **Pizza** | `Pizza.png`     | [Flaticon](https://www.flaticon.com/free-icon/pizza_6978255) |
| **Apple Pie** | `ApplePie.png`  | [Flaticon](https://www.flaticon.com/free-icon/pie-cake_17673040) |
| **Strudel** | `Strudel.png`   | [Flaticon](https://www.flaticon.com/free-icon/meat_15620753) |
| **CovriLuca** | `CovriLuca.png` | [Flaticon](https://www.flaticon.com/free-icon/eating_15113960) |

---
### Beverages

| Item | Filename     | Source |
|------|--------------|--------|
| **Water** | `Water.png`  | [Flaticon](https://www.flaticon.com/free-icon/water_824239) |
| **Coke** | `Coke.png`   | [Flaticon](https://www.flaticon.com/free-icon/soda-bottle_8933016) |
| **Coffee** | `Coffee.png` | [Flaticon](https://www.flaticon.com/free-icon/coffee-cup_1047462) |
| **Ayran** | `Ayran.png`  | [Flaticon](https://www.flaticon.com/free-icon/ayran_6411124) |
| **Matcha** | `Matcha.png` | [Flaticon](https://www.flaticon.com/free-icon/matcha-latte_11602032) |
---

### UI

Interface elements were extracted from *AdVenture Capitalist* using [AssetStudio](https://github.com/Perfare/AssetStudio).

---

### Font

| Font | File | Source |
|------|------|--------|
| **Mighty Souly** | `MightySouly-lxggD.ttf` | [Download](https://font.download/font/mighty-souly) |

---

### Audio

| Audio                          | File | Source |
|--------------------------------|------|--------|
| **Adventure Capitalist** | `adventure_capitalist_theme_song.wav` | [YouTube](https://www.youtube.com/watch?v=VWYzCHB6kpg) (converted to .wav) |

---

## Technologies Used

*   **C++23:** The core programming language used for the project.
*   **SFML 3:** Simple and Fast Multimedia Library for handling windowing, graphics, audio, and input.
*   **CMake:** For managing the build process and dependencies.