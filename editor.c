#include "editor.h"

#include "system.h"
#include "physics.h"



int g_move_lineid = -1;
float g_move_linex1 = 0;
float g_move_liney1 = 0;
float g_move_linex2 = 0;
float g_move_liney2 = 0;

void UpdateEditor()
{
	SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
	SDL_Rect mouserect = { mousex - 1, mousey - 1, 3, 3 };
	SDL_RenderDrawRect(main_renderer, &mouserect);

	if (mousebtns & MOUSEBTN_LCLICK)
	{
		// Start drawing new line
		AddLine(mousex, mousey, mousex, mousey, 30);
		g_move_lineid = g_scene_lines.count - 1;
	} else if (mousebtns & MOUSEBTN_LRELEASE)
	{
		g_move_lineid = -1;
	} else if ((mousebtns & MOUSEBTN_RDOWN) && (g_move_lineid == -1))
	{
		// remove_lines_at_position
	}

	if (g_move_lineid >= 0)
	{
		Line* line = g_scene_lines.items[g_move_lineid];
		MoveLine(line, line->x1, line->y1, mousex, mousey, line->width, g_physics_substeps);
	}
}
