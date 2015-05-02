#include <highgui.h>
#include <cv.h>
#include "opencv2/opencv.hpp"

//for the rasperry pi
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif


extern "C" {
	#include <SDL.h>
	#include <SDL_thread.h>
	#include <SDL_image.h>
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <SDL_mixer.h>

//#include <vector>
bool sick = true;
using namespace cv;

// Necessary definitions
/*
#define SDL_AUDIO_BUFFER_SIZE 1024
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000*/
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1232
#define MAX_CAMERAS 3

// "../../../../../lib/" - the location of music.
Mix_Music *gMusic = NULL;


 void close();
 int backupWorker(void* data);
 int gpsWorker(void* data);

 int videoStream = -1;
 int audioStream = -1;
 int quit = 0;
 bool surfaceFree1 = true;
 bool surfaceFree2 = true;
 int failures = 0;

//
 SDL_mutex* threadLock1 = NULL;
 SDL_cond* surfaceReady1 = NULL;
 SDL_cond* imageReady1 = NULL;
 SDL_Surface* threadSurface1 = NULL;
 SDL_Texture* threadText1 = NULL;

 SDL_mutex* threadLock2 = NULL;
 SDL_cond* surfaceReady2 = NULL;
 SDL_cond* imageReady2 = NULL;
 SDL_Surface* threadSurface2 = NULL;
 SDL_Texture* threadText2 = NULL;

 IplImage threadImage1;
 IplImage threadImage2;

 bool updatedImage1 = false;
 bool updatedImage2 = false;
//

 VideoCapture cap(0);
 VideoCapture cap2(0);
 int cameraHeight;
 int cameraWidth;

 Mat frame;
 Mat frame2;

 SDL_Renderer* renderer = NULL;
 SDL_Window* window = NULL;

 SDL_Rect videoRect;
 SDL_Rect videoRect2;

AVFormatContext* ctxArray[MAX_CAMERAS] = {NULL};	// context array for all cameras
IplImage* imageArray[MAX_CAMERAS];		// image array for all cameras

AVFormatContext* pFormatCtx = NULL;
AVFormatContext* pFormatCtx2 = NULL;


/***********************************************************************
/*							SDL functions 
/***********************************************************************/
// Initializes SDL window / renderer for use
bool init_SDL()
{
	bool success = true;
	
	if (SDL_Init(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) < 0)
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else 
	{
		window = SDL_CreateWindow("Video Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("error");
			success = false;
		}
		else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL)
			{
				printf("Renderer could not be created. SDL_Error: %s \n", SDL_GetError());
				success = false;
			}
			else 
			{
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (! (IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not init. SDL_image error: %s\n", IMG_GetError());
					success = false;
				}

				 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}

			}
		}
	}
	videoRect.x = 0;
	videoRect.y = 0;
	videoRect.w = 1280;	//640
	videoRect.h = 720;	//480
	
	videoRect2.x = 640;
	videoRect2.y = 0;
	videoRect2.w = 640;
	videoRect2.h = 480;

	// cap.set(CV_CAP_PROP_FRAME_WIDTH, videoRect.w);
	// cap.set(CV_CAP_PROP_FRAME_HEIGHT, videoRect.h);
	
	cameraWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	cameraHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	printf("Camera Width%d, Camera Height %d \n",cameraWidth,cameraHeight);

	threadLock1 = SDL_CreateMutex();
	imageReady1 = SDL_CreateCond();
	surfaceReady1 = SDL_CreateCond();
	
	threadLock2 = SDL_CreateMutex();
	imageReady2 = SDL_CreateCond();
	surfaceReady2 = SDL_CreateCond();
	return success;
}

bool loadMedia()
{
	bool success = true;
	//Load music
	gMusic = Mix_LoadMUS("assets/Polaris.mp3");
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

int backupWorker(void* data) 
{
	while (!quit)
	{
		cap >> frame;
		threadImage1 = frame;
		updatedImage1 = true;
	}
	return 0;
}

int gpsWorker(void* data)
{
	while (true)
	{

		cap2 >> frame2;
		threadImage2 = frame2;
		updatedImage2 = true;
	}
	return 0;
}

// Shows an individual frame of the supplied video
int show_Camera(IplImage* img){	
	//SDL_RenderClear(renderer);
	
	if(updatedImage1 == true){

		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)img->imageData,
			img->width,
			img->height,
			img->depth * img->nChannels,
			img->widthStep,
			0xff0000, 0x00ff00, 0x0000ff, 0
			);
		updatedImage1 = false;

		//SDL_DestroyTexture(threadText1);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		surface = NULL;
		SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;
		SDL_RenderCopyEx(renderer, texture, NULL, &videoRect ,0, NULL, flip);
		SDL_DestroyTexture(texture);
		return 1;
	}
		return 0;

}

void show_GPS(IplImage* img2){
	SDL_LockMutex(threadLock2);
	if (updatedImage2 == false){
		SDL_CondSignal(surfaceReady2);
		SDL_UnlockMutex(threadLock2);
		return;
	}
	SDL_Surface* surface2 = SDL_CreateRGBSurfaceFrom((void*)img2->imageData,
		img2->width,
		img2->height,
		img2->depth * img2->nChannels,
		img2->widthStep,
		0xff0000, 0x00ff00, 0x0000ff, 0
		);
	updatedImage2 = false;
	SDL_CondSignal(surfaceReady2);
	SDL_UnlockMutex(threadLock2);

	SDL_DestroyTexture(threadText2);

	SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, surface2);

	SDL_FreeSurface(surface2);

	surface2 = NULL;


	//SDL_RenderCopyEx(renderer, texture2, NULL, &videoRect2, 0, NULL, flip);
}

/***********************************************************************
 * 
 * *********************************************************************/
 
 int main(int argc, char* argv[])
 {

    
 	av_register_all();

 	if (!init_SDL()){
 		fprintf(stderr, "Could not initialize SDL!\n");
 		return -1;
 	}
 	if (!cap.isOpened()){
 		fprintf(stderr, "Failed to load file!\n");
 		return -1;
 	}
	if( !loadMedia() ){
		printf( "Failed to load media!\n" );
	}


 	AVPacket packet;
	// read frame is undefined

 	SDL_Thread* threadID = SDL_CreateThread(backupWorker, "Backup Camera Thread", NULL);
 	//SDL_Thread* gpsThread = SDL_CreateThread(gpsWorker, "GPS Camera Thread", NULL);

	surfaceFree1 = false;
	surfaceFree2 = false;

	int screenUpdate = 0;
	int loops = 0;
	int threadReturnValue;	



 	while (!quit)
 	{
		loops++;
 		SDL_Event event;
 		while (SDL_PollEvent(&event)){
	 		switch(event.type)
	 		{
	 			case SDL_QUIT:
		 			quit = 1;
		 			close();
		 			SDL_Quit();
		 			exit(0);
		 			break;

	 			case SDL_KEYDOWN:
	 				switch(event.key.keysym.sym) {
				 		case SDLK_ESCAPE: 
				 			SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0xFF, 0xFF );   
				 		 	printf("Esc was Pressed!");
				       	 	quit = true;
				       	 	SDL_WaitThread(threadID, &threadReturnValue);
				       	 	printf("\nThread returned value: %d", threadReturnValue);
				       	 	close();
				 			SDL_Quit();
				 			exit(0);
				        	break;

						case SDLK_9:
							//If there is no music playing
							if( Mix_PlayingMusic() == 0 )
							{
								//Play the music
								Mix_PlayMusic( gMusic, -1 );
							}
							//If music is being played
							else
							{
								//If the music is paused
								if( Mix_PausedMusic() == 1 )
								{
									//Resume the music
									Mix_ResumeMusic();
								}
								//If the music is playing
								else
								{
									//Pause the music
									Mix_PauseMusic();
								}
							}
							break;
						
						case SDLK_0:
							//Stop the music
							Mix_HaltMusic();
							break;

			        	default:
			 				break;
			    }

	 		}
 		}

 		
		screenUpdate = show_Camera(&threadImage1);

		//show_GPS(&threadImage2);

		if (screenUpdate == 1){
			//SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2 , SCREEN_HEIGHT+100);
			SDL_RenderPresent(renderer);
			loops = 0;
		}
//		cvWaitKey(1); //any way to do this naturally?

	}



	SDL_WaitThread(threadID, NULL);
	//SDL_WaitThread(gpsThread, NULL);

	avformat_close_input(&pFormatCtx);
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		avformat_close_input(&ctxArray[i]);
	}
	return 0;
}

	void close()
	{
		Mix_FreeMusic(gMusic);
		Mix_CloseAudio();
		SDL_DestroyTexture(threadText1);
		SDL_DestroyTexture(threadText2);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		// SDL_DestroyMutex(threadLock1);
		// SDL_DestroyMutex(threadLock2);
		threadText1 = NULL;
		threadText2 = NULL;
		threadLock1 = NULL;
		threadLock2 = NULL;
		window = NULL;
		renderer = NULL;
	//Quit SDL subsystems
		IMG_Quit();
		SDL_Quit();
	}
