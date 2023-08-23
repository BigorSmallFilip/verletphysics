#include "system.h"

#include <stdio.h>



SDL_Window*	main_window		= NULL;
int		main_window_width	= 0;
int		main_window_height	= 0;
double		main_window_refreshrate = 144;

SDL_Renderer*	main_renderer		= NULL;

SDL_Texture*	main_texture		= NULL;
int		screen_width		= 0;
int		screen_height		= 0;



static void show_error_box(
	const char* const title,
	const char* const message
)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, main_window);
}

void ShowError(
	const char* const title,
	const char* const message
)
{
	printf("%s | %s\n", title, message);
	show_error_box(title, message);
}

void ShowSDLError(
	const char* const message
)
{
	char boxmessage[200];
	sprintf_s(boxmessage, 200, "%s\nSDL_Error() = \"%s\"", message, SDL_GetError());
	ShowError("SDL Error!", boxmessage);;
}



static Bool init_sdl_video()
{
	printf("Initializing video subsystem system\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ShowSDLError("Couldn't initialize SDL!");
		return FALSE;
	}
	return TRUE;
}



static Bool init_main_window(
	const char* const	title,
	const int		width,
	const int		height
)
{
	printf(
		"Initializing main window\n title = \"%s\"\n width = %i\n height = %i\n",
		title,
		width,
		height
	);
	main_window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		0
	);
	if (!main_window)
	{
		ShowSDLError("Couldn't initialize main window!");
		return FALSE;
	}
	main_window_width = width;
	main_window_height = height;
	return TRUE;
}



static Bool init_main_renderer(
	const SDL_RendererFlags renderer_flags
)
{
	printf(
		"Initializing main renderer\n render_flags = %i\n",
		(int)renderer_flags
	);
	main_renderer = SDL_CreateRenderer(
		main_window,
		-1,
		renderer_flags
	);
	if (!main_renderer)
	{
		ShowSDLError("Couldn't initialize main renderer!");
		return FALSE;
	}
	return TRUE;
}



static Bool init_main_texture(
	const int		width,
	const int		height
)
{
	printf(
		"Initializing main texture\n width = %i\n height = %i\n",
		width,
		height
	);
	main_texture = SDL_CreateTexture(
		main_renderer,
		SDL_PIXELFORMAT_BGR888,
		SDL_TEXTUREACCESS_TARGET,
		width,
		height
	);
	if (!main_texture)
	{
		ShowSDLError("Couldn't initialize main texture!");
		return FALSE;
	}
	screen_width = width;
	screen_height = height;
	return TRUE;
}



Bool SystemInit(
	const char* const	window_title,
	const int		window_width,
	const int		window_height,
	const SDL_RendererFlags renderer_flags,
	const int		width,
	const int		height
)
{
	if (!init_sdl_video()) goto on_error;
	if (!init_main_window(window_title, window_width, window_height)) goto on_error;
	if (!init_main_renderer(renderer_flags)) goto on_error;
	if (!init_main_texture(width, height)) goto on_error;

	keyboard_inputs = (const char*)SDL_GetKeyboardState(NULL);

	return TRUE;

on_error:
	SystemQuit();
	return FALSE;
}



void SystemQuit()
{
	SDL_DestroyTexture(main_texture);
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);

	SDL_Quit();
}



void RenderWindow()
{
	SDL_SetRenderTarget(main_renderer, NULL);
	SDL_RenderCopy(main_renderer, main_texture, NULL, NULL);
	SDL_RenderPresent(main_renderer);
}



SystemInputs system_inputs	= 0;

const char* keyboard_inputs	= NULL;

int mousex			= 0;
int mousey			= 0;
MouseBtns mousebtns		= 0;

void UpdateInputs()
{
	SDL_Event e;

	system_inputs = 0;
	
	// Set mouse button click and release flags to 0
	mousebtns &= ~(MOUSEBTN_LCLICK | MOUSEBTN_LRELEASE | MOUSEBTN_RCLICK | MOUSEBTN_RRELEASE);

	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			system_inputs |= SYSINP_EXIT;
			break;

		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&mousex, &mousey);
			mousex = (int)((double)mousex / ((double)main_window_width / (double)screen_width));
			mousey = (int)((double)mousey / ((double)main_window_height / (double)screen_height));
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				mousebtns |= MOUSEBTN_LDOWN;
				mousebtns |= MOUSEBTN_LCLICK;
			}
			else if (e.button.button == SDL_BUTTON_RIGHT)
			{
				mousebtns |= MOUSEBTN_RDOWN;
				mousebtns |= MOUSEBTN_RCLICK;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				mousebtns &= ~MOUSEBTN_LDOWN;
				mousebtns |= MOUSEBTN_LRELEASE;
			}
			else if (e.button.button == SDL_BUTTON_RIGHT)
			{
				mousebtns &= ~MOUSEBTN_RDOWN;
				mousebtns |= MOUSEBTN_RRELEASE;
			}
			break;
		}
	}
}