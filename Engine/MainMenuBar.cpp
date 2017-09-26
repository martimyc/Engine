#include "SDL\include\SDL.h"
#include "imgui\imgui.h"
#include "Application.h"
#include "MainMenuBar.h"

ModuleMainMenuBar::ModuleMainMenuBar(Application * app, bool start_enabled) : Module(app, "Main Menu Bar", start_enabled)
{
}

ModuleMainMenuBar::~ModuleMainMenuBar()
{
}

UPDATE_STATUS ModuleMainMenuBar::Update(float dt)
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
			if (ImGui::BeginMenu("Tests"))
			{
				//if (ImGui::MenuItem("Random number Test"))
				//	Activate(UI_RAND_TEST);

				//if (ImGui::MenuItem("Geometry collision Test"))
				//	Activate(UI_GEOMETRY_COLLISION_TEST);

				//if (ImGui::MenuItem("ImGui Demo"))
				//	Activate(UI_TEST);
				
				ImGui::EndMenu();
			}

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

			ImGui::Text("%s", App->GetTitle().c_str());
			ImGui::Text("Organization: %s", App->GetOrganization().c_str());

			ImGui::Text("Authors:");

			if (ImGui::Button("Marc Latorre"))
				App->OpenWebsite("https://github.com/marclafr");

			if (ImGui::Button("Marti Majo"))
				App->OpenWebsite("https://github.com/martimyc");

			ImGui::Separator();
			ImGui::Text("Licenses:");

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 0.8f, 0.8f));

			if (ImGui::Button("ImGui V.1.51"))
				App->OpenWebsite("https://github.com/ocornut/imgui");

			ImGui::SameLine();
			if (ImGui::Button("Brofiler"))
				App->OpenWebsite("http://brofiler.com/");

			ImGui::SameLine();
			if (ImGui::Button("Math Geo Lib V.1.5"))
				App->OpenWebsite("https://github.com/juj/MathGeoLib");


			if (ImGui::Button("SDL 2.0.3"))
				App->OpenWebsite("https://www.libsdl.org/download-2.0.php");

			ImGui::SameLine();
			if (ImGui::Button("OPENGL Version 4"))
				App->OpenWebsite("https://www.opengl.org/");

			ImGui::SameLine();
			if (ImGui::Button("MMGR"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/Engine/mmgr/README.txt");

			if (ImGui::Button("License"))
				App->OpenWebsite("https://github.com/martimyc/Engine/blob/master/LICENSE"); 
			
			ImGui::PopStyleColor(3);
			ImGui::End();
		}

		ret = UPDATE_CONTINUE;
	}
	return ret;
}
