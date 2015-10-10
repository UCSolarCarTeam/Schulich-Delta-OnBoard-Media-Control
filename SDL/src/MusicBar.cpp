#include "MusicBar.h"

MusicBar::MusicBar(SongPlayer *songPlayer)
{
    mPlayer = songPlayer;
    int musicbarSurfaceWidth = 1080;
    int musicbarSurfaceHeight = 64;
    surface = SDL_CreateRGBSurface(0, musicbarSurfaceWidth, musicbarSurfaceHeight, 32, 0, 0, 0, 0);
    init();    
}

int MusicBar::init()
{
    int songNameFontSize; 
    int timeFontSize;
    
    songNameFontSize = 45;
    timeFontSize = 25;
    
    if (TTF_Init() !=0)
    {
        fprintf(stderr, "TTF_Init Failed%s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    songNameFont = TTF_OpenFont("assets/ant-maru.ttf", songNameFontSize);
    if (songNameFont == NULL)
    {
        fprintf(stderr, "TTF_OpenFont Failed%s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    
    timeFont = TTF_OpenFont("assets/Trebuchet-MS.ttf", timeFontSize);
    if (timeFont == NULL)
    {
        fprintf(stderr, "TTF_OpenFont Failed%s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
}

void MusicBar::drawSongName()
{
    std::string songName;
    int songStringLength;
    const char * songChar;
    int songWidth;
    int songHeight;

    songName = mPlayer->currentSong();
    printf("Song name is: %s\n", songName.c_str());
    songStringLength = songName.length();
    songName = songName.substr(12, songStringLength - 16); // removes SongLibrary/ and .mp3 from songName
    songChar = songName.c_str(); 
    
    SDL_Surface *songSurface;
    SDL_Color songColor = {255, 255, 255}; // EDIT
    SDL_Color songBackgroundColor = {43,43,43};
    //songSurface = TTF_RenderText_Solid(songNameFont, songChar, songColor);
    //songSurface = TTF_RenderUTF8_Shaded(songNameFont, songChar, songColor, songBackgroundColor );
    songSurface = TTF_RenderUTF8_Blended(songNameFont, songChar, songColor);
    TTF_SizeText(songNameFont, songChar, &songWidth, &songHeight);
    //if (songWidth > 880)
    //    songFontSize = 30;
    SDL_Rect songLocation = {1080/2 - songWidth/2, 64/2 - songHeight/2, 0, 0};      // EDIT
    SDL_BlitSurface(songSurface, NULL, surface, &songLocation);
    SDL_FreeSurface(songSurface);
}

std::string MusicBar::convertToString(int songIntTime)
{
    std::string songStringTime;

    std::stringstream convertSongIntTime;
    convertSongIntTime << songIntTime;
    songStringTime = convertSongIntTime.str();

    return songStringTime;
}

void MusicBar::drawSongTime()
{ 
    double songCurrentTime;
    int songCurrentMins;
    int songCurrentSecs;
    double songCurrentPercent;
    int songCurrentIntTime;
 
    std::string songCurrentStrTime;
    std::string songCurrentStrMins;
    std::string songCurrentStrSecs;
    const char * songCurrentCharTime; 

    double songTotalTime;
    int songTotalMins;
    int songTotalSecs;
    int songTotalIntTime;

    std::string songTotalStrTime;
    std::string songTotalStrMins;
    std::string songTotalStrSecs;
    const char * songTotalCharTime;

    int totalTimeWidth;
    int totalTimeHeight;

    songCurrentTime = mPlayer->getCurrentTime();
    songTotalTime = mPlayer->getSongLength();
    
    songCurrentPercent = songCurrentTime / songTotalTime; 
    songCurrentIntTime = songCurrentTime;
    songCurrentMins = songCurrentIntTime / 60; // EDIT?
    songCurrentSecs = songCurrentIntTime % 60; // EDIT?

    //songTotalTime = songTotalTime - songCurrentTime; // Remove to stop end time increment
    songTotalIntTime = songTotalTime;
    songTotalMins = songTotalTime / 60; // EDIT?
    songTotalSecs = songTotalIntTime % 60; // EDIT?
    
    songCurrentStrMins = convertToString(songCurrentMins);
    songCurrentStrSecs = convertToString(songCurrentSecs);
    
    if (songCurrentSecs < 10) // EDIT?
    {
        songCurrentStrTime = songCurrentStrMins + ":0" + 
                                songCurrentStrSecs;
    }
    else
    {
        songCurrentStrTime = songCurrentStrMins + ":" + songCurrentStrSecs;
    }
    
    songCurrentCharTime = songCurrentStrTime.c_str();

    songTotalStrMins = convertToString(songTotalMins);
    songTotalStrSecs = convertToString(songTotalSecs);
    
    if (songTotalSecs < 10) // EDIT?
    {
        songTotalStrTime = songTotalStrMins + ":0" + songTotalStrSecs;
    }
    else
    {
        songTotalStrTime = songTotalStrMins + ":" + songTotalStrSecs;
    }

    songTotalCharTime =  songTotalStrTime.c_str();
    
    SDL_Surface *songCurrentTimeSurface;
    SDL_Color songCurrentTimeColor = {255, 255, 255}; // EDIT
    //songCurrentTimeSurface = TTF_RenderText_Solid(timeFont, songCurrentCharTime, songCurrentTimeColor);
    songCurrentTimeSurface = TTF_RenderText_Blended(timeFont, songCurrentCharTime, songCurrentTimeColor);
    SDL_Rect songCurrentTimeLocation = {0, 4, 0, 0}; // EDIT
    SDL_BlitSurface(songCurrentTimeSurface, NULL, surface, &songCurrentTimeLocation);
    SDL_FreeSurface(songCurrentTimeSurface);
    
    SDL_Surface *songTotalTimeSurface;
    SDL_Color songTotalTimeColor = {255, 255, 255}; // EDIT
    //songTotalTimeSurface = TTF_RenderText_Solid(timeFont, songTotalCharTime, songTotalTimeColor);
    songTotalTimeSurface = TTF_RenderText_Blended(timeFont, songTotalCharTime, songTotalTimeColor);
    TTF_SizeText(timeFont, songTotalCharTime, &totalTimeWidth, &totalTimeHeight);
    SDL_Rect songTotalTimeLocation = {1080 - totalTimeWidth, 4, 0}; // EDIT
    SDL_BlitSurface(songTotalTimeSurface, NULL, surface, &songTotalTimeLocation);
    SDL_FreeSurface(songTotalTimeSurface);

    SDL_Surface *songTimebarSurface;
    songTimebarSurface = SDL_CreateRGBSurface(0, 0 + songCurrentPercent*1080, 3, 32, 0, 0, 0, 0); // EDIT
    SDL_FillRect(songTimebarSurface, NULL, SDL_MapRGB(songTimebarSurface->format,0,162,255)); // EDIT
    SDL_Rect songTimebarLocation = {0, 0, 0, 0}; // EDIT
    SDL_BlitSurface(songTimebarSurface, NULL, surface, &songTimebarLocation);
    SDL_FreeSurface(songTimebarSurface);
}

void MusicBar::drawVolumeBar()
{
    double songVolumeCurrent;
    double songVolumePercent;
    double songVolumeMax;
    songVolumeMax = 2.0;

    songVolumeCurrent = mPlayer->getVolume();
    
    songVolumePercent = songVolumeCurrent / songVolumeMax;

    // Volume Background Bar Prototype
    SDL_Surface *volumeBackgroundSurface;
    int volumeBackgroundHeight;
    int volumeBackgroundXLocation;
    int volumeBackgroundYLocation;
    int i; 
    int volumeBarNumber;

    volumeBarNumber = 20;
    
    volumeBackgroundHeight = 4;
    volumeBackgroundXLocation = 880;
    volumeBackgroundYLocation = 50;
    
    for (i = 0; i < volumeBarNumber; i++) // EDIT
    {
        volumeBackgroundSurface = SDL_CreateRGBSurface(0,4, volumeBackgroundHeight,32,0,0,0,0);
        SDL_FillRect(volumeBackgroundSurface, NULL, SDL_MapRGB(volumeBackgroundSurface->format,0,0,0));
        SDL_Rect volumeBackgroundLocation = {volumeBackgroundXLocation,volumeBackgroundYLocation,0,0};
        SDL_BlitSurface(volumeBackgroundSurface, NULL, surface, &volumeBackgroundLocation);
        SDL_FreeSurface(volumeBackgroundSurface);

        volumeBackgroundHeight += 2; // EDIT
        volumeBackgroundXLocation += 6; // EDIT
        volumeBackgroundYLocation -= 2; // EDIT
    }

    // Volume Bar Prototype
    SDL_Surface *volumeSurface;
    int volumeHeight;
    int volumeXLocation;
    int volumeYLocation;
    int colorGreen;
    
    volumeHeight = 4;
    volumeXLocation = 880;
    volumeYLocation = 50;
    colorGreen = 162;
    songVolumePercent = songVolumePercent * volumeBarNumber - 0.1;

    for (i = 0; i < songVolumePercent; i++) 
    {
        volumeSurface = SDL_CreateRGBSurface(0,4, volumeHeight, 32,0,0,0,0); // EDIT
        SDL_FillRect(volumeSurface, NULL, SDL_MapRGB(volumeSurface->format,255,colorGreen,0)); //EDIT
        SDL_Rect volumeLocation = {volumeXLocation, volumeYLocation, 0, 0}; // EDIT
        SDL_BlitSurface(volumeSurface, NULL, surface, &volumeLocation);
        SDL_FreeSurface(volumeSurface);
    
        volumeHeight += 2; // EDIT
        colorGreen -= 6; // EDIT
        volumeXLocation += 6; // EDIT
        volumeYLocation -= 2; // EDIT
    }
}

void MusicBar::update()
{
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format,43,43,43)); // EDIT
    drawSongName();
    drawSongTime();
    drawVolumeBar();
}

SDL_Surface* MusicBar::returnMusicBar()
{
    return surface;
}














