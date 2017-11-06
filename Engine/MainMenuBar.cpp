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

				if (ImGui::MenuItem("Scene"))
					App->scene_manager->OpenCloseConfigSceneWindow();

				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("Console"))
				App->console->OpenCloseConsoleWindow();

			//if (ImGui::BeginMenu("Tests"))
			//{
				//if (ImGui::MenuItem("Random number Test"))
				//	Activate(UI_RAND_TEST);

				//if (ImGui::MenuItem("Geometry collision Test"))
				//	Activate(UI_GEOMETRY_COLLISION_TEST);

				//if (ImGui::MenuItem("ImGui Demo"))
				//	Activate(UI_TEST);
				
				//ImGui::EndMenu();
			//}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
					App->primitives->Create3DCube();

				if (ImGui::MenuItem("Sphere"))
					App->primitives->CreateSphere();
			
				ImGui::EndMenu();
			}

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
		
		if (show_about)
		{
			ImGui::Begin("About", &show_about);

			ImGui::Text("%s", App->GetTitle());
			ImGui::Text("Organization: %s", App->GetOrganization());

			ImGui::Separator();
			ImGui::Text("Authors:");

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

			if (ImGui::Button("Marc Latorre"))
				App->OpenWebsite("https://github.com/marclafr");

			if (ImGui::Button("Marti Majo"))
				App->OpenWebsite("https://github.com/martimyc");

			ImGui::PopStyleColor(3);
			
			ImGui::Separator();
			ImGui::Text("Libraries:");

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.57f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.57f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.57f, 0.8f, 0.8f));

			if (ImGui::Button("ImGui V:1.51"))
				App->OpenWebsite("https://github.com/ocornut/imgui/releases/tag/v1.51");

			ImGui::SameLine();
			if (ImGui::Button("Brofiler V:1.1.2"))
				App->OpenWebsite("http://brofiler.com/");

			ImGui::SameLine();
			if (ImGui::Button("Math Geo Lib V.1.5"))
				App->OpenWebsite("https://github.com/juj/MathGeoLib/releases/tag/v1.5");


			if (ImGui::Button("SDL 2.0.6"))
				App->OpenWebsite("https://www.libsdl.org/download-2.0.php");

			ImGui::SameLine();
			if (ImGui::Button("OPENGL Version 4"))
				App->OpenWebsite("https://www.opengl.org/");
		
			ImGui::SameLine();
			if (ImGui::Button("Glew V.2.1.0"))
				App->OpenWebsite("https://github.com/nigels-com/glew/releases/tag/glew-2.1.0");

			ImGui::SameLine();
			if (ImGui::Button("MMGR"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/Engine/mmgr/README.txt");

			ImGui::PopStyleColor(3);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 0.8f, 0.8f));

			ImGui::Separator();
			if (ImGui::Button("License"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/LICENSE"); 
			
			ImGui::PopStyleColor(3);
			ImGui::End();
		}

		ret = UPDATE_CONTINUE;
	}
	return ret;
}
