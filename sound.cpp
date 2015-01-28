//============================================================================
// Name        : sound.cpp
// Author      : Ihab Zbib
// Version     :
// Copyright   : CSUSB
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string>
#include "playground.h"
#include "sound.h"


using namespace std;

extern bool quit; //global variable

extern SDL_mutex *sound_mutex;			//mutex for accessing sound


Sound::Sound(){
	shoot = false;
	explode = false;
	pause = false;
}

void Sound::load_music(string filename){
	music = Mix_LoadMUS(filename.c_str());


	if (music == NULL) {
		throw printf("Unable to load OGG file: %s\n", Mix_GetError());
	}

	//Play that funky music!
	//  -1 in argument means play music again and again, if 0, means play once
	if (Mix_PlayMusic(music, -1) == -1) {
		throw printf("Unable to play OGG file: %s\n", Mix_GetError());
	}


}


bool musicPlaying = true;
Mix_Chunk *sfx;

void channelFinished(int channel) {
	Mix_FreeChunk(sfx);
}

//Play the audio chunk
void play_sfx(char *name) {
	int channel;

	//load wav file
	sfx = Mix_LoadWAV(name);
	//play once
	channel = Mix_PlayChannel(-1, sfx, 0);
	Mix_ChannelFinished(channelFinished);
}

void musicFinished() {
	//Music is done!
	musicPlaying = false;
}

int sound_thread(void *src) {

	Sound *sound = ( Sound* ) src;

	int audio_rate = 22050; //Frequency of audio playback
	Uint16 audio_format = AUDIO_S16SYS; //Format of the audio we're playing
	int audio_channels = 2; //2 channels = stereo
	int audio_buffers = 4096; //Size of the audio buffers in memory

	//Initialize SDL_mixer with our chosen audio settings
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)
			!= 0) {
		printf("Unable to initialize audio: %s\n", Mix_GetError());
		return 1;
	}
	SDL_mutexP(sound_mutex); //Lock sound mutex
	sound->load_music("sound/funky.mid");
	SDL_mutexV(sound_mutex); //Release sound mutex

	//Make sure that the musicFinished() function is called when the music stops playing
	Mix_HookMusicFinished(musicFinished);

	while (!quit) {

		if (sound->pause) {
			Mix_PauseMusic(); //pause music
			musicPlaying = false;
		}
		else if (!musicPlaying) {
			Mix_ResumeMusic(); //resume playing music
			musicPlaying = true;
		}

		if(sound->explode){
			SDL_mutexP(sound_mutex); //Lock sound mutex
			sound->load_music("sound/cannon3.wav");
			SDL_Delay(500);
			sound->load_music("sound/funky.mid");
			sound->explode = false;
			SDL_mutexV(sound_mutex); //Release sound mutex
		}

		else if(sound->shoot){
			SDL_mutexP(sound_mutex); //Lock sound mutex
			sound->load_music("sound/bomb3.wav");
			SDL_Delay(300);
			sound->load_music("sound/funky.mid");
			sound->shoot = false;
			SDL_mutexV(sound_mutex); //Release sound mutex
		}

		SDL_Delay(100); //give up some cpu time for others

	}

	//Release the memory allocated to our music
	Mix_FadeOutMusic(1000); //fade out music in 1 sec
	Mix_HaltMusic();
	Mix_FreeMusic(sound->music);

	//Need to make sure that SDL_mixer and SDL have a chance to clean up
	//Mix_CloseAudio();

	return 0;
}

