
#include "../include/musicpreview.h"
#include "../include/logging.h"
#include "../include/signalhandling.h"

void preview_audio(const char *file_path) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        log_message(LOG_LEVEL_ERROR, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        log_message(LOG_LEVEL_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Load the music file
    Mix_Music *music = Mix_LoadMUS(file_path);
    if (music == NULL) {
        log_message(LOG_LEVEL_ERROR, "Failed to load music file! SDL_mixer Error: %s", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Play the music
    if (Mix_PlayMusic(music, 1) == -1) {
        log_message(LOG_LEVEL_ERROR, "SDL_mixer Error: %s", Mix_GetError());
    } else {
        log_message(LOG_LEVEL_INFO, "Playing music: %s", file_path);
    }

    // Wait until the music finishes
    int ch;
    while (Mix_PlayingMusic() && ch != 'q') {
        ch = getch();
        if (ch == 'q') {
          break;
        }
        SDL_Delay(100);
    }

    // Clean up
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();

    log_message(LOG_LEVEL_INFO, "Exiting SDL...");
    
}

