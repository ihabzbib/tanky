#include <string>
#include "SDL/SDL_mixer.h"

#ifndef SOUND_H
#define	SOUND_H
using namespace std;

int sound_thread(void *surf);
class Sound{
	public:
		bool shoot;
		bool explode;
		bool pause;
		Mix_Music *music;
		void load_music(string filename);
		Sound();
};

#endif
