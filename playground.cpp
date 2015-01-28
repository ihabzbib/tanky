#include "SDL/SDL_ttf.h"
#include <sstream>
#include <iostream>
#include <string>
#include "playground.h"
#include "sound.h"

using namespace std;

extern SDL_Event event;

extern SDL_mutex *pg_mutex;		//mutex for accessing key queues

extern bool quit;		//global variable

//The attributes of the screen
const int SCREEN_BPP    = 32;

//The surface that will be used
SDL_Surface* screen = NULL;

PlayGround::PlayGround(int width, int height){
	this->width = width;
	this->height = height;

	//Initialize the screen
	 this->init();
}

void PlayGround::displayText(string text, int x, int y, int size){

	//Open the font
	TTF_Font* font = TTF_OpenFont( "font/arial.ttf", size );
	//If there was an error in loading the font
	if( font == NULL )
	{
		throw printf("PlayGround::init(): font is NULL");
	}

	SDL_Color textColor = { 0, 0, 0 };

	SDL_Surface* sdl_text = TTF_RenderText_Solid( font, text.c_str(), textColor );

	//If there was an error in rendering the text
	if( sdl_text == NULL )
	{
		throw printf("PlayGround::refresh sdl_text is NULL");
	}

	apply_surface( x, y, sdl_text, screen);


	SDL_FreeSurface(sdl_text);

	//Close the font that was used
	TTF_CloseFont(font);

}

string PlayGround::numberToString(int num){
	stringstream number;
	number << num;
	return number.str();
}

void PlayGround::refresh(int frame_rate){

	//Fill the screen white
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = width;
	rect.h = 35;

	//Fill the screen white//EEF3E2
	SDL_FillRect( screen, &rect, SDL_MapRGB( screen->format, 0xEE, 0xF3, 0xE2 ) );


	//Display options
	displayText("F2=NEW  ESC=QUIT", 20, 5, 25);


	//Display score
	displayText("Score " + numberToString(score), width - 270, 5, 25);

	//Display lives
	displayText("Lives " + numberToString(lives), width - 100, 5, 25);


	if(lives > 0){
		//Draw the figures
		if(tank->show){
			apply_image(tank);
		}
		if(target->show){
			apply_image(target);
		}
		if(missile->show){
			apply_image(missile);
		}
		for(int i=0; i<dot_idx; i++){
			if(dots[i]->show){
				apply_image(dots[i]);
			}
		}
	}
	else{
		lives = 0;
		//Display Game Over
		displayText("Game Over", (width / 2) - 250, (height / 2) - 50, 100);
		soundtrack.pause = true;
	}


	//Update the screen
	if( SDL_Flip( screen ) == -1 ) {
		throw printf("PlayGround::refresh failed. SDL_Flip");
	}


	SDL_Delay(frame_rate);
}


int PlayGround::collide_bounding_box(Figure *sa,  Figure *sb)
{

	if(!sa->show || !sb->show || lives == 0){
		return 0;
	}

	int ax, ay, bx, by;
	ax = sa->x;
	ay = sa->y;
	bx = sb->x;
	by = sb->y;

	if(bx + sb->image->w < ax)	return 0;	//just checking if their
	if(bx > ax + sa->image->w)	return 0;	//bounding boxes even touch

	if(by + sb->image->h < ay)	return 0;
	if(by > ay + sa->image->h)	return 0;

	return 1;
}


/**
 * takes in the coordinates of where you want to blit the surface,
 * the surface you're going to blit and the surface you're going to blit it to.
 *DL_BlitSurface() only accepts the offsets inside of an SDL_Rect.
 * @param int x
 * @param int y
 * @param SDL_Surface source
 * @param SDL_Surface destination
 * @return void
 */
void PlayGround::apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination){
	//Make a temporary rectangle to hold the offsets
	SDL_Rect offset;

	//Give the offsets to the rectangle
	offset.x = x;
	offset.y = y;

	//Blit the surface
	SDL_BlitSurface(source, NULL, destination, &offset);
}

void PlayGround::addDot(Figure* dot){
	dots[dot_idx] = dot;
	dot_idx++;
	if(dot_idx > max_dots){
		dot_idx = 0;
	}
}

/**
 * Initialize
 * @return bool
 */
void PlayGround::init(){
	//Initialize the SDL subsystem
	if(SDL_Init( SDL_INIT_EVERYTHING ) == -1){
		throw printf("PlayGround::init(): SDL_Init has failed");
	}

	//Set up screen
	screen = SDL_SetVideoMode(width, height, SCREEN_BPP, SDL_SWSURFACE);

	//If there was an error in setting up the screen
	if(screen == NULL){
		throw printf("PlayGround::init(): screen is NULL");
	}

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
    	throw printf("PlayGround::init(): TTF_Init has failed");
    }

    //Initialize class variables
	target_hit = false;
	tank_hit = false;
	missile_hit = false;
	soundtrack = Sound();
	dot_idx = 0;
	score = 0;
	lives = 3;
	max_dots = 5;
	soundtrack.pause = false;

	//Set the window caption
	SDL_WM_SetCaption("Tanky by Ihab Zbib, CSUSB CSCI 595", NULL);
}



void PlayGround::apply_image(Figure* figure) {

	//If there was an error in loading the image
	if( figure == NULL || figure->image == NULL ) {
		throw printf(("PlayGround::apply_image: " + figure->imageFileName + " is NULL").c_str());
	}

	//Apply the surface to the screen
	apply_surface( figure->x, figure->y, figure->image, screen );

}



void PlayGround::clean_up(){

	//Free Surfaces
	SDL_FreeSurface(screen);

	//Quit SDL_ttf
	TTF_Quit();

	//Quit	SDL
	SDL_Quit();
}
