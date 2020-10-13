

//Using SDL, SDL_image, SDL_ttf, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include "LTexture.h"
#include <string>


// iip is an integer integer pair, we use it for 
// representing a points coordinates.
typedef std::pair<int,int> iip;


//Screen dimension constants
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 20;

const int CLOCK_PERIOD = 150;
const auto SDL_COLOR_BLACK = SDL_Color {.r= 0, .g = 0x0, .b = 0, .a = 0xff };
const auto SDL_COLOR_RED = SDL_Color {.r= 0xff, .g = 0x0, .b = 0, .a = 0xff };


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFontTiny = NULL;
TTF_Font *gFont = NULL;
TTF_Font *gFontBig = NULL;


// How many pixels correspond to 1 unit in the grid
double xf, yf;


// from http://lazyfoo.net/SDL_tutorials/lesson32/index.php
// A class that keeps helps keep track of the ticks
class Timer {
private:
    //The clock time when the timer started
    int startTicks;
    bool started;

public:
    Timer()  {
    	startTicks = 0;
    	started = false;
	}

    //Start the timer and register the current clock time
    void start(){
	    started = true;
	    startTicks = SDL_GetTicks();
	}

    //Gets the timer's time
    int get_ticks(){
	    if( started == true ){
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
	    }
	    return 0;
	}
};

// A class to represent the snake
class snake {
private:
    // A snake is represented by a vector of the positions of its parts.
    // It starts with just one piece at coordinate (2,1)
    std::vector<iip> V{iip(2,1)};

public:
    // function appleCollision:
    //    If the head of the snake is in the same position as
    //    the apple, we extend the snake's tail and return true.
    bool appleCollision(const iip& apple, const iip& last) {
        if (apple.first == V[0].first and apple.second == V[0].second) {
            V.push_back(last);
            return true;
        }
        return false;
    }

    // function move:
    //    Given the coordinate of the apple, and the direction of movement
    //    modifies the snake chunks.
    int move(const iip& apple, char direction) {
        iip last = V[V.size()-1];

        // remove the last element
        V.erase(V.begin()+V.size()-1);

        iip head = V[0];

        if (direction == 'R') head.first++;
        else if (direction == 'L') head.first--;
        else if (direction == 'U') head.second++;
        else if (direction == 'D') head.second--;

        // insert at the first position
        V.insert(V.begin(),iip(head));

        return appleCollision(apple, last);
    }

    // function getV:
    //    Just a getter for the vector of 2D coordinates.
    const std::vector<iip> & getV() const {
        return V;
    }
};


// function init:
//    Initializes several of the structures needed by SDL.
//    Also creates the main window.
//    Nothing interesting to see or modify here.

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Snake", SDL_WINDOWPOS_UNDEFINED,
		        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else{
			//Create synced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 ) {
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

// function loadMedia:
//    Loads several fonts that we might use other projects.

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 20 );
	if( gFont == NULL ) {
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}

    gFontBig = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 40 );
    if( gFontBig == NULL ) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    gFontTiny = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 10 );
    if( gFontTiny == NULL ) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    return success;
}

// function close:
//    Before the program ends, we must free some reserved memories and quit
//    some SDL subsystems. This function takes care of that.

void close() {
	//Free global font
	TTF_CloseFont( gFont );
    gFontTiny = gFont = gFontBig = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// function drawPoint():
//   Given:
//     * a reference to the renderer
//     * a reference to a 2D point
//     * a reference to a RGB color
//   Draws a rectangle a point in the grid (a rectangle of width xf and height yf)

void drawPoint(SDL_Renderer* renderer, const iip &p, const SDL_Color& color){
	int x =  p.first * xf;
	int y = SCREEN_HEIGHT -  p.second * yf;
	SDL_Rect fillRect = { x ,  y , (int)xf, (int)yf };
	SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );		
	SDL_RenderFillRect( gRenderer, &fillRect );
}


int main( int argc, char* args[] ) {
	// the snake object
    snake aSnake;
    
    // the apple is just a random coordinate
    iip apple(rand()%(GRID_WIDTH-1) + 1,rand()%(GRID_HEIGHT-1) + 1);
	
    // a time object to keep track of time
	Timer delta;

	// this variable keeps track of the ticks the last time we entered
	// the game loop
	int prevTicks = 0;
	
	// the key pressed by the user, we initialize to R
	char key = 'R';

	// a temporary vector for reading the data vector of the snake
    std::vector<iip> tmpV;

    // compute the number of pixels per grid square
    // we will need these for paiting the snake and apple
    xf = SCREEN_WIDTH  / GRID_WIDTH;
    yf = SCREEN_HEIGHT / GRID_HEIGHT; 


	//Start up SDL and create window
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	}
	else {
		//Load media
		if( !loadMedia() ) {
			printf( "Failed to load media!\n" );
		}
		else {	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			delta.start();
			//While application is running
			while( !quit ) {
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 ) {
					//User requests quit
					if( e.type == SDL_QUIT ) { quit = true;}

					else if (e.type == SDL_KEYDOWN ) {
						if ( e.key.keysym.sym== SDLK_q)      quit = true;
						else if ( e.key.keysym.sym== SDLK_h) key = 'L';
						else if ( e.key.keysym.sym== SDLK_l) key = 'R';
						else if ( e.key.keysym.sym== SDLK_j) key = 'D';
						else if ( e.key.keysym.sym== SDLK_k) key = 'U';
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                int result = 0;

                // this block decides if it is time to move. We move
                // approximately CLOCK_PERIOD ticks
                if (prevTicks / CLOCK_PERIOD < delta.get_ticks()/CLOCK_PERIOD) {

                    result = aSnake.move(apple,key);
                    if (result) {
                        apple.first  = rand()%(GRID_WIDTH-1) + 1;
                        apple.second = rand()%(GRID_HEIGHT-1) + 1;
                    }
                }
                prevTicks  = delta.get_ticks();

				// read the list of positions of the snake and draw them
				tmpV = aSnake.getV();
				for (int i = 0; i < tmpV.size(); i++)
					drawPoint(gRenderer, tmpV[i], SDL_COLOR_BLACK);

                // draw the apple
				drawPoint(gRenderer, apple, SDL_COLOR_RED);

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}