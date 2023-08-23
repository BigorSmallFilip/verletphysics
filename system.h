#pragma once

#include "utility.h"
#include <SDL.h>

extern SDL_Window*	main_window;
extern int		main_window_width;
extern int		main_window_height;
extern double		main_window_refreshrate;

extern SDL_Renderer*	main_renderer;

extern SDL_Texture*	main_texture;
extern int		screen_width;
extern int		screen_height;



/**
 * @brief Shows an errorbox and writes to the console.
 * @param title A header for the error message.
 * @param message The error message.
*/
void ShowError(
	const char* const title,
	const char* const message
);

/**
 * @brief Shows an errorbox and writes to the console a formated error message which includes the SDL_GetError() string.
 * @param message A message which precedes the SDL_GetError() string.
*/
void ShowSDLError(
	const char* const message
);



/**
 * @brief Initializes main system variables.
 * @param window_title Text on the top of the window.
 * @param window_width Window width in pixels.
 * @param window_height Window height in pixels.
 * @param renderer_flags Flags given to the main_renderer.
 * @param width The main_texture width in pixels.
 * @param height The main_texture height in pixels.
 * @return TRUE on success, FALSE on failure. Stop executing if this fails.
*/
Bool SystemInit(
	const char* const	window_title,
	const int		window_width,
	const int		window_height,
	const SDL_RendererFlags renderer_flags,
	const int		width,
	const int		height
);

/**
 * @brief Quits and frees all main system variables.
*/
void SystemQuit();

/**
 * @brief Updates the main window with the main_texture.
*/
void RenderWindow();



typedef enum
{
	SYSINP_EXIT = 1,
} SystemInputs;
extern SystemInputs system_inputs;

extern const char* keyboard_inputs;

extern int mousex;
extern int mousey;

typedef enum
{
	MOUSEBTN_LCLICK		= 1,
	MOUSEBTN_LDOWN		= 2,
	MOUSEBTN_LRELEASE	= 4,
	MOUSEBTN_RCLICK		= 8,
	MOUSEBTN_RDOWN		= 16,
	MOUSEBTN_RRELEASE	= 32,
} MouseBtns;
extern MouseBtns mousebtns;

void UpdateInputs();
