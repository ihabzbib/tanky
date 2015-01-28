#include "SDL/SDL_image.h"
#include <string>
#ifndef FIGURE_H
#define FIGURE_H

using namespace std;

class Figure{
	public:
		int x, y, dx, dy, bg_x, bg_y, height, width;
		SDL_Surface* image;
		Figure();
		SDL_Surface* load_image(string filename);
		void setImage(string filename);
		void clean_up();
		string imageFileName;
		string imageBaseName;
		bool show;
};
#endif
