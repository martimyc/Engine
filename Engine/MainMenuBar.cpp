#include "SDL2\include\SDL.h"
#include "imgui\imgui.h"
#include "Application.h"
#include "Console.h"
#include "BasicGeometry.h"
#include "Audio.h"
#include "Camera3D.h"
#include "HardwareSoftwareInfo.h"
#include "Renderer3D.h"
#include "Window.h"
#include "TimeManager.h"
//TODO if we only have 1 obj
#include "SceneManager.h"
#include "GameObject.h"
//----------
#include "MainMenuBar.h"

MainMenuBar::MainMenuBar(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

MainMenuBar::~MainMenuBar()
{
}

UPDATE_STATUS MainMenuBar::Update(float dt)
{
	UPDATE_STATUS ret = UPDATE_ERROR;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				SDL_Event e;
				e.type = SDL_QUIT;
				SDL_PushEvent(&e);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Editor Camera Info"))
					App->camera->OpenCloseMatricesDebugWindow();

				if (ImGui::MenuItem("Timers"))
					App->time_manager->OpenCloseTimers();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Configuration"))
			{
				if (ImGui::MenuItem("Application"))
					App->OpenCloseConfigAppWindow();

				if (ImGui::MenuItem("Audio"))
					App->audio->OpenCloseConfigAudioWindow();

				if (ImGui::MenuItem("Camera"))
					App->camera->OpenCloseConfigCameraWindow();

				if (ImGui::MenuItem("Hardware/Software info"))
					App->hardware_software_info->OpenCloseConfigHardwareSoftwareWindow();

				if (ImGui::MenuItem("3D Renderer"))
					App->renderer_3d->OpenCloseConfigRendererWindow();

				if (ImGui::MenuItem("Window"))
					App->window->OpenCloseConfigWindowWindow();

				if (ImGui::MenuItem("Draw Modes"))
					App->scene_manager->OpenCloseConfigSceneWindow();

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Console"))
				App->console->OpenCloseConsoleWindow();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			/*if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
					App->primitives->Create3DCube();

				if (ImGui::MenuItem("Sphere"))
					App->primitives->CreateSphere();

				ImGui::EndMenu();
			}*/
			if(ImGui::MenuItem("Create Empty"))
				App->scene_manager->AddEmptyGO();

			if (ImGui::MenuItem("Camera"))
				App->scene_manager->CreateCamera();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Download latest"))
				App->OpenWebsite("https://github.com/martimyc/Engine/releases");

			if (ImGui::MenuItem("Report a bug"))
				App->OpenWebsite("https://github.com/martimyc/Engine/issues");

			if (ImGui::MenuItem("About"))
				show_about = !show_about;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if (App->BeginDockWindow("About", &show_about))
		{
			ImGui::Text("%s", App->GetTitle());
			ImGui::Text("Organization: %s", App->GetOrganization());

			ImGui::Separator();
			ImGui::Text("Authors:");

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

			if (ImGui::Button("Marc Latorre Freixes"))
				App->OpenWebsite("https://github.com/marclafr");

			if (ImGui::Button("Marti Majo Ylla-Catala"))
				App->OpenWebsite("https://github.com/martimyc");

			ImGui::PopStyleColor(3);

			ImGui::Separator();
			ImGui::Text("Libraries:");

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.57f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.57f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.57f, 0.8f, 0.8f));

			if (ImGui::Button("ImGui 1.51"))
				App->OpenWebsite("https://github.com/ocornut/imgui/releases/tag/v1.51");

			ImGui::SameLine();
			if (ImGui::Button("Brofiler 1.1.2"))
				App->OpenWebsite("http://brofiler.com/");

			ImGui::SameLine();
			if (ImGui::Button("Math Geo Lib 1.5"))
				App->OpenWebsite("https://github.com/juj/MathGeoLib/releases/tag/v1.5");


			if (ImGui::Button("SDL 2.0.6"))
				App->OpenWebsite("https://www.libsdl.org/download-2.0.php");

			ImGui::SameLine();
			if (ImGui::Button("OpenGL 4.0"))
				App->OpenWebsite("https://www.opengl.org/");

			ImGui::SameLine();
			if (ImGui::Button("Glew 2.1.0"))
				App->OpenWebsite("https://github.com/nigels-com/glew/releases/tag/glew-2.1.0");

			ImGui::SameLine();
			if (ImGui::Button("MMGR"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/Engine/mmgr/README.txt");


			if (ImGui::Button("Assimp"))
				App->OpenWebsite("http://www.assimp.org/");

			ImGui::SameLine();
			if (ImGui::Button("DeviL"))
				App->OpenWebsite("http://openil.sourceforge.net/");

			ImGui::SameLine();
			if (ImGui::Button("Dirent"))
				App->OpenWebsite("https://github.com/tronkko/dirent");

			ImGui::SameLine();
			if (ImGui::Button("Parson"))
				App->OpenWebsite("http://kgabis.github.io/parson/");

			ImGui::SameLine();
			if (ImGui::Button("MD5 (UUIDs)"))
				App->OpenWebsite("https://rosettacode.org/wiki/MD5#C");
					

			ImGui::PopStyleColor(3);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 0.8f, 0.8f));

			ImGui::Separator();
			if (ImGui::Button("License"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/LICENSE");

			ImGui::PopStyleColor(3);
		}
		App->EndDockWindow();

		ret = UPDATE_CONTINUE;
	}
	return ret;
}
