# Splitting Comets Game

A browser-based game built using HTML5 Canvas and JavaScript. The game features a spaceship that dodges and shoots comets, a dynamic starfield background, and various explosion and collision effects.

## Table of Contents

- [Overview](#overview)
- [Function Documentation](#function-documentation)
  - [Starfield Functions](#starfield-functions)
  - [Texture & Pattern Functions](#texture--pattern-functions)
  - [Planet Drawing Function](#planet-drawing-function)
  - [UI Functions](#ui-functions)
  - [Explosion Functions](#explosion-functions)
  - [Comet Functions](#comet-functions)
  - [Game Loop & Update Functions](#game-loop--update-functions)
  - [Input & Restart Functions](#input--restart-functions)
- [Repository Structure](#repository-structure)
- [Further Documentation](#further-documentation)

## Overview

The game simulates a spaceship navigating a starfield while dodging and shooting comets. It includes various game mechanics such as collision detection, comet splitting, explosion effects, power-ups, and multiple game states (menu, playing, win, and game over).

## Function Documentation

### Starfield Functions
- **initStarfield()**  
  Initializes an array of stars with random positions, sizes, and flicker properties. Called on page load and during window resize.

- **updateStarfield(dt)**  
  Updates the flicker timing of each star based on the elapsed time (`dt`).

- **drawStarfield(ctx)**  
  Renders the starfield on the canvas using the provided drawing context.

### Texture & Pattern Functions
- **generateStonePattern()**  
  Creates a stone-like repeating texture pattern used for normal comets.

- **generateRockPattern()**  
  Creates a darker, rocky texture pattern suitable for big comets.

### Planet Drawing Function
- **drawCronos(ctx)**  
  Draws the Cronos planet with a radial gradient and a ring using a dust pattern. The function accepts a canvas drawing context.

### UI Functions
- **drawHealthBar()**  
  Draws the player's health bar in the top-right corner of the canvas.

- **spawnPowerUp()**  
  Creates and returns a power-up object with a random spawn location and velocity.

### Explosion Functions
- **createSmallExplosion(x, y)**  
  Triggers a small explosion at the given coordinates, spawns explosion particles, and plays a sound effect.

- **createHugeExplosion(x, y)**  
  Triggers a large explosion effect similar to `createSmallExplosion` but with more particles.

- **updateExplosionParticles(dt)**  
  Updates the position, size, and lifetime of explosion particles. Removes particles that have expired.

### Comet Functions
- **generateCometShape(segments)**  
  Generates an array of vertices (each with angle, factor, frequency, and phase) that define the shape of a comet.

- **spawnComet(tier, x, y, vx, vy)**  
  Creates a normal comet with a specified tier, position, and velocity. Comets can split upon being hit.

- **spawnBigComet()**  
  Creates a big comet with a unique rocky texture (using `generateRockPattern()`) and a smoother, rounder shape. Big comets are indestructible.

- **checkCometCollisions()**  
  Checks for collisions between normal comets. If collisions occur, the comets may split or be removed, and explosion effects are triggered.

- **checkBigCometCollisions()**  
  Handles collisions among big comets and between big and normal comets, applying bounce physics.

- **drawNormalComets()**  
  Renders normal comets on the canvas with a morphing effect and draws their trails.

- **drawBigComets(ctx)**  
  Renders big comets with their rocky texture. Accepts the canvas drawing context.

### Game Loop & Update Functions
- **gameLoop(now)**  
  The main game loop function that calculates the time delta (`dt`), updates all game objects, and triggers the drawing function. Called recursively via `requestAnimationFrame`.

- **update(dt)**  
  The core update function that handles movement, collision detection, shooting mechanics, spawning comets/power-ups, and transitions between game states.

### Input & Restart Functions
- **restartGame()**  
  Resets the game state to its initial configuration (menu screen), clears all game objects, and reinitializes the starfield. Called when the player restarts after winning or losing.

## Repository Structure

- **README.md**: This documentation file.
- **index.html**: Main HTML file that includes all the game code.
- **/assets**: (Optional) Folder for sound effects, images, and other assets.

## Further Documentation

For more detailed technical documentation, consider integrating [JSDoc](https://jsdoc.app/) to generate HTML docs from your code comments.

---

Feel free to customize this README.md to better fit your project's specifics or to add more details as needed.
