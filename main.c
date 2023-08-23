#include "utility.h"
#include "system.h"

#include "scene.h"
#include "render.h"
#include "physics.h"
#include "editor.h"



int main(int argc, char** argv)
{
	if (!SystemInit(
		"Window title",
		1600,				// Window width
		900,				// Window height
		SDL_RENDERER_ACCELERATED |	// Render flags
		//SDL_RENDERER_PRESENTVSYNC |
		SDL_RENDERER_TARGETTEXTURE,
		1600,				// Screen width
		900)				// Screen height
		) return -1;

	InitRender();

	InitScene(20000, 2, 4, 40);



	unsigned long long timer_start = 0;
	unsigned long long timer_end = 0;
	double smoothed_framerate = 0;
	double dt = 0;
	
	while (!(system_inputs & SYSINP_EXIT))
	{
		timer_start = SDL_GetPerformanceCounter();

		UpdateInputs();
		SDL_SetRenderTarget(main_renderer, main_texture);
		SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
		SDL_RenderClear(main_renderer);
		
		UpdatePhysics(dt);
		RenderScene();

		UpdateEditor();

		RenderWindow();

		timer_end = SDL_GetPerformanceCounter();
		dt = ((double)(timer_end - timer_start)) / (double)SDL_GetPerformanceFrequency();
		double framerate = 1.0 / dt;
		smoothed_framerate = (smoothed_framerate * 99 + framerate) / 100;
		
		printf("FPS = % 7u        Smoothed FPS = % 7u        dt = % fms\r", (unsigned int)framerate, (unsigned int)smoothed_framerate, dt * 1000);
	}

	SystemQuit();

	return 0;
}
