#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "lib/Vector2D.h"

std::vector<std::string> menu = {"play", "options"};

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;

const int SCREEN_HEIGHT = 600;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

const int FPS = 120;
const int DELAY_TIME = 1000.0f / FPS;

SDL_Texture* LoadTextureBMP(SDL_Renderer* &renderer, const std::string &str) {
  // Load image as SDL_Surface
  SDL_Surface* surface = SDL_LoadBMP( str.c_str() );
  if (surface == nullptr) {
    std::cout << "Failed to load surface " << str << " error : " << SDL_GetError() << std::endl;
    return nullptr;
  }
  // SDL_Surface is just the raw pixels
  // Convert it to a hardware-optimzed texture so we can render it
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  // Don't need the orignal texture, release the memory
  SDL_FreeSurface(surface);
  return texture;
}

SDL_Texture* loadTexture(SDL_Renderer* &renderer, const std::string &str) {
  // Load image as SDL_Surface
  SDL_Texture* texture = IMG_LoadTexture(renderer, str.c_str());
  if (texture == nullptr) {
    std::cout << "Failed to load texture " << str << " error : " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return texture;
}

/*
 * title - the window caption/title that appears on the top of the window
 * posX  - x position of the window
 * posY  - y position of the window
 * sizeW - width of the window
 * sizeH - height of the window
 * flags - specifies window state or properties
 *         see: https://wiki.libsdl.org/SDL_WindowFlags
 */
SDL_Window* createWindow() {
	return SDL_CreateWindow(
		"SDL Tutorial",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
};
/*
* window - the SDL_Window this renderer will be attached to. In our case, this
*          is the SDL_Window we just created.
* index  - specifies which rendering driver to use. It sounds complicated, but
*          we can just specify -1. This means we’ll use the first driver we can find.
* flags  - species how the rendering should be done. For now, we’ll just use
*          SDL_RENDERER_ACCELERATED which lets us use the graphics card to render quickly.
*          see: https://wiki.libsdl.org/SDL_RendererFlags?highlight=%28\bCategoryEnum\b%29|%28SDLEnumTemplate%29
*/
SDL_Renderer* createRenderer(SDL_Window* &window) {
	return SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);
};

bool init() {
	bool initialized = true;
	if (SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
	  // std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		initialized = false;
	}
	return initialized;
}

//Frees media and shuts down SDL
void close(SDL_Window* &window, SDL_Renderer* &renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Texture* getTextTexture(std::string text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
  int textLength = text.length();
   SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
   if (textSurface == NULL) {
     printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
   }
   else {
     SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
     if (textTexture == NULL) {
       printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
     }
     SDL_FreeSurface(textSurface);
     return textTexture;
   }
}

int main( int argc, char* args[] ) {

  //Start up SDL and create window
  if(!init()) {
    printf( "Failed to initialize!\n" );
		return 0;
  }
  if (TTF_Init() == -1){
    std::cout << "Init TTF  failt: " << SDL_GetError() << std::endl;
    return -1;
  }

  // The window we'll be rendering to
	SDL_Window* window = createWindow();

	// The window renderer
	SDL_Renderer* renderer = createRenderer(window);

	// Main loop flag
	bool quit = false;

  // Scenes
  bool sceneHall = true;
  bool scenePlay = false;

	//Event handler
	SDL_Event e;

  Uint32 frameStart, frameTime;

  // scores
  int player1Score = 0;
  int player2Score = 0;

  /**************************************
   * Score font setup
   **************************************/
  TTF_Font* scoreTextFont;
  SDL_Color scoreTextColor = { 255, 255, 255, 255 };

  SDL_Surface* player1ScoreTextSurface;
  SDL_Texture* player1ScoreTextTexture;
  SDL_Rect player1ScoreTextDstRect;

  SDL_Surface* player2ScoreTextSurface;
  SDL_Texture* player2ScoreTextTexture;
  SDL_Rect player2ScoreTextDstRect;

  // Load our fonts, with a huge size
  scoreTextFont = TTF_OpenFont("./assets/Roboto-Black.ttf", 90);
  if (scoreTextFont == nullptr) {
    std::cout << "Failed to load score text font: " << SDL_GetError() << std::endl;
    return -1;
  }

  /**************************************
   * Menu font setup
   **************************************/
   TTF_Font* menuTextFont;
   SDL_Color menuTextColor = { 255, 255, 255, 255 };
   int menuTextSize = 24;

   // Load our fonts, with a huge size
   menuTextFont = TTF_OpenFont("./assets/Roboto-Black.ttf", menuTextSize);
   if (menuTextFont == nullptr) {
     std::cout << "Failed to load menu text font: " << SDL_GetError() << std::endl;
     return -1;
   }

   std::string menuOption1 = "play";
   int menuOption1RectW = menuOption1.length() * menuTextSize;
   SDL_Rect menuOption1Rect = {
     SCREEN_WIDTH_HALF - menuOption1RectW / 2,
     20,
     menuOption1RectW,
     100
   };
   SDL_Texture* menuOption1Texture = getTextTexture(menuOption1, menuTextFont, menuTextColor, renderer);

   std::string menuOption2 = "options";
   int menuOption2RectW = menuOption2.length() * menuTextSize;
   SDL_Rect menuOption2Rect = {
     SCREEN_WIDTH_HALF - menuOption2RectW / 2,
     120,
     menuOption2RectW,
     100
   };
   SDL_Texture* menuOption2Texture = getTextTexture(menuOption2, menuTextFont, menuTextColor, renderer);

  /**************************************
   * Game objects
   **************************************/
  SDL_Texture* hallTexture = loadTexture(renderer, "./assets/hall.jpg");
  SDL_Texture* courtTexture = loadTexture(renderer, "./assets/court.png");
  SDL_Texture* paddle1Texture = loadTexture(renderer, "./assets/paddle-green.png");
  SDL_Texture* paddle2Texture = loadTexture(renderer, "./assets/paddle-blue.png");
  SDL_Texture* ballTexture = loadTexture(renderer, "./assets/ball.png");

  SDL_Rect hallSrcRect;
  SDL_Rect hallDstRect;

  SDL_Rect courtSrcRect;
  SDL_Rect courtDstRect;

  SDL_Rect paddle1SrcRect;
  SDL_Rect paddle1DstRect;

  SDL_Rect paddle2SrcRect;
  SDL_Rect paddle2DstRect;

  SDL_Rect ballSrcRect;
  SDL_Rect ballDstRect;

  SDL_Rect ballInvRect;

  const int PADDLE_WIDTH = 32;
  const int PADDLE_WIDTH_HALF = PADDLE_WIDTH / 2;

  const int PADDLE_HEIGHT = 128;
  const int PADDLE_HEIGHT_HALF = PADDLE_HEIGHT / 2;

  const int BALL_WIDTH = 32;
  const int BALL_WIDTH_HALF = BALL_WIDTH / 2;

  const int BALL_HEIGHT = 32;
  const int BALL_HEIGHT_HALF = BALL_HEIGHT / 2;

  // paddle 1
  Vector2D paddle1Pos = Vector2D(20, SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF);
  Vector2D paddle1Vel = Vector2D(0, 2);

  // paddle 2
  Vector2D paddle2Pos = Vector2D(SCREEN_WIDTH - 20 - PADDLE_WIDTH, SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF);
  Vector2D paddle2Vel = Vector2D(0, 1.5);

  // ball
  Vector2D ballPos = Vector2D(SCREEN_WIDTH_HALF - BALL_WIDTH_HALF, 0);
  Vector2D ballVel = Vector2D(2.5, 2.5);

  int ballSpeedLow = 3.0;
  int ballSpeedHight = 4.5;

  // ball invisible
  Vector2D ballInvPos = Vector2D();
  Vector2D ballInvVel = Vector2D();

  float ballInvPointY = 0.0f;

  bool paddle1HasCollide = false;
  bool paddle2HasCollide = false;

  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	//While application is running
	while(!quit) {

    frameStart = SDL_GetTicks();

		while( SDL_PollEvent( &e ) != 0 ) {
			// User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}
      // User presses a key
      else if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
          case SDLK_SPACE:
          if (sceneHall) {
            sceneHall = false;
            scenePlay = true;
          }
          break;
          case SDLK_ESCAPE:
          if (scenePlay) {
            sceneHall = true;
            scenePlay = false;
          }
          break;
        }
      }
		}

    if (sceneHall) {
      // Select the color for drawing. It is set to black here.
  	  SDL_SetRenderDrawColor(renderer, 000, 000, 000, 255);
      // Clear screen
  		SDL_RenderClear(renderer);

      // hall
      hallSrcRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
      hallDstRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
      SDL_RenderCopy(renderer, hallTexture, &hallSrcRect, &hallDstRect);

      // menu
      SDL_RenderCopy(renderer, menuOption1Texture, nullptr, &menuOption1Rect);
      SDL_RenderCopy(renderer, menuOption2Texture, nullptr, &menuOption2Rect);

      SDL_RenderPresent(renderer);
    }
    // play scene
    else {

    if (keyboardState[SDL_SCANCODE_UP]) {
      paddle1Pos -= paddle1Vel;
    }
    if (keyboardState[SDL_SCANCODE_DOWN]) {
      paddle1Pos += paddle1Vel;
    }

    // ball intersects paddle1

    if (ballPos.x >= paddle1Pos.x &&
        ballPos.x <= paddle1Pos.x + PADDLE_WIDTH &&
        ballPos.y + BALL_HEIGHT >= paddle1Pos.y &&
        ballPos.y <= paddle1Pos.y + PADDLE_HEIGHT) {

      ballPos.x = paddle1Pos.x + PADDLE_WIDTH;

      // ball intersects paddle1 in top
      if (ballPos.y >= paddle1Pos.y && ballPos.y <= paddle1Pos.y + 42) {
        ballVel.x = ballSpeedHight * 1;
        ballVel.y = ballSpeedHight * -1;
      }
      // ball intersects paddle1 in middle
      else if (ballPos.y  > paddle1Pos.y + 42 && ballPos.y < paddle1Pos.y + 42 + 44) {
        ballVel.x = ballSpeedLow * 1;
        ballVel.y = 0;
      }
      // ball intersects paddle1 in bottom
      else if (ballPos.y >= paddle1Pos.y + 42 + 44 && ballPos.y <= paddle1Pos.y + PADDLE_HEIGHT) {
        ballVel.x = ballSpeedHight * 1;
        ballVel.y = ballSpeedHight * 1;
      }

      ballInvPos = ballPos;
      ballInvVel = ballVel * 2.5;

      paddle1HasCollide = true;
      paddle2HasCollide = false;
    }

    // ball intersects paddle2
    if (ballPos.x + BALL_WIDTH >= paddle2Pos.x &&
        ballPos.x + BALL_WIDTH <= paddle2Pos.x + PADDLE_WIDTH &&
        ballPos.y + BALL_HEIGHT >= paddle2Pos.y &&
        ballPos.y <= paddle2Pos.y + PADDLE_HEIGHT) {

      // ball intersects paddle2 in top
      if (ballPos.y + BALL_HEIGHT >= paddle2Pos.y && ballPos.y + BALL_HEIGHT <= paddle2Pos.y + 42) {
        ballVel.x = ballSpeedHight * -1;
        ballVel.y = ballSpeedHight * -1;
      }
      // ball intersects paddle2 in middle
      else if (ballPos.y + BALL_HEIGHT >= paddle2Pos.y + 42 && ballPos.y + BALL_HEIGHT <= paddle2Pos.y + 42 + 44) {
        ballVel.x = ballSpeedLow * -1;
        ballVel.y = 0;
      }
      // ball intersects paddle2 in bottom
      else if (ballPos.y <= paddle2Pos.y + PADDLE_HEIGHT && ballPos.y >= paddle2Pos.y + PADDLE_HEIGHT - 42) {
        ballVel.x = ballSpeedHight * -1;
        ballVel.y = ballSpeedHight * 1;
      }

      ballPos.x = paddle2Pos.x - PADDLE_WIDTH;

      ballInvPos = ballPos;
      ballInvVel = ballVel * 2.5;

      paddle1HasCollide = false;
      paddle2HasCollide = true;
    }

    // ball intersects court top
    if (ballPos.y >= SCREEN_HEIGHT - BALL_HEIGHT) {
      ballVel.y *= -1;
    }
    // ball intersects court bottom
    if (ballPos.y < 0) {
      ballVel.y *= -1;
    }

    // ball invisible intersects court top
    if (ballInvPos.y >= SCREEN_HEIGHT - BALL_HEIGHT) {
      ballInvVel.y *= -1;
    }
    // ball invisible intersects court bottom
    if (ballInvPos.y < 0) {
      ballInvVel.y *= -1;
    }

    // ball invisible intersects paddle2 y
    if (ballInvPos.x > SCREEN_WIDTH - 20 - PADDLE_WIDTH - BALL_WIDTH) {
      ballInvPointY = ballInvPos.y;
      ballInvPos.x = 0.0f;
      ballInvPos.y = 0.0f;
      ballInvVel.x = 0.0f;
      ballInvVel.y = 0.0f;
    }


    // ball overpass court right
    if (ballPos.x >= SCREEN_WIDTH) {
      player1Score += 1;

      ballPos.x = SCREEN_WIDTH_HALF - BALL_WIDTH_HALF;
      ballPos.y = 0;

      ballVel.x = 2.5;
      ballVel.y = 2.5;

      paddle1Pos.x = 20;
    	paddle1Pos.y = SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF;

      paddle2Pos.x = SCREEN_WIDTH - 20 - PADDLE_WIDTH;
      paddle2Pos.y = SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF;

      paddle1HasCollide = false;
      paddle2HasCollide = false;
      SDL_Delay(1000);
    }

    // ball overpass court left
    if (ballPos.x <= 0 - BALL_WIDTH) {
      player2Score += 1;

      ballPos.x = SCREEN_WIDTH_HALF - BALL_WIDTH_HALF;
      ballPos.y = 0;

      ballVel.x = 2.5;
      ballVel.y = 2.5;

      paddle1Pos.x = 20;
    	paddle1Pos.y = SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF;

      paddle2Pos.x = SCREEN_WIDTH - 20 - PADDLE_WIDTH;
      paddle2Pos.y = SCREEN_HEIGHT_HALF - PADDLE_HEIGHT_HALF;

      paddle1HasCollide = false;
      paddle2HasCollide = false;
      SDL_Delay(1000);
    }

    ballPos += ballVel;
    ballInvPos += ballInvVel;

    if (paddle1HasCollide) {
      // paddle2Pos.y = ballPos.y;
      if (ballInvPointY > 0.0f) {
        /*
         * the paddle should always try to collide with the ball in the middle
         * of the paddle.
         *
         * the ball invisible y determines the point where the invisible
         * ball pass the x paddle boundary so the paddle should move towards
         * that point plus half of its height minus half of the height of the
         * invisible ball, in order to try to collide with the normal ball in
         * the middle of the paddle.
         */
        paddle2Pos.y += ballInvPointY > paddle2Pos.y + PADDLE_HEIGHT_HALF - BALL_HEIGHT ? paddle2Vel.y : paddle2Vel.y * -1;
      }
      else {
        paddle2Pos.y += ballInvVel.y > 0 ? paddle2Vel.y : paddle2Vel.y * -1;
      }
    }

    if (paddle2HasCollide) {
      paddle2Pos.y += ballVel.y > 0 ? paddle2Vel.y : paddle2Vel.y * -1;
    }

    // paddle1 intersects court top
    if (paddle1Pos.y < -PADDLE_HEIGHT_HALF) {
      paddle1Pos.y = -PADDLE_HEIGHT_HALF;
    }
    // paddle1 intersects court bottom
    if (paddle1Pos.y >= SCREEN_HEIGHT - PADDLE_HEIGHT_HALF) {
      paddle1Pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT_HALF;
    }
    // paddle2 intersects court top
    if (paddle2Pos.y < 0 -PADDLE_HEIGHT_HALF) {
      paddle2Pos.y = -PADDLE_HEIGHT_HALF;
    }
    // paddle2 intersects court bottom
    if (paddle2Pos.y >= SCREEN_HEIGHT - PADDLE_HEIGHT_HALF) {
      paddle2Pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT_HALF;
    }

    /**************************************
     * Render
     **************************************/
		// Select the color for drawing. It is set to black here.
	  SDL_SetRenderDrawColor(renderer, 000, 000, 000, 255);
    // Clear screen
		SDL_RenderClear(renderer);

    // ball invisible
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect ballInvRect = {(int)ballInvPos.x, (int)ballInvPos.y, 32, 32};
    SDL_RenderFillRect(renderer, &ballInvRect);

    // court
    courtSrcRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    courtDstRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, courtTexture, &courtSrcRect, &courtDstRect);

    // paddle 1
		paddle1SrcRect = { 0, 0, PADDLE_WIDTH, PADDLE_HEIGHT };
		paddle1DstRect = { (int)paddle1Pos.x, (int)paddle1Pos.y, 32, 128 };
		SDL_RenderCopy(renderer, paddle1Texture, &paddle1SrcRect, &paddle1DstRect);

    // paddle 2
    paddle2SrcRect = { 0, 0, PADDLE_WIDTH, PADDLE_HEIGHT };
    paddle2DstRect = { (int)paddle2Pos.x, (int)paddle2Pos.y, 32, 128 };
    SDL_RenderCopy(renderer, paddle2Texture, &paddle2SrcRect, &paddle2DstRect);

    // player 1 score text
    player1ScoreTextSurface = TTF_RenderText_Solid(scoreTextFont, std::to_string(player1Score).c_str(), scoreTextColor);
    player1ScoreTextTexture = SDL_CreateTextureFromSurface(renderer, player1ScoreTextSurface);
    SDL_FreeSurface(player1ScoreTextSurface);
    player1ScoreTextDstRect = { SCREEN_WIDTH_HALF - 30 - 20, 20 , 30, 60 };
    SDL_RenderCopy(renderer, player1ScoreTextTexture, nullptr, &player1ScoreTextDstRect);

    // player 2 score text
    player2ScoreTextSurface = TTF_RenderText_Solid(scoreTextFont, std::to_string(player2Score).c_str(), scoreTextColor);
    player2ScoreTextTexture = SDL_CreateTextureFromSurface(renderer, player2ScoreTextSurface);
    SDL_FreeSurface(player2ScoreTextSurface);
    player2ScoreTextDstRect = { SCREEN_WIDTH_HALF + 20, 20 , 30, 60 };
    SDL_RenderCopy(renderer, player2ScoreTextTexture, nullptr, &player2ScoreTextDstRect);

    // ball
    ballSrcRect = { 0, 0, BALL_WIDTH, BALL_HEIGHT };
    ballDstRect = { (int)ballPos.x, (int)ballPos.y, 32, 32 };
    SDL_RenderCopy(renderer, ballTexture, &ballSrcRect, &ballDstRect);

    SDL_RenderPresent(renderer);

    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < DELAY_TIME) {
      SDL_Delay((int) (DELAY_TIME - frameTime));
    }
    } // end of play scene
  }
  //Free resources and close SDL
  close(window, renderer);

  return 0;
}
