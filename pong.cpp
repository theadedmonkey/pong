#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;

const int SCREEN_HEIGHT = 768;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

const int FPS = 120;
const int DELAY_TIME = 1000.0f / FPS;

// window and renderer
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// game object constants
const int PADDLE_WIDTH = 32;
const int PADDLE_WIDTH_HALF = PADDLE_WIDTH / 2;
const int PADDLE_HEIGHT = 128;
const int PADDLE_HEIGHT_HALF = PADDLE_HEIGHT / 2;
const int PLAYER_PADDLE_SPEED = 5;
const int ENEMY_PADDLE_SPEED = 5;

const int BALL_WIDTH = 32;
const int BALL_WIDTH_HALF = BALL_WIDTH / 2;
const int BALL_HEIGHT = 32;
const int BALL_HEIGHT_HALF = BALL_HEIGHT / 2;
int ballSpeedX;
int ballSpeedY;

// game textures
SDL_Texture* courtTexture = nullptr;
SDL_Texture* playerPaddleTexture = nullptr;
SDL_Texture* enemyPaddleTexture = nullptr;
SDL_Texture* ballTexture = nullptr;

// game rects
SDL_Rect courtRect;
SDL_Rect playerPaddleRect;
SDL_Rect enemyPaddleRect;
SDL_Rect ballRect;

bool initSDL();
SDL_Texture* loadTexture(const std::string& path);
bool loadMedia();
bool rectsIntersects(const SDL_Rect &a, const SDL_Rect &b);
bool initGame();
void drawGame();
void updateGame();
void handlePaddleCollisionWithScreen(SDL_Rect &paddleRect);
void updatePlayerPaddle();
void updateEnemyPaddle();
void bounceBall(int paddleY);
void updateBall();
void resetGame();

bool initSDL() {
	if (SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
	  std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

  window = SDL_CreateWindow(
		"Pong",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
  if(window == NULL) {
    std::cout << "Failed to create window with error: " << SDL_GetError() << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL) {
    std::cout << "Failed to create renderer with error: " << SDL_GetError() << std::endl;
    return false;
  }

	return true;
}

SDL_Texture* loadTexture(const std::string &path) {
  SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
  if (texture == nullptr) {
    std::cout << "Failed to load texture " << path << " error : " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return texture;
}

bool loadMedia() {
  courtTexture = loadTexture("assets/court.png");
  if (!courtTexture) {
    return false;
  }

  playerPaddleTexture = loadTexture("assets/paddle-blue.png");
  if (!playerPaddleTexture) {
    return false;
  }

  enemyPaddleTexture = loadTexture("assets/paddle-green.png");
  if (!enemyPaddleTexture) {
    return false;
  }

  ballTexture = loadTexture("assets/ball.png");
  if (!ballTexture) {
    return false;
  }

  return true;
}

bool rectsIntersects(const SDL_Rect &a, const SDL_Rect &b) {
  if(a.x >= b.x + b.w) {
    return false;
  }

  if(b.x >= a.x + a.w) {
    return false;
  }

  if(a.y >= b.y + b.h) {
    return false;
  }

  if(b.y >= a.y + a.h) {
    return false;
  }

  return true;
}

bool initGame() {
  if(!initSDL()) {
    return false;
  }

  if(!loadMedia()) {
    return false;
  }

  resetGame();

  return true;
}

void drawGame() {
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, courtTexture, nullptr, &courtRect);
  SDL_RenderCopy(renderer, playerPaddleTexture, nullptr, &playerPaddleRect);
  SDL_RenderCopy(renderer, enemyPaddleTexture, nullptr, &enemyPaddleRect);
  SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);
  SDL_RenderPresent(renderer);
}

void updateGame() {
  updatePlayerPaddle();
  updateEnemyPaddle();
  updateBall();
}

void handlePaddleCollisionWithScreen(SDL_Rect &paddleRect) {
  if(paddleRect.y < 0 - PADDLE_HEIGHT_HALF) {
    paddleRect.y = 0 - PADDLE_HEIGHT_HALF;
  }

  if(paddleRect.y > SCREEN_HEIGHT - PADDLE_HEIGHT_HALF) {
    paddleRect.y = SCREEN_HEIGHT - PADDLE_HEIGHT_HALF;
  }
}

void updatePlayerPaddle() {
  Uint8 *keys = (Uint8*)SDL_GetKeyboardState(NULL);

  if(keys[SDL_SCANCODE_UP]) {
    playerPaddleRect.y -= PLAYER_PADDLE_SPEED;
  }

  if(keys[SDL_SCANCODE_DOWN]) {
    playerPaddleRect.y += PLAYER_PADDLE_SPEED;
  }

  handlePaddleCollisionWithScreen(playerPaddleRect);
}

void updateEnemyPaddle() {
  // enemy paddle ai
  if(enemyPaddleRect.y + PADDLE_HEIGHT_HALF > ballRect.y + BALL_HEIGHT_HALF) {
    enemyPaddleRect.y -= ENEMY_PADDLE_SPEED;
  }

  if(enemyPaddleRect.y + PADDLE_HEIGHT_HALF < ballRect.y + BALL_HEIGHT_HALF) {
    enemyPaddleRect.y += ENEMY_PADDLE_SPEED;
  }

  handlePaddleCollisionWithScreen(enemyPaddleRect);
}

void bounceBall(int paddleY) {
	ballSpeedX *= -1;

	const int ballYRelativeToPaddle = ballRect.y - paddleY;
	if(
	  ballYRelativeToPaddle >= -32 &&
		ballYRelativeToPaddle < 28
	) { ballSpeedY = -4; }

	if(
		ballYRelativeToPaddle >= 28 &&
		ballYRelativeToPaddle <= 68
	) { ballSpeedY = 0; }

	if(
		ballYRelativeToPaddle > 68 &&
		ballYRelativeToPaddle <= 128
	) { ballSpeedY = 4; }
}

void updateBall() {
  ballRect.x += ballSpeedX;
  ballRect.y += ballSpeedY;

  if(rectsIntersects(playerPaddleRect, ballRect)) {
    ballRect.x = playerPaddleRect.x + PADDLE_WIDTH;
		bounceBall(playerPaddleRect.y);
  }

  if(rectsIntersects(enemyPaddleRect, ballRect)) {
    ballRect.x = enemyPaddleRect.x - BALL_WIDTH;
		bounceBall(enemyPaddleRect.y);
  }

  // check top and bottom court collisions
  if (ballRect.y < 0) {
    ballRect.y = 0;
    ballSpeedY *= -1;
  }

  if (ballRect.y > SCREEN_HEIGHT - BALL_HEIGHT) {
    ballRect.y = SCREEN_HEIGHT - BALL_HEIGHT;
    ballSpeedY *= -1;
  }

  // check left and right court collisions
  if (ballRect.x < 0 - BALL_WIDTH) {
    SDL_Delay(500);
    resetGame();
  }

  if(ballRect.x > SCREEN_WIDTH) {
    SDL_Delay(500);
    resetGame();
  }
}

void resetGame() {
  courtRect = {
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
  };

  playerPaddleRect = {
    20, SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF, PADDLE_WIDTH, PADDLE_HEIGHT
  };

  enemyPaddleRect = {
    SCREEN_WIDTH - PADDLE_WIDTH - 20, SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF, PADDLE_WIDTH, PADDLE_HEIGHT
  };

  ballRect = {
    SCREEN_WIDTH_HALF - BALL_WIDTH_HALF, 0, BALL_WIDTH, BALL_HEIGHT
  };

	ballSpeedX = 4;
	ballSpeedY = 4;
}

int main( int argc, char* args[] ) {
  if(!initGame()) {
    return 0;
  }

	SDL_Event e;
  bool isRunning = true;

  while(isRunning) {
    long int oldTime = SDL_GetTicks();

    while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				isRunning = false;
			}
    }
    updateGame();
    drawGame();

    int frameTime = SDL_GetTicks() - oldTime;
    if(frameTime < DELAY_TIME) {
      SDL_Delay(DELAY_TIME - frameTime);
    }
  }

}
