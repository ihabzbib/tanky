#include <sstream>
#include <iostream>
#include <string>
#include "SDL/SDL_image.h"
#include "figure.h"

using namespace std;

void Figure::setImage(string filename){
	this->image =  load_image(filename);
}


SDL_Surface* Figure::load_image(string filename){

	this->imageFileName = filename;

	//Temporary storage for the image that's loaded
	SDL_Surface* loadedImage = NULL;

	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load image
	loadedImage = IMG_Load(filename.c_str());

	if(loadedImage == NULL){
		throw printf(("Can not load image from file: " + filename).c_str());
	}

	//If nothing went wrong in loading the image

	//Create an optimized image
	optimizedImage = SDL_DisplayFormat(loadedImage);

	//Free the old image
	SDL_FreeSurface(loadedImage);

	//Return the optimized image
	return optimizedImage;

}

Figure::Figure(){
	show = false;
	image = NULL;
	x = y = dx = dy = bg_x = bg_y = 0;
	height = width = 20;
}

/**
 * Loads the image in 32-bit, then returns a pointer to the optimized version of the loaded image at 24-bit.
 * @param string filename
 * @return SDL_Surface
 */
void Figure::clean_up(){
	if(image != NULL){
		SDL_FreeSurface(image);
	}

}
