#include <stdio.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


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

  //The window we'll be rendering to
	SDL_Window* window = createWindow();

	//The window renderer
	SDL_Renderer* renderer = createRenderer(window);

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

  // scores
  int player1Score = 0;
  int player2Score = 0;

  /**************************************
   * Text font setup
   **************************************/
  TTF_Font* textFont;
  SDL_Color textColor = { 255, 255, 255, 255 };


  SDL_Surface* player1ScoreSurface;
  SDL_Texture* player1ScoreTexture;
  SDL_Rect player1ScoreDstRect;

  SDL_Surface* player2ScoreSurface;
  SDL_Texture* player2ScoreTexture;
  SDL_Rect player2ScoreDstRect;

  // Load our fonts, with a huge size
  textFont = TTF_OpenFont("./assets/Roboto-Black.ttf", 90);
  if (textFont == nullptr) {
    std::cout << "Failed to load text font: " << SDL_GetError() << std::endl;
    return -1;
  }

  /**************************************
   * Game objects
   **************************************/
  SDL_Texture* courtTexture = loadTexture(renderer, "./assets/court.png");
  SDL_Texture* paddle1Texture = loadTexture(renderer, "./assets/paddle-green.png");
  SDL_Texture* paddle2Texture = loadTexture(renderer, "./assets/paddle-blue.png");
  SDL_Texture* ballTexture = loadTexture(renderer, "./assets/ball.png");

  SDL_Rect courtSrcRect;
  SDL_Rect courtDstRect;

  SDL_Rect paddle1SrcRect;
  SDL_Rect paddle1DstRect;

  SDL_Rect paddle2SrcRect;
  SDL_Rect paddle2DstRect;

  SDL_Rect ballSrcRect;
  SDL_Rect ballDstRect;

  int paddle1X = 20;
	int paddle1Y = SCREEN_HEIGHT / 2 - 128 / 2;
  int paddle1SpeedY = 25;

  int paddle2X = SCREEN_WIDTH - 20 - 38;
  int paddle2Y = SCREEN_HEIGHT / 2 - 128 / 2;
  int paddle2SpeedY = 25;

  int ballX = SCREEN_WIDTH / 2 - 32 / 2;
  int ballY = 0;
  int ballSpeedX = 5;
  int ballSpeedY = 5;

	//While application is running
	while(!quit) {
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}
			else if(e.type == SDL_KEYDOWN) {
				switch( e.key.keysym.sym ) {
					case SDLK_w:
					paddle1Y -= paddle1SpeedY;
					break;
					case SDLK_s:
					paddle1Y += paddle1SpeedY;
					break;
          case SDLK_UP:
          paddle2Y -= paddle2SpeedY;
          break;
          case SDLK_DOWN:
          paddle2Y += paddle2SpeedY;
          break;
				}
			}
		}


    // paddle1 intersects ball
    if (SDL_HasIntersection(&ballDstRect, &paddle1DstRect)) {
      ballSpeedX *= -1;
    }
    // paddle2 intersects ball
    if (SDL_HasIntersection(&ballDstRect, &paddle2DstRect)) {
      ballSpeedX *= -1;
    }
    // ball intersects court top
    if (ballY >= SCREEN_HEIGHT - 32) {
      ballSpeedY *= -1;
    }
    // ball intersects court bottom
    if (ballY < 0) {
      ballSpeedY *= -1;
    }

    // ball overpass court right
    if (ballX >= SCREEN_WIDTH) {
      player1Score += 1;
      ballX = SCREEN_WIDTH / 2 - 32 / 2;
      ballY = 0;

      paddle1X = 20;
    	paddle1Y = SCREEN_HEIGHT / 2 - 128 / 2;

      paddle2X = SCREEN_WIDTH - 20 - 38;
      paddle2Y = SCREEN_HEIGHT / 2 - 128 / 2;
      SDL_Delay(1000);
    }

    // ball overpass court left
    if (ballX <= 0 - 32) {
      player2Score += 1;

      ballX = SCREEN_WIDTH / 2 - 32 / 2;
      ballY = 0;

      paddle1X = 20;
    	paddle1Y = SCREEN_HEIGHT / 2 - 128 / 2;

      paddle2X = SCREEN_WIDTH - 20 - 38;
      paddle2Y = SCREEN_HEIGHT / 2 - 128 / 2;

      SDL_Delay(1000);
    }

    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // paddle1 intersects court top
    if (paddle1Y < 0) {
      paddle1Y = 0;
    }
    // paddle1 intersects court bottom
    if (paddle1Y >= SCREEN_HEIGHT - 128) {
      paddle1Y = SCREEN_HEIGHT - 128;
    }
    // paddle2 intersects court top
    if (paddle2Y < 0) {
      paddle2Y = 0;
    }
    // paddle2 intersects court bottom
    if (paddle2Y >= SCREEN_HEIGHT - 128) {
      paddle2Y = SCREEN_HEIGHT - 128;
    }

    /**************************************
     * Render
     **************************************/
		// Select the color for drawing. It is set to black here.
	  SDL_SetRenderDrawColor(renderer, 000, 000, 000, 255);
    // Clear screen
		SDL_RenderClear(renderer);

    courtSrcRect = { 0, 0, 800, 600 };
    courtDstRect = { 0, 0, 800, 600 };
    SDL_RenderCopy(renderer, courtTexture, &courtSrcRect, &courtDstRect);

		paddle1SrcRect = { 0, 0, 32, 128 };
		paddle1DstRect = { paddle1X, paddle1Y, 32, 128 };
		SDL_RenderCopy(renderer, paddle1Texture, &paddle1SrcRect, &paddle1DstRect);

    paddle2SrcRect = { 0, 0, 32, 128 };
    paddle2DstRect = { paddle2X, paddle2Y, 32, 128 };
    SDL_RenderCopy(renderer, paddle2Texture, &paddle2SrcRect, &paddle2DstRect);


    player1ScoreSurface = TTF_RenderText_Solid(textFont, std::to_string(player1Score).c_str(), textColor);
    player1ScoreTexture = SDL_CreateTextureFromSurface(renderer, player1ScoreSurface);
    SDL_FreeSurface(player1ScoreSurface);
    player1ScoreDstRect = { SCREEN_WIDTH / 2 - 30 - 20, 20 , 30, 60 };
    SDL_RenderCopy(renderer, player1ScoreTexture, nullptr, &player1ScoreDstRect);

    player2ScoreSurface = TTF_RenderText_Solid(textFont, std::to_string(player2Score).c_str(), textColor);
    player2ScoreTexture = SDL_CreateTextureFromSurface(renderer, player2ScoreSurface);
    SDL_FreeSurface(player2ScoreSurface);
    player2ScoreDstRect = { SCREEN_WIDTH / 2 + 20, 20 , 30, 60 };
    SDL_RenderCopy(renderer, player2ScoreTexture, nullptr, &player2ScoreDstRect);

    ballSrcRect = { 0, 0, 32, 32 };
    ballDstRect = { ballX, ballY, 32, 32 };
    SDL_RenderCopy(renderer, ballTexture, &ballSrcRect, &ballDstRect);

		// Up until now everything was drawn behind the scenes.
    // This will show the new, red contents of the window.
    SDL_RenderPresent(renderer);

    SDL_Delay(15);
  }

  //Free resources and close SDL
  close(window, renderer);

  return 0;
}
