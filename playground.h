#include "SDL/SDL_image.h"
#include <string>
#include "figure.h"
#include "sound.h"

#ifndef PLAYGROUND_H
#define PLAYGROUND_H

class PlayGround{
	public:
		int width, height, score, lives;
		SDL_Surface* screen;
		bool target_hit;
		bool tank_hit;
		bool missile_hit;


		Sound soundtrack;
		Figure* missile;
		Figure* tank;
		Figure* target;

		PlayGround();
		PlayGround(int width, int height);
		void init();
		void apply_image(Figure* figure);
		void clean_up();
		void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
		void refresh(int frame_rate);
		void addDot(Figure* dot);
		Figure* dots[5];
		int dot_idx;
		int collide_bounding_box(Figure *sa, Figure *sb);
		void displayText(string text, int x, int y, int size);
		bool key_pressed;
		int event;
		int max_dots;
	private:
		string numberToString(int number);

};
#endif
