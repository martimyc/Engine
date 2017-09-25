#include <stdlib.h>

#include "Brofiler\Brofiler.h"

#include "Application.h"
#include "Globals.h"
#include "imgui\imgui.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#include "ModuleConsole.h"

enum MAIN_STATES
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	MAIN_STATES state = MAIN_CREATION;
	Application* App = NULL;
	
	while (state != MAIN_EXIT)
	{
		BROFILER_FRAME("Unice_Frame");

		switch (state)
		{
		case MAIN_CREATION:

			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			if (App->Init() == false)
			{
				App->LOG("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				App->LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				App->LOG("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			App->LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				App->LOG("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}
	delete App;
	return main_return;
}