  #include "raylib.h"
  #include <vector>
  #include <cmath>
  #include <string>
  #include <algorithm>
  #include <random>
  #include <memory>
  #include <ctime>

  // Random number generation utilities
  std::mt19937 rng(std::time(nullptr));
  float RandomRange(float min, float max) {
      std::uniform_real_distribution<float> dist(min, max);
      return dist(rng);
  }

  int RandomInt(int min, int max) {
      std::uniform_int_distribution<int> dist(min, max);
      return dist(rng);
  }

  // Game constants
  const int SCREEN_WIDTH = 1280;
  const int SCREEN_HEIGHT = 720;
  const char* GAME_TITLE = "Splitting Comets";

  // Spaceship constants
  const float SHIP_ROT_SPEED = 6.0f;        // radians/sec
  const float SHIP_REVERSE = 400.0f;         // pixels/sec²
  const float BULLET_SPEED = 600.0f;         // pixels/sec
  const float BULLET_RADIUS = 4.0f;
  const float BULLET_INTERVAL = 0.03f;       // sec between shots
  const float FRICTION = 0.99f;
  const float SHIP_SIZE = 30.0f;             // spaceship size
  const float SHIP_ACCEL = 800.0f;           // pixels/sec²

  // Comet constants
  const float COMET_SPEED_MIN = 400.0f;
  const float COMET_SPEED_MAX = 700.0f;
  const float COMET_FRICTION = 0.998f;
  const int MAX_COMET_TIER = 3;
  const float BASE_COMET_RADIUS = 30.0f;
  const float COMET_SPAWN_MIN = 2.0f;
  const float COMET_SPAWN_MAX = 3.0f;
  const int COMET_SPAWN_BATCH_MIN = 1;
  const int COMET_SPAWN_BATCH_MAX = 3;
  const float COMET_MIN_SPEED = 250.0f;

  // Big comets
  const float BIG_COMET_SPAWN_INTERVAL = 25.0f;
  const int BIG_COMET_SPAWN_MAX = 1;
  const float BIG_COMET_SPEED_MIN = 150.0f;
  const float BIG_COMET_SPEED_MAX = 200.0f;
  const float BIG_COMET_SCALE = 5.5f;

  // Power-up constants
  const float POWERUP_SPAWN_INTERVAL = 15.0f;
  const float POWERUP_LIFETIME = 15.0f;
  const float POWERUP_BOX_SIZE = 60.0f;
  const float POWERUP_RADIUS = POWERUP_BOX_SIZE / 2.0f;

  // Star field constants
  const int NUM_STARS = 500;

  // Game states
  enum GameState {
      MENU,
      SETTINGS,
      PLAYING,
      GAMEOVER,
      WIN
  };

  // Forward declarations of structures
  struct CometShapePoint;
  struct Star;
  struct Spark;
  struct Bullet;
  struct ExplosionParticle;
  struct Spaceship;
  struct Comet;
  struct BigComet;
  struct PowerUp;

  // Structure definitions
  struct CometShapePoint {
      float angle;
      float factor;
      float frequency;
      float phase;
  };

  struct Star {
      float x;
      float y;
      float baseSize;
      float sizeVariation;
      float sizeSpeed;
      float sizeTime;
      bool flicker;
  };

  struct Spark {
      float angle;
      float length;
      float lifetime;
  };

  struct Bullet {
      Vector2 position;
      Vector2 velocity;
      float radius;
  };

  struct ExplosionParticle {
      Vector2 position;
      Vector2 velocity;
      float lifetime;
      float size;
      Color color;
  };

  struct Spaceship {
      Vector2 position;
      Vector2 velocity;
      float angle;
      float size;
      int hitPoints;
      float hitFlashTimer;
  };

  struct TrailPoint {
      float x;
      float y;
  };

  struct Comet {
      Vector2 position;
      Vector2 velocity;
      float radius;
      int tier;
      Color color;
      int hitPoints;
      float hitEffectTimer;
      float slowEffectTimer;
      float indestructableTimer;
      float oldVX;
      float oldVY;
      std::vector<TrailPoint> trail;
      float trailTimer;
      std::vector<CometShapePoint> shape;
      float rotationSpeed;
      bool morph;
  };

  struct BigComet {
      Vector2 position;
      Vector2 velocity;
      float radius;
      Color color;
      std::vector<CometShapePoint> shape;
      std::vector<TrailPoint> trail;
      float trailTimer;
      bool morph;
      float rotationSpeed;
  };

  struct PowerUp {
      Vector2 position;
      Vector2 velocity;
      float lifetime;
      float radius;
  };

  // Global variables
  GameState gameState = MENU;
  Spaceship ship;
  std::vector<Bullet> bullets;
  std::vector<Spark> sparks;
  std::vector<Star> stars;
  std::vector<Comet> comets;
  std::vector<BigComet> bigComets;
  std::vector<ExplosionParticle> explosionParticles;
  std::vector<PowerUp> powerUps;

  float gameTime = 0.0f;
  float lastBulletTime = 0.0f;
  float cometSpawnTimer = 0.0f;
  float bigCometSpawnTimer = 0.0f;
  float powerUpSpawnTimer = 0.0f;
  int score = 0;
  bool winTriggered = false;

  // Sound related variables
  Music menuMusic;
  Music gameplayMusic;
  Sound bulletSound;
  Sound hitSound;
  Sound explosionSound;
  Sound powerUpSound;
  Sound victorySound;
  Sound defeatSound;

  bool musicEnabled = true;
  bool soundEnabled = true;

  // Textures and rendering resources
  Texture2D rockTexture;
  RenderTexture2D cometTexture;

  // Function prototypes
  void InitStarfield();
  void UpdateStarfield(float dt);
  void DrawStarfield();
  void DrawCronos();
  void DrawSpaceship();
  void DrawHealthBar();
  void DrawMusicIcon();
  void SpawnComet(int tier = MAX_COMET_TIER, float x = -1, float y = -1, float vx = 0, float vy = 0);
  void SpawnBigComet();
  Vector2 GetSpawnLocation();
  void CreateSmallExplosion(float x, float y);
  void CreateHugeExplosion(float x, float y);
  void UpdateExplosionParticles(float dt);
  void CheckCometCollisions();
  void CheckBigCometCollisions();
  void RestartGame();
  void DrawNormalComets();
  void DrawBigComets();
  void Update(float dt);
  void Draw();
  std::vector<CometShapePoint> GenerateCometShape(int segments);
  std::string GetQuadrant(float x, float y);
  bool IsInSameQuadrant(float x1, float y1, float x2, float y2);
  PowerUp SpawnPowerUp();
  void ToggleMusic();
  void ToggleAllSounds();
  void StartMenuMusic();
  void StartGameplayMusic();

  // Main function
  int main() {
      // Initialize window
      InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
      SetTargetFPS(60);

      // Initialize audio device
      InitAudioDevice();

      // Load sounds
      menuMusic = LoadMusicStream("menu_music.mp3");
      gameplayMusic = LoadMusicStream("ingame_music.mp3");

      bulletSound = LoadSound("mixkit-achievement-bell-600.wav");
      hitSound = LoadSound("mixkit-arcade-retro-game-over.wav");
      explosionSound = LoadSound("expl-sfx.mp3");
      powerUpSound = LoadSound("mixkit-video-game-health-recharge-2837.wav");
      victorySound = LoadSound("mixkit-cheering-crowd-loud-whistle.wav");
      defeatSound = LoadSound("mixkit-arcade-retro-game-over.wav");

      // Set volumes
      SetMusicVolume(menuMusic, 0.1f);
      SetMusicVolume(gameplayMusic, 0.1f);
      SetSoundVolume(victorySound, 0.4f);
      SetSoundVolume(defeatSound, 0.3f);
      SetSoundVolume(bulletSound, 0.25f);
      SetSoundVolume(hitSound, 0.2f);
      SetSoundVolume(explosionSound, 0.125f);
      SetSoundVolume(powerUpSound, 0.3f);

      // Generate rock texture for comets
      Image rockImg = GenImagePerlinNoise(256, 256, 0, 0, 4.0f);
      rockTexture = LoadTextureFromImage(rockImg);
      UnloadImage(rockImg);

      // Initialize game resources
      InitStarfield();

      // Initialize player ship
      ship.position = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
      ship.velocity = (Vector2){ 0, 0 };
      ship.angle = 0;
      ship.size = SHIP_SIZE;
      ship.hitPoints = 5;
      ship.hitFlashTimer = 0;

      // Spawn initial comet
      SpawnComet();

      // Start menu music
      StartMenuMusic();

      // Main game loop
      while (!WindowShouldClose()) {
          // Update music streams
          if (musicEnabled) {
              if (gameState == MENU || gameState == SETTINGS) {
                  UpdateMusicStream(menuMusic);
              } else if (gameState == PLAYING) {
                  UpdateMusicStream(gameplayMusic);
              }
          }

          // Calculate delta time
          float dt = GetFrameTime();

          // Process input
          if (IsKeyPressed(KEY_M)) {
              ToggleMusic();
          }

          if (gameState == PLAYING && IsKeyPressed(KEY_ESCAPE)) {
              gameState = SETTINGS;
          }

          if (gameState == MENU) {
              if (IsKeyPressed(KEY_ENTER)) {
                  gameState = PLAYING;
                  score = 0;
                  gameTime = 0;
                  winTriggered = false;
                  StartGameplayMusic();
              } else if (IsKeyPressed(KEY_S)) {
                  gameState = SETTINGS;
              }
          } else if (gameState == SETTINGS && IsKeyPressed(KEY_ESCAPE)) {
              gameState = MENU;
              StartMenuMusic();
          } else if ((gameState == GAMEOVER || gameState == WIN) && IsKeyPressed(KEY_ENTER)) {
              // Stop sounds
              StopSound(victorySound);
              StopSound(defeatSound);

              RestartGame();
              gameState = MENU;
              StartMenuMusic();
          }

          // Update game
          Update(dt);

          // Draw everything
          BeginDrawing();
          ClearBackground(BLACK);
          Draw();
          EndDrawing();
      }

      // Unload resources
      UnloadTexture(rockTexture);

      // Unload sounds
      UnloadMusicStream(menuMusic);
      UnloadMusicStream(gameplayMusic);
      UnloadSound(bulletSound);
      UnloadSound(hitSound);
      UnloadSound(explosionSound);
      UnloadSound(powerUpSound);
      UnloadSound(victorySound);
      UnloadSound(defeatSound);

      CloseAudioDevice();
      CloseWindow();

      return 0;
  }

  // Initialize stars
  void InitStarfield() {
      stars.clear();
      stars.resize(NUM_STARS);

      for (int i = 0; i < NUM_STARS; i++) {
          Star& star = stars[i];
          star.x = RandomRange(0, SCREEN_WIDTH);
          star.y = RandomRange(0, SCREEN_HEIGHT);
          star.baseSize = RandomRange(0.5f, 2.5f);
          star.flicker = RandomRange(0.0f, 1.0f) < 0.7f;
          star.sizeVariation = star.flicker ? RandomRange(0.2f, 1.2f) : 0.0f;
          star.sizeSpeed = star.flicker ? RandomRange(0.2f, 0.7f) : 0.0f;
          star.sizeTime = star.flicker ? RandomRange(0.0f, 2.0f * PI) : 0.0f;
      }
  }

  // Update starfield
  void UpdateStarfield(float dt) {
      for (auto& star : stars) {
          if (star.flicker) {
              star.sizeTime += dt * star.sizeSpeed;
          }
      }
  }

  // Draw starfield
  void DrawStarfield() {
      for (const auto& star : stars) {
          float currentSize = star.baseSize;
          if (star.flicker) {
              currentSize += star.sizeVariation * sin(star.sizeTime);
              if (currentSize < 0) currentSize = 0;
          }

          // Draw star as a white circle with gradient
          DrawCircleGradient(
              (int)star.x, (int)star.y,
              currentSize,
              ColorAlpha(WHITE, 1.0f),
              ColorAlpha(WHITE, 0.0f)
          );
      }
  }

  // Draw Cronos planet
  void DrawCronos() {
      // Planet position & size
      float x = SCREEN_WIDTH * 0.75f;
      float y = SCREEN_HEIGHT * 0.5f;
      float radius = 200.0f;

      // Draw rings behind the planet
      float ringOuter = radius * 1.9f;
      float ringInner = radius * 1.25f;
      float ringFlatten = 0.45f;

      // Draw the back half of the ring
      BeginBlendMode(BLEND_ALPHA);
      DrawEllipse(
          x, y,
          ringOuter, ringOuter * ringFlatten,
          ColorAlpha((Color){60, 50, 40, 255}, 0.4f)
      );

      // Draw the planet
      DrawCircleGradient(
          x, y, radius,
          (Color){201, 184, 163, 255},
          (Color){127, 111, 90, 255}
      );

      // Draw the front half of the ring
      DrawEllipseLinesEx(
          (Rectangle){x - ringOuter, y - ringOuter * ringFlatten, ringOuter * 2, ringOuter * ringFlatten * 2},
          1.5f,
          ColorAlpha(WHITE, 0.3f)
      );
      EndBlendMode();
  }

  // Draw the player's spaceship
  void DrawSpaceship() {
      // Save original position and angle for transform
      Vector2 pos = ship.position;
      float angle = ship.angle;

      // Determine ship fill color based on hit state
      Color shipFill = GRAY;
      if (ship.hitFlashTimer > 0) {
          shipFill = RED;
      }
      else if (ship.hitPoints <= 2) {
          shipFill = ((int)(gameTime * 10) % 2 == 0) ? RED : GRAY;
      }

      // Draw ship
      BeginMode2D((Camera2D){ {pos.x, pos.y}, {0, 0}, angle * RAD2DEG, 1.0f });

      // Body
      Vector2 points[4] = {
          {ship.size, 0},
          {ship.size * 0.2f, ship.size * 0.7f},
          {-ship.size * 0.6f, 0},
          {ship.size * 0.2f, -ship.size * 0.7f}
      };
      DrawTriangleFan(points, 4, shipFill);
      DrawPolyLines(points, 4, true, 2.0f, WHITE);

      // Top gun
      Vector2 topGun[4] = {
          {ship.size * 0.5f, -ship.size * 0.4f},
          {ship.size * 0.9f, -ship.size * 0.4f},
          {ship.size * 0.9f, -ship.size * 0.6f},
          {ship.size * 0.5f, -ship.size * 0.5f}
      };
      DrawTriangleFan(topGun, 4, DARKGRAY);
      DrawPolyLines(topGun, 4, true, 2.0f, LIGHTGRAY);

      // Bottom gun
      Vector2 bottomGun[4] = {
          {ship.size * 0.5f, ship.size * 0.4f},
          {ship.size * 0.9f, ship.size * 0.4f},
          {ship.size * 0.9f, ship.size * 0.6f},
          {ship.size * 0.5f, ship.size * 0.5f}
      };
      DrawTriangleFan(bottomGun, 4, DARKGRAY);
      DrawPolyLines(bottomGun, 4, true, 2.0f, LIGHTGRAY);

      // Flame (if moving forward)
      if (IsKeyDown(KEY_UP)) {
          float flameLength = ship.size * 0.8f;
          float flameWidth = ship.size * 0.3f;

          Vector2 flame[3] = {
              {-ship.size * 0.6f, flameWidth / 2.0f},
              {-ship.size * 0.6f - flameLength, 0},
              {-ship.size * 0.6f, -flameWidth / 2.0f}
          };

          DrawTriangle(flame[0], flame[1], flame[2], YELLOW);
      }

      // Cockpit
      float cockpitRadius = ship.size * 0.3f;
      DrawCircleGradient(
          ship.size * 0.3f, 0,
          cockpitRadius,
          SKYBLUE, BLUE
      );
      DrawCircleLines(ship.size * 0.3f, 0, cockpitRadius, WHITE);

      EndMode2D();
  }

  // Draw health bar
  void DrawHealthBar() {
      const float barWidth = 100.0f;
      const float barHeight = 15.0f;
      const float margin = 20.0f;
      const float x = SCREEN_WIDTH - margin - barWidth;
      const float y = margin;

      DrawRectangleLinesEx((Rectangle){x, y, barWidth, barHeight}, 2.0f, WHITE);

      float hpFraction = (float)ship.hitPoints / 5.0f;
      DrawRectangle(x, y, barWidth * hpFraction, barHeight, RED);
  }

  // Draw music/sound icon
  void DrawMusicIcon() {
      const float iconSize = 25.0f;
      const float margin = 20.0f;
      const float x = margin;
      const float y = margin;

      // Draw speaker base
      Vector2 speakerBase[5] = {
          {x, y + iconSize/2},
          {x + iconSize/3, y + iconSize/2},
          {x + iconSize*2/3, y},
          {x + iconSize*2/3, y + iconSize},
          {x + iconSize/3, y + iconSize/2}
      };
      DrawTriangleFan(speakerBase, 5, WHITE);

      // Draw sound waves if sound is enabled
      if (soundEnabled) {
          // First wave
          DrawArc(x + iconSize*2/3, y + iconSize/2, iconSize/3, -PI/4, PI/4, 0, WHITE);

          // Second wave
          DrawArc(x + iconSize*2/3, y + iconSize/2, iconSize*2/3, -PI/4, PI/4, 0, WHITE);
      } else {
          // Draw X over the speaker when sound is off
          DrawLine(x + iconSize/3, y + iconSize/4, x + iconSize, y + iconSize*3/4, WHITE);
          DrawLine(x + iconSize, y + iconSize/4, x + iconSize/3, y + iconSize*3/4, WHITE);
      }
  }

  // Generate a shape for a comet
  std::vector<CometShapePoint> GenerateCometShape(int segments) {
      std::vector<CometShapePoint> shape;

      for (int i = 0; i < segments; i++) {
          CometShapePoint point;
          point.angle = i * 2.0f * PI / segments;
          point.factor = 1.0f + (RandomRange(0.0f, 1.0f) * 0.3f - 0.15f);
          point.frequency = (RandomRange(1.0f, 3.0f) * 2.0f);
          point.phase = RandomRange(0.0f, 2.0f * PI);
          shape.push_back(point);
      }

      return shape;
  }

  // Spawn a comet
  void SpawnComet(int tier, float x, float y, float vx, float vy) {
      Comet comet;

      // Position
      if (x < 0 || y < 0) {
          Vector2 loc = GetSpawnLocation();
          comet.position.x = loc.x;
          comet.position.y = loc.y;
      } else {
          comet.position.x = x;
          comet.position.y = y;
      }

      // Velocity
      if (vx == 0 && vy == 0) {
          float angle = RandomRange(0.0f, 2.0f * PI);
          float speed = RandomRange(COMET_SPEED_MIN, COMET_SPEED_MAX);
          comet.velocity.x = cosf(angle) * speed;
          comet.velocity.y = sinf(angle) * speed;
      } else {
          comet.velocity.x = vx;
          comet.velocity.y = vy;
      }

      // Size and properties
      comet.radius = BASE_COMET_RADIUS * powf(0.6f, MAX_COMET_TIER - tier);

      // Health based on tier
      if (tier == 3) comet.hitPoints = 5;
      else if (tier == 2) comet.hitPoints = 4;
      else comet.hitPoints = 3;

      comet.tier = tier;
      comet.color = DARKGRAY;
      comet.hitEffectTimer = 0;
      comet.slowEffectTimer = 0;
      comet.indestructableTimer = 0;
      comet.oldVX = comet.velocity.x;
      comet.oldVY = comet.velocity.y;
      comet.trailTimer = 0;
      comet.shape = GenerateCometShape(12); // More segments for rounder comets
      comet.morph = true;
      comet.rotationSpeed = RandomRange(1.0f, 3.0f);

      comets.push_back(comet);
  }

  // Spawn a big comet
  void SpawnBigComet() {
      BigComet bigComet;
      float x, y, angle;

      // Choose starting position that will cross closer to center screen
      if (RandomRange(0.0f, 1.0f) < 0.5f) {
          // Spawn from left or right edge
          x = RandomRange(0.0f, 1.0f) < 0.5f ? -BASE_COMET_RADIUS * BIG_COMET_SCALE : SCREEN_WIDTH + BASE_COMET_RADIUS *
  BIG_COMET_SCALE;

          // Calculate y to aim toward center-ish area (with variation)
          float targetY = SCREEN_HEIGHT * (0.3f + RandomRange(0.0f, 1.0f) * 0.4f);
          y = RandomRange(0.0f, SCREEN_HEIGHT);

          // Calculate angle toward target point near center
          angle = atan2f(targetY - y, (SCREEN_WIDTH/2.0f) - x);
      } else {
          // Spawn from top or bottom edge
          y = RandomRange(0.0f, 1.0f) < 0.5f ? -BASE_COMET_RADIUS * BIG_COMET_SCALE : SCREEN_HEIGHT + BASE_COMET_RADIUS *
  BIG_COMET_SCALE;

          // Calculate x to aim toward center-ish area (with variation)
          float targetX = SCREEN_WIDTH * (0.3f + RandomRange(0.0f, 1.0f) * 0.4f);
          x = RandomRange(0.0f, SCREEN_WIDTH);

          // Calculate angle toward target point near center
          angle = atan2f((SCREEN_HEIGHT/2.0f) - y, targetX - x);
      }

      // Slow down big comets for longer screen time
      float speed = RandomRange(BIG_COMET_SPEED_MIN * 0.7f, BIG_COMET_SPEED_MAX * 0.7f);

      bigComet.position.x = x;
      bigComet.position.y = y;
      bigComet.velocity.x = cosf(angle) * speed;
      bigComet.velocity.y = sinf(angle) * speed;
      bigComet.radius = BASE_COMET_RADIUS * BIG_COMET_SCALE;
      bigComet.color = (Color){205, 127, 50, 255}; // Bronze color

      // Use more segments for a smoother, rounder shape
      std::vector<CometShapePoint> shape = GenerateCometShape(18);

      // Make big comets much more round with minimal variation
      for (auto& point : shape) {
          // Minimal variation for near-perfect roundness
          point.factor = 1.0f + (point.factor - 1.0f) * 0.05f;
          // Greatly reduce frequency for slower rotation
          point.frequency = point.frequency * 0.3f;
      }

      bigComet.shape = shape;
      bigComet.trailTimer = 0;
      bigComet.morph = true;
      bigComet.rotationSpeed = RandomRange(0.3f, 0.5f);

      bigComets.push_back(bigComet);
  }

  // Get a spawn location for comets and power-ups
  Vector2 GetSpawnLocation() {
      Vector2 loc;
      int attempt = 0;

      do {
          if (RandomRange(0.0f, 1.0f) < 0.5f) {
              loc.x = RandomRange(0.0f, SCREEN_WIDTH);
              loc.y = 0;
          } else {
              loc.x = RandomRange(0.0f, SCREEN_WIDTH);
              loc.y = SCREEN_HEIGHT;
          }
          attempt++;
          if (attempt > 10) break;
      } while (IsInSameQuadrant(loc.x, loc.y, ship.position.x, ship.position.y));

      return loc;
  }

  // Determine quadrant of the screen
  std::string GetQuadrant(float x, float y) {
      const float midX = SCREEN_WIDTH / 2.0f;
      const float midY = SCREEN_HEIGHT / 2.0f;

      if (x < midX && y < midY) return "top-left";
      if (x >= midX && y < midY) return "top-right";
      if (x < midX && y >= midY) return "bottom-left";
      return "bottom-right";
  }

  // Check if two positions are in the same quadrant
  bool IsInSameQuadrant(float x1, float y1, float x2, float y2) {
      return GetQuadrant(x1, y1) == GetQuadrant(x2, y2);
  }

  // Create a small explosion
  void CreateSmallExplosion(float x, float y) {
      if (soundEnabled) {
          PlaySound(explosionSound);
      }

      const int count = 10;
      for (int i = 0; i < count; i++) {
          float angle = RandomRange(0.0f, 2.0f * PI);
          float speed = RandomRange(50.0f, 150.0f);

          ExplosionParticle particle;
          particle.position = (Vector2){x, y};
          particle.velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed};
          particle.lifetime = RandomRange(0.3f, 0.7f);
          particle.size = RandomRange(2.0f, 4.0f);
          particle.color = ORANGE;

          explosionParticles.push_back(particle);
      }
  }

  // Create a huge explosion
  void CreateHugeExplosion(float x, float y) {
      if (soundEnabled) {
          PlaySound(explosionSound);
      }

      const int count = 50;
      for (int i = 0; i < count; i++) {
          float angle = RandomRange(0.0f, 2.0f * PI);
          float speed = RandomRange(100.0f, 300.0f);

          ExplosionParticle particle;
          particle.position = (Vector2){x, y};
          particle.velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed};
          particle.lifetime = RandomRange(0.8f, 1.5f);
          particle.size = RandomRange(5.0f, 10.0f);
          particle.color = ORANGE;

          explosionParticles.push_back(particle);
      }
  }

  // Update explosion particles
  void UpdateExplosionParticles(float dt) {
      for (int i = explosionParticles.size() - 1; i >= 0; i--) {
          ExplosionParticle& p = explosionParticles[i];

          p.position.x += p.velocity.x * dt;
          p.position.y += p.velocity.y * dt;
          p.lifetime -= dt;
          p.size *= 0.98f;

          if (p.lifetime <= 0) {
              explosionParticles.erase(explosionParticles.begin() + i);
          }
      }
  }

  // Check for collisions between comets
  void CheckCometCollisions() {
      std::vector<int> indicesToRemove;

      for (size_t i = 0; i < comets.size(); i++) {
          for (size_t j = i + 1; j < comets.size(); j++) {
              Comet& c1 = comets[i];
              Comet& c2 = comets[j];

              float dx = c2.position.x - c1.position.x;
              float dy = c2.position.y - c1.position.y;
              float dist = sqrtf(dx*dx + dy*dy);
              float minDist = c1.radius + c2.radius;

              // Colliding
              if (dist < minDist && dist > 0) {
                  // If either is < MAX tier, we remove it
                  if (c1.tier < MAX_COMET_TIER) {
                      indicesToRemove.push_back(i);
                      CreateSmallExplosion(c1.position.x, c1.position.y);
                      score += 5;
                  }

                  if (c2.tier < MAX_COMET_TIER) {
                      indicesToRemove.push_back(j);
                      CreateSmallExplosion(c2.position.x, c2.position.y);
                      score += 5;
                  }

                  // If both are MAX tier, bounce
                  if (c1.tier == MAX_COMET_TIER && c2.tier == MAX_COMET_TIER) {
                      float nx = dx / dist;
                      float ny = dy / dist;
                      float dvx = c1.velocity.x - c2.velocity.x;
                      float dvy = c1.velocity.y - c2.velocity.y;
                      float dot = dvx * nx + dvy * ny;

                      if (dot <= 0) {
                          float impulse = dot;
                          c1.velocity.x -= impulse * nx;
                          c1.velocity.y -= impulse * ny;
                          c2.velocity.x += impulse * nx;
                          c2.velocity.y += impulse * ny;

                          float overlap = minDist - dist;
                          c1.position.x -= nx * overlap / 2.0f;
                          c1.position.y -= ny * overlap / 2.0f;
                          c2.position.x += nx * overlap / 2.0f;
                          c2.position.y += ny * overlap / 2.0f;
                      }
                  }
              }
          }
      }

      // Remove comets that were marked (in reverse order to avoid invalidating indices)
      std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());
      for (auto index : indicesToRemove) {
          if (index >= 0 && index < (int)comets.size()) {
              comets.erase(comets.begin() + index);
          }
      }
  }

  // Check for collisions between big comets
  void CheckBigCometCollisions() {
      // Big Comets vs. Big Comets
      for (size_t i = 0; i < bigComets.size(); i++) {
          for (size_t j = i + 1; j < bigComets.size(); j++) {
              BigComet& bc1 = bigComets[i];
              BigComet& bc2 = bigComets[j];

              float dx = bc2.position.x - bc1.position.x;
              float dy = bc2.position.y - bc1.position.y;
              float dist = sqrtf(dx*dx + dy*dy);
              float minDist = bc1.radius + bc2.radius;

              if (dist < minDist && dist > 0) {
                  float nx = dx / dist;
                  float ny = dy / dist;
                  float overlap = minDist - dist;

                  // Bounce them apart equally
                  bc1.position.x -= nx * (overlap / 2.0f);
                  bc1.position.y -= ny * (overlap / 2.0f);
                  bc2.position.x += nx * (overlap / 2.0f);
                  bc2.position.y += ny * (overlap / 2.0f);

                  // Adjust velocities (to simulate a bounce)
                  float impulse = (bc1.velocity.x - bc2.velocity.x) * nx + (bc1.velocity.y - bc2.velocity.y) * ny;
                  if (impulse < 0) {
                      bc1.velocity.x -= impulse * nx;
                      bc1.velocity.y -= impulse * ny;
                      bc2.velocity.x += impulse * nx;
                      bc2.velocity.y += impulse * ny;
                  }
              }
          }
      }

      // Big Comets vs. Normal Comets
      for (size_t i = 0; i < bigComets.size(); i++) {
          for (size_t j = 0; j < comets.size(); j++) {
              BigComet& bc = bigComets[i];
              Comet& c = comets[j];

              float dx = c.position.x - bc.position.x;
              float dy = c.position.y - bc.position.y;
              float dist = sqrtf(dx*dx + dy*dy);
              float minDist = bc.radius + c.radius;

              if (dist < minDist && dist > 0) {
                  float nx = dx / dist;
                  float ny = dy / dist;
                  float overlap = minDist - dist;

                  // Adjust positions so they bounce apart
                  c.position.x += nx * (overlap / 2.0f);
                  c.position.y += ny * (overlap / 2.0f);
                  bc.position.x -= nx * (overlap / 2.0f);
                  bc.position.y -= ny * (overlap / 2.0f);

                  // Bounce the normal comet more strongly
                  c.velocity.x = -c.velocity.x * 0.5f;
                  c.velocity.y = -c.velocity.y * 0.5f;
              }
          }
      }
  }

  // Spawn a power-up
  PowerUp SpawnPowerUp() {
      Vector2 loc = GetSpawnLocation();

      PowerUp powerUp;
      powerUp.position = loc;
      powerUp.velocity = (Vector2){
          RandomRange(-50.0f, 50.0f),
          RandomRange(-50.0f, 50.0f)
      };
      powerUp.lifetime = 10.0f;
      powerUp.radius = POWERUP_RADIUS;

      return powerUp;
  }

  // Toggle music on/off
  void ToggleMusic() {
      musicEnabled = !musicEnabled;
      soundEnabled = musicEnabled; // Link sound effects to music toggle

      if (musicEnabled) {
          // Resume music based on current game state
          if (gameState == MENU || gameState == SETTINGS) {
              StartMenuMusic();
          } else if (gameState == PLAYING) {
              StartGameplayMusic();
          }
      } else {
          // Stop music
          StopMusicStream(menuMusic);
          StopMusicStream(gameplayMusic);
      }
  }

  // Toggle all sounds
  void ToggleAllSounds() {
      soundEnabled = !soundEnabled;

      if (!soundEnabled) {
          // Turn off music
          musicEnabled = false;
          StopMusicStream(menuMusic);
          StopMusicStream(gameplayMusic);
      }
  }

  // Start menu music
  void StartMenuMusic() {
      StopMusicStream(gameplayMusic);

      if (musicEnabled) {
          PlayMusicStream(menuMusic);
      }
  }

  // Start gameplay music
  void StartGameplayMusic() {
      StopMusicStream(menuMusic);

      if (musicEnabled) {
          PlayMusicStream(gameplayMusic);
      }
  }

  // Restart the game
  void RestartGame() {
      gameState = MENU;
      StartMenuMusic();

      ship.position = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
      ship.velocity = (Vector2){ 0, 0 };
      ship.angle = 0;
      ship.hitPoints = 5;
      ship.hitFlashTimer = 0;

      bullets.clear();
      sparks.clear();
      comets.clear();
      bigComets.clear();
      explosionParticles.clear();
      powerUps.clear();

      lastBulletTime = 0;
      cometSpawnTimer = 0;
      bigCometSpawnTimer = 0;
      powerUpSpawnTimer = 0;
      score = 0;
      gameTime = 0;
      winTriggered = false;

      // Spawn an initial comet
      SpawnComet();
      InitStarfield();
  }

  // Draw the normal comets
  void DrawNormalComets() {
      for (const auto& comet : comets) {
          // Draw comet trail
          if (!comet.trail.empty()) {
              float maxAlpha = 0.1f;
              for (size_t i = 0; i < comet.trail.size(); i++) {
                  const auto& pos = comet.trail[i];
                  float alpha = ((float)(i + 1) / comet.trail.size()) * maxAlpha;

                  DrawCircle(pos.x, pos.y, comet.radius, ColorAlpha(SKYBLUE, alpha));
              }
          }

          // Draw comet body with shape
          if (!comet.shape.empty()) {
              // Calculate all points first
              std::vector<Vector2> points;
              points.reserve(comet.shape.size());

              for (const auto& v : comet.shape) {
                  float factor = v.factor;
                  if (comet.morph) {
                      const float rotSpeed = comet.rotationSpeed;
                      factor += 0.1f * sin(gameTime * v.frequency * rotSpeed + v.phase);
                  }

                  float x = comet.position.x + comet.radius * factor * cos(v.angle);
                  float y = comet.position.y + comet.radius * factor * sin(v.angle);
                  points.push_back((Vector2){x, y});
              }

              // Draw filled polygon
              DrawTriangleFan(points.data(), points.size(), ColorAlpha(comet.color, 0.8f));

              // Draw outline
              DrawPolyLines(points.data(), points.size(), true, 2.0f, comet.color);
          } else {
              // Fallback to circle if no shape
              DrawCircle(comet.position.x, comet.position.y, comet.radius, ColorAlpha(comet.color, 0.8f));
              DrawCircleLines(comet.position.x, comet.position.y, comet.radius, comet.color);
          }
      }
  }

  // Draw the big comets
  void DrawBigComets() {
      for (const auto& bc : bigComets) {
          // Draw big comet with shape
          if (!bc.shape.empty()) {
              // Calculate all points first
              std::vector<Vector2> points;
              points.reserve(bc.shape.size());

              for (const auto& v : bc.shape) {
                  float factor = v.factor;
                  if (bc.morph) {
                      const float rotSpeed = bc.rotationSpeed;
                      factor += 0.1f * sin(gameTime * v.frequency * rotSpeed + v.phase);
                  }

                  float x = bc.position.x + bc.radius * factor * cos(v.angle);
                  float y = bc.position.y + bc.radius * factor * sin(v.angle);
                  points.push_back((Vector2){x, y});
              }

              // Draw filled polygon with texture
              DrawTriangleFan(points.data(), points.size(), bc.color);

              // Draw outline
              DrawPolyLines(points.data(), points.size(), true, 1.5f, ColorAlpha((Color){50, 30, 20, 255}, 0.3f));
          } else {
              // Fallback to circle if no shape
              DrawCircle(bc.position.x, bc.position.y, bc.radius, bc.color);
              DrawCircleLines(bc.position.x, bc.position.y, bc.radius, ColorAlpha((Color){50, 30, 20, 255}, 0.3f));
          }
      }
  }

  // Update game state
  void Update(float dt) {
      // Always update starfield
      UpdateStarfield(dt);

      // Update ship hit flash timer
      if (ship.hitFlashTimer > 0) {
          ship.hitFlashTimer -= dt;
          if (ship.hitFlashTimer < 0) ship.hitFlashTimer = 0;
      }

      // if in menu/settings/gameover/win, do minimal
      if (gameState != PLAYING) {
          return;
      }

      // Increase global time so comet morph can happen
      gameTime += dt;

      // Power-ups
      powerUpSpawnTimer += dt;
      if (powerUpSpawnTimer >= POWERUP_SPAWN_INTERVAL) {
          powerUps.push_back(SpawnPowerUp());
          powerUpSpawnTimer = 0;
      }

      for (int i = powerUps.size() - 1; i >= 0; i--) {
          PowerUp& pu = powerUps[i];
          pu.lifetime -= dt;
          pu.position.x += pu.velocity.x * dt;
          pu.position.y += pu.velocity.y * dt;

          // Bounce off walls
          if (pu.position.x - pu.radius < 0) {
              pu.position.x = pu.radius;
              pu.velocity.x = -pu.velocity.x;
          }
          if (pu.position.x + pu.radius > SCREEN_WIDTH) {
              pu.position.x = SCREEN_WIDTH - pu.radius;
              pu.velocity.x = -pu.velocity.x;
          }
          if (pu.position.y - pu.radius < 0) {
              pu.position.y = pu.radius;
              pu.velocity.y = -pu.velocity.y;
          }
          if (pu.position.y + pu.radius > SCREEN_HEIGHT) {
              pu.position.y = SCREEN_HEIGHT - pu.radius;
              pu.velocity.y = -pu.velocity.y;
          }

          if (pu.lifetime <= 0) {
              powerUps.erase(powerUps.begin() + i);
              continue;
          }

          // Check if spaceship picks it up
          float dx = ship.position.x - pu.position.x;
          float dy = ship.position.y - pu.position.y;
          if (sqrt(dx*dx + dy*dy) < (ship.size/2 + pu.radius)) {
              // Refill HP
              ship.hitPoints = std::min(5, ship.hitPoints + 3);

              if (soundEnabled) {
                  PlaySound(powerUpSound);
              }

              powerUps.erase(powerUps.begin() + i);
              continue;
          }
      }

      // Win condition if survive 90s and no comets left
      bool spawnAllowed = gameTime < 90.0f;
      if (gameTime >= 60.0f && comets.empty() && !winTriggered) {
          CreateHugeExplosion(ship.position.x, ship.position.y);
          winTriggered = true;
          gameState = WIN;
          StopMusicStream(gameplayMusic);

          if (soundEnabled) {
              PlaySound(victorySound);
          }
      }

      // Spaceship vs comets
      for (size_t i = 0; i < comets.size(); i++) {
          Comet& c = comets[i];
          float dx = ship.position.x - c.position.x;
          float dy = ship.position.y - c.position.y;
          float dist = sqrt(dx*dx + dy*dy);

          if (dist < ship.size + c.radius) {
              float nx = dx / dist;
              float ny = dy / dist;
              float overlap = (ship.size + c.radius) - dist;

              // Separate them
              ship.position.x += nx * (overlap / 2.0f);
              ship.position.y += ny * (overlap / 2.0f);
              c.position.x -= nx * (overlap / 2.0f);
              c.position.y -= ny * (overlap / 2.0f);

              // Bounce velocities
              ship.velocity.x = -ship.velocity.x * 0.5f;
              ship.velocity.y = -ship.velocity.y * 0.5f;
              c.velocity.x = -c.velocity.x * 0.3f;
              c.velocity.y = -c.velocity.y * 0.3f;

              // Damage spaceship
              ship.hitPoints--;
              ship.hitFlashTimer = 0.3f;

              if (ship.hitPoints <= 0) {
                  comets.clear();
                  bigComets.clear();
                  bullets.clear();
                  sparks.clear();

                  CreateHugeExplosion(ship.position.x, ship.position.y);
                  gameState = GAMEOVER;
                  StopMusicStream(gameplayMusic);

                  if (soundEnabled) {
                      PlaySound(defeatSound);
                  }

                  return;
              }
          }
      }

      // Spaceship vs big comets
      for (size_t i = 0; i < bigComets.size(); i++) {
          BigComet& bc = bigComets[i];
          float dx = ship.position.x - bc.position.x;
          float dy = ship.position.y - bc.position.y;
          float dist = sqrt(dx*dx + dy*dy);

          if (dist < ship.size * 1.2f + bc.radius) {
              float nx = dx / dist;
              float ny = dy / dist;
              float overlap = (ship.size + bc.radius) - dist;

              ship.position.x += nx * (overlap / 2.0f);
              ship.position.y += ny * (overlap / 2.0f);
              ship.velocity.x = -ship.velocity.x * 0.5f;
              ship.velocity.y = -ship.velocity.y * 0.5f;

              ship.hitPoints--;
              ship.hitFlashTimer = 0.3f;

              if (ship.hitPoints <= 0) {
                  comets.clear();
                  bigComets.clear();
                  bullets.clear();
                  sparks.clear();

                  CreateHugeExplosion(ship.position.x, ship.position.y);
                  gameState = GAMEOVER;
                  StopMusicStream(gameplayMusic);

                  if (soundEnabled) {
                      PlaySound(defeatSound);
                  }

                  return;
              }
          }
      }

      // Movement controls
      if (IsKeyDown(KEY_UP)) {
          ship.velocity.x += cos(ship.angle) * SHIP_ACCEL * dt;
          ship.velocity.y += sin(ship.angle) * SHIP_ACCEL * dt;
      }

      if (IsKeyDown(KEY_DOWN)) {
          ship.velocity.x -= cos(ship.angle) * SHIP_REVERSE * dt;
          ship.velocity.y -= sin(ship.angle) * SHIP_REVERSE * dt;
      }

      if (IsKeyDown(KEY_SPACE)) {
          // Quick brake
          float speed = sqrt(ship.velocity.x * ship.velocity.x + ship.velocity.y * ship.velocity.y);
          if (speed > 0) {
              float deceleration = SHIP_REVERSE * dt;
              float newSpeed = std::max(0.0f, speed - deceleration);
              ship.velocity.x = (ship.velocity.x / speed) * newSpeed;
              ship.velocity.y = (ship.velocity.y / speed) * newSpeed;
          }
      }

      if (IsKeyDown(KEY_LEFT)) {
          ship.angle -= SHIP_ROT_SPEED * dt;
      }

      if (IsKeyDown(KEY_RIGHT)) {
          ship.angle += SHIP_ROT_SPEED * dt;
      }

      // Spaceship movement
      ship.position.x += ship.velocity.x * dt;
      ship.position.y += ship.velocity.y * dt;
      ship.velocity.x *= FRICTION;
      ship.velocity.y *= FRICTION;

      // Keep ship within bounds
      if (ship.position.x - ship.size < 0) {
          ship.position.x = ship.size;
          ship.velocity.x = -ship.velocity.x * 0.3f;
      }
      if (ship.position.x + ship.size > SCREEN_WIDTH) {
          ship.position.x = SCREEN_WIDTH - ship.size;
          ship.velocity.x = -ship.velocity.x * 0.3f;
      }
      if (ship.position.y - ship.size < 0) {
          ship.position.y = ship.size;
          ship.velocity.y = -ship.velocity.y * 0.3f;
      }
      if (ship.position.y + ship.size > SCREEN_HEIGHT) {
          ship.position.y = SCREEN_HEIGHT - ship.size;
          ship.velocity.y = -ship.velocity.y * 0.3f;
      }

      // Shooting
      lastBulletTime += dt;
      if (lastBulletTime > BULLET_INTERVAL) {
          // Triple bullets from different gun positions
          const float noseX = ship.position.x + cos(ship.angle) * ship.size;
          const float noseY = ship.position.y + sin(ship.angle) * ship.size;
          const float perpX = cos(ship.angle + PI/2);
          const float perpY = sin(ship.angle + PI/2);

          // Positions for each gun
          const float topGunX = ship.position.x + cos(ship.angle) * (ship.size * 0.8f) + perpX * (ship.size * 0.4f);
          const float topGunY = ship.position.y + sin(ship.angle) * (ship.size * 0.8f) + perpY * (ship.size * 0.4f);

          const float bottomGunX = ship.position.x + cos(ship.angle) * (ship.size * 0.8f) - perpX * (ship.size * 0.4f);
          const float bottomGunY = ship.position.y + sin(ship.angle) * (ship.size * 0.8f) - perpY * (ship.size * 0.4f);

          // Slight angle variation for more natural shot
          const float topAngle = ship.angle + RandomRange(-0.05f, 0.05f);
          const float centerAngle = ship.angle + RandomRange(-0.03f, 0.03f);
          const float bottomAngle = ship.angle + RandomRange(-0.05f, 0.05f);

          // Bullet spawn positions from each gun
          std::vector<Vector2> spawnPositions = {
              {topGunX, topGunY},       // Top gun
              {noseX, noseY},           // Center gun
              {bottomGunX, bottomGunY}  // Bottom gun
          };

          std::vector<float> bulletAngles = {topAngle, centerAngle, bottomAngle};

          for (size_t i = 0; i < 3; i++) {
              const Vector2& pos = spawnPositions[i];
              const float angle = bulletAngles[i];

              Bullet bullet;
              bullet.position = pos;
              bullet.velocity.x = cos(angle) * BULLET_SPEED + ship.velocity.x;
              bullet.velocity.y = sin(angle) * BULLET_SPEED + ship.velocity.y;
              bullet.radius = BULLET_RADIUS;

              bullets.push_back(bullet);
          }

          if (soundEnabled) {
              PlaySound(bulletSound);
          }

          lastBulletTime = 0;

          // Create sparks
          const int numSparks = (int)RandomRange(1, 3);
          for (int i = 0; i < numSparks; i++) {
              const float baseSparkOffset = PI/8;
              const float sparkAngle = ship.angle + (RandomRange(0.0f, 1.0f) < 0.5f ? baseSparkOffset : -baseSparkOffset) +
   RandomRange(-0.05f, 0.05f);

              Spark spark;
              spark.angle = sparkAngle;
              spark.length = RandomRange(2.0f, 5.0f);
              spark.lifetime = 0.2f;

              sparks.push_back(spark);
          }
      }

      // Update sparks
      for (int i = sparks.size() - 1; i >= 0; i--) {
          sparks[i].lifetime -= dt;
          if (sparks[i].lifetime <= 0) {
              sparks.erase(sparks.begin() + i);
          }
      }

      // Update bullets
      for (int i = bullets.size() - 1; i >= 0; i--) {
          Bullet& b = bullets[i];
          b.position.x += b.velocity.x * dt;
          b.position.y += b.velocity.y * dt;

          // Off screen => remove
          if (b.position.x < 0 || b.position.x > SCREEN_WIDTH ||
              b.position.y < 0 || b.position.y > SCREEN_HEIGHT) {
              bullets.erase(bullets.begin() + i);
          }
      }

      // Update comets
      for (auto& c : comets) {
          if (c.slowEffectTimer > 0) {
              c.slowEffectTimer -= dt;
              if (c.slowEffectTimer < 0) c.slowEffectTimer = 0;
          }

          if (c.indestructableTimer > 0) {
              c.indestructableTimer -= dt;
              if (c.indestructableTimer < 0) c.indestructableTimer = 0;
          }

          if (c.hitEffectTimer > 0) {
              c.hitEffectTimer -= dt;
              if (c.hitEffectTimer < 0) c.hitEffectTimer = 0;
          }

          c.position.x += c.velocity.x * dt;
          c.position.y += c.velocity.y * dt;
          c.velocity.x *= COMET_FRICTION;
          c.velocity.y *= COMET_FRICTION;

          // Never drop below COMET_MIN_SPEED
          float speed = sqrt(c.velocity.x * c.velocity.x + c.velocity.y * c.velocity.y);
          if (speed < COMET_MIN_SPEED && speed > 0) {
              float factor = COMET_MIN_SPEED / speed;
              c.velocity.x *= factor;
              c.velocity.y *= factor;
          }

          // Bounce off screen edges
          if (c.position.x - c.radius < 0) {
              c.position.x = c.radius;
              c.velocity.x = -c.velocity.x;
          }
          if (c.position.x + c.radius > SCREEN_WIDTH) {
              c.position.x = SCREEN_WIDTH - c.radius;
              c.velocity.x = -c.velocity.x;
          }
          if (c.position.y - c.radius < 0) {
              c.position.y = c.radius;
              c.velocity.y = -c.velocity.y;
          }
          if (c.position.y + c.radius > SCREEN_HEIGHT) {
              c.position.y = SCREEN_HEIGHT - c.radius;
              c.velocity.y = -c.velocity.y;
          }

          // Update trail
          c.trailTimer += dt;
          if (c.trailTimer >= 0.05f) { // TRAIL_INTERVAL
              c.trailTimer -= 0.05f;
              c.trail.push_back((TrailPoint){c.position.x, c.position.y});
              if (c.trail.size() > 5) {
                  c.trail.erase(c.trail.begin());
              }
          }
      }

      // Update big comets
      for (int i = bigComets.size() - 1; i >= 0; i--) {
          BigComet& bc = bigComets[i];
          bc.position.x += bc.velocity.x * dt;
          bc.position.y += bc.velocity.y * dt;

          // Remove if off screen
          if (bc.position.x < -bc.radius || bc.position.x > SCREEN_WIDTH + bc.radius ||
              bc.position.y < -bc.radius || bc.position.y > SCREEN_HEIGHT + bc.radius) {
              bigComets.erase(bigComets.begin() + i);
          }
      }

      // Spawn big comets over time
      bigCometSpawnTimer += dt;
      if (bigCometSpawnTimer >= BIG_COMET_SPAWN_INTERVAL) {
          for (int i = 0; i < BIG_COMET_SPAWN_MAX; i++) {
              SpawnBigComet();
          }
          bigCometSpawnTimer = 0;
      }

      // Update explosion particles
      UpdateExplosionParticles(dt);

      // Bullets vs Comets
      for (int i = bullets.size() - 1; i >= 0; i--) {
          Bullet& b = bullets[i];
          bool bulletHit = false;

          for (int j = comets.size() - 1; j >= 0; j--) {
              Comet& c = comets[j];

              if (c.indestructableTimer > 0) continue;

              float dx = b.position.x - c.position.x;
              float dy = b.position.y - c.position.y;
              if (sqrt(dx*dx + dy*dy) < c.radius) {
                  bulletHit = true;

                  if (soundEnabled) {
                      PlaySound(hitSound);
                  }

                  c.hitPoints--;

                  // Slow effect if tier 3
                  if (c.tier == 3 && c.slowEffectTimer == 0) {
                      c.velocity.x *= 0.66f;
                      c.velocity.y *= 0.66f;
                      c.slowEffectTimer = 0.5f;
                  }

                  c.hitEffectTimer = 0.5f;

                  // Destroyed
                  if (c.hitPoints <= 0) {
                      CreateSmallExplosion(c.position.x, c.position.y);

                      // If bigger than tier1 => split
                      if (c.tier > 1) {
                          const int newTier = c.tier - 1;
                          const float baseSpeed = sqrt(c.velocity.x * c.velocity.x + c.velocity.y * c.velocity.y);
                          int splits = (int)RandomRange(2, 6);
                          const float offset = 50.0f;

                          for (int k = 0; k < splits; k++) {
                              float newAngle = RandomRange(0.0f, 2.0f * PI);
                              float factor = RandomRange(0.7f, 0.8f);
                              float extraFactor = RandomRange(1.2f, 1.5f);
                              float newSpeed = baseSpeed * factor * extraFactor;

                              SpawnComet(
                                  newTier,
                                  c.position.x + cos(newAngle) * offset,
                                  c.position.y + sin(newAngle) * offset,
                                  cos(newAngle) * newSpeed,
                                  sin(newAngle) * newSpeed
                              );

                              // Short indestructable so they don't instantly collide
                              comets.back().indestructableTimer = 0.01f;
                          }
                      }

                      score += 10;
                      comets.erase(comets.begin() + j);
                  }

                  break;
              }
          }

          // If not hit normal comet, check big comet
          if (!bulletHit) {
              for (int j = bigComets.size() - 1; j >= 0; j--) {
                  BigComet& bc = bigComets[j];
                  float dx = b.position.x - bc.position.x;
                  float dy = b.position.y - bc.position.y;

                  if (sqrt(dx*dx + dy*dy) < bc.radius) {
                      bulletHit = true;
                      // Big comets are indestructible
                      break;
                  }
              }
          }

          if (bulletHit) {
              bullets.erase(bullets.begin() + i);
          }
      }

      // Check collisions among comets
      CheckCometCollisions();
      CheckBigCometCollisions();

      // Spawn new comets over time if < 90s
      if (spawnAllowed) {
          cometSpawnTimer += dt;
          if (cometSpawnTimer > RandomRange(COMET_SPAWN_MIN, COMET_SPAWN_MAX)) {
              // Scale spawn count based on area
              float area = SCREEN_WIDTH * SCREEN_HEIGHT;
              float scaleFactor = area / 480000.0f;
              scaleFactor = std::max(0.5f, std::min(2.0f, scaleFactor));

              int spawnCount = (int)(
                  (COMET_SPAWN_BATCH_MIN + (int)(RandomRange(0.0f, 1.0f) * (COMET_SPAWN_BATCH_MAX - COMET_SPAWN_BATCH_MIN +
   1))) *
                  scaleFactor
              );

              for (int i = 0; i < spawnCount; i++) {
                  SpawnComet();
              }

              cometSpawnTimer = 0;
          }
      }
  }

  // Main draw function
  void Draw() {
      // Draw background elements
      DrawStarfield();
      DrawCronos();

      // Menu state
      if (gameState == MENU) {
          DrawText("Splitting Comets", SCREEN_WIDTH/2 - MeasureText("Splitting Comets", 48)/2, SCREEN_HEIGHT/2 - 50, 48,
  WHITE);
          DrawText("Press ENTER to start", SCREEN_WIDTH/2 - MeasureText("Press ENTER to start", 24)/2, SCREEN_HEIGHT/2, 24,
   WHITE);
          DrawText("Press S for settings", SCREEN_WIDTH/2 - MeasureText("Press S for settings", 24)/2, SCREEN_HEIGHT/2 +
  40, 24, WHITE);
          DrawText("Press M to toggle sound", SCREEN_WIDTH/2 - MeasureText("Press M to toggle sound", 24)/2,
  SCREEN_HEIGHT/2 + 80, 24, WHITE);

          char scoreText[20];
          sprintf(scoreText, "Score: %d", score);
          DrawText(scoreText, SCREEN_WIDTH/2 - MeasureText(scoreText, 24)/2, 40, 24, WHITE);

          const char* soundStatus = soundEnabled ? "ON" : "OFF";
          char soundText[20];
          sprintf(soundText, "Sound: %s", soundStatus);
          DrawText(soundText, SCREEN_WIDTH/2 - MeasureText(soundText, 18)/2, SCREEN_HEIGHT/2 + 120, 18, WHITE);

          DrawMusicIcon();
          return;
      }

      // Settings state
      if (gameState == SETTINGS) {
          DrawText("Settings", SCREEN_WIDTH/2 - MeasureText("Settings", 48)/2, SCREEN_HEIGHT/2 - 50, 48, WHITE);
          DrawText("Press ESC to return", SCREEN_WIDTH/2 - MeasureText("Press ESC to return", 24)/2, SCREEN_HEIGHT/2, 24,
  WHITE);

          const char* soundStatus = soundEnabled ? "ON" : "OFF";
          char soundText[50];
          sprintf(soundText, "Press M to toggle all sounds: %s", soundStatus);
          DrawText(soundText, SCREEN_WIDTH/2 - MeasureText(soundText, 24)/2, SCREEN_HEIGHT/2 + 40, 24, WHITE);

          DrawMusicIcon();
          return;
      }

      // Win state
      if (gameState == WIN) {
          // Draw explosion particles
          for (const auto& p : explosionParticles) {
              DrawCircle(p.position.x, p.position.y, p.size, p.color);
          }

          DrawText("WIN!", SCREEN_WIDTH/2 - MeasureText("WIN!", 48)/2, SCREEN_HEIGHT/2, 48, GREEN);

          char scoreText[20];
          sprintf(scoreText, "Score: %d", score);
          DrawText(scoreText, SCREEN_WIDTH/2 - MeasureText(scoreText, 24)/2, 40, 24, WHITE);
          return;
      }

      // Game over state
      if (gameState == GAMEOVER) {
          // Draw explosion particles
          for (const auto& p : explosionParticles) {
              DrawCircle(p.position.x, p.position.y, p.size, p.color);
          }

          DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 48)/2, SCREEN_HEIGHT/2, 48, RED);
          DrawText("Press ENTER to restart", SCREEN_WIDTH/2 - MeasureText("Press ENTER to restart", 24)/2, SCREEN_HEIGHT/2
  + 40, 24, WHITE);

          char scoreText[20];
          sprintf(scoreText, "Score: %d", score);
          DrawText(scoreText, SCREEN_WIDTH/2 - MeasureText(scoreText, 24)/2, 40, 24, WHITE);
          return;
      }

      // Playing state
      char scoreText[20];
      sprintf(scoreText, "Score: %d", score);
      DrawText(scoreText, SCREEN_WIDTH/2 - MeasureText(scoreText, 24)/2, 40, 24, WHITE);
      DrawHealthBar();
      DrawMusicIcon();

      // Draw power-ups
      for (const auto& pu : powerUps) {
          Rectangle rect = {
              pu.position.x - POWERUP_BOX_SIZE/2,
              pu.position.y - POWERUP_BOX_SIZE/2,
              POWERUP_BOX_SIZE,
              POWERUP_BOX_SIZE
          };

          // Draw power-up box with rounded corners
          DrawRectangleRounded(rect, 0.2f, 8, MAROON);
          DrawRectangleRoundedLines(rect, 0.2f, 8, 4.0f, WHITE);

          // Draw top triangle indicator
          Vector2 triPoints[3] = {
              {pu.position.x - POWERUP_BOX_SIZE/2, pu.position.y - POWERUP_BOX_SIZE/2},
              {pu.position.x, pu.position.y - POWERUP_BOX_SIZE},
              {pu.position.x + POWERUP_BOX_SIZE/2, pu.position.y - POWERUP_BOX_SIZE/2}
          };
          DrawTriangle(triPoints[0], triPoints[1], triPoints[2], WHITE);

          // Draw "PuP" text
          DrawText("PuP", pu.position.x - MeasureText("PuP", 14)/2, pu.position.y - 7, 14, BLACK);
      }

      // Draw spaceship
      DrawSpaceship();

      // Draw sparks
      for (const auto& spark : sparks) {
          const float noseX = ship.position.x + cos(ship.angle) * ship.size;
          const float noseY = ship.position.y + sin(ship.angle) * ship.size;

          DrawLine(
              noseX, noseY,
              noseX + cos(spark.angle) * spark.length,
              noseY + sin(spark.angle) * spark.length,
              YELLOW
          );
      }

      // Draw bullets
      for (const auto& b : bullets) {
          DrawCircle(b.position.x, b.position.y, b.radius, WHITE);
      }

      // Draw comets
      DrawNormalComets();
      DrawBigComets();

      // Draw explosion particles
      for (const auto& p : explosionParticles) {
          DrawCircle(p.position.x, p.position.y, p.size, p.color);
      }
  }