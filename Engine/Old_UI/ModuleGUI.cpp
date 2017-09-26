#include "imgui/imgui.h"
#include "SDL\include\SDL.h"
#include "imgui\imgui_impl_sdl.h"
#include "glut\glut.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "ModuleInput.h"
#include "UI_Element.h"
#include "UI_Test.h"
#include "UI_Rand_Test.h"
#include "UI_MainMenu.h"
#include "UI_GeometryCollisionTest.h"
#include "ModuleGUI.h"

bool ModuleGUI::CreateMainMenuBar()
{
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
			if (ImGui::MenuItem("Random number Test"))
				Activate(UI_RAND_TEST);

			if (ImGui::MenuItem("Geometry collision Test"))
				Activate(UI_GEOMETRY_COLLISION_TEST);

			if (ImGui::MenuItem("ImGui Demo"))
				Activate(UI_TEST);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Download latest"))
				App->OpenWebsite("https://github.com/martimyc/Engine/releases");		

			if (ImGui::MenuItem("Report a bug"))
				App->OpenWebsite("https://github.com/martimyc/Engine/issues");;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
		return true;
	}
	return false;
}

ModuleGUI::ModuleGUI(Application * app, bool start_enabled): Module(app, "GUI",start_enabled)
{}

ModuleGUI::~ModuleGUI()
{}

bool ModuleGUI::Init()
{
	ImGui_ImplSdlGL2_Init(App->window->window);
	
	ImGuiIO& io = ImGui::GetIO();
	//ImFont* font0 = 
	io.Fonts->AddFontDefault();
	//ImFont* font1 = io.Fonts->AddFontFromFileTTF("imgui-master/extra_fonts/Cousine-Regular.ttf", 13.0f);
	//ImFont* font2 = io.Fonts->AddFontFromFileTTF("Cousine-Regular.ttf", 13.0f);

	
	//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);						//Reduces alpha
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(50, 0, 50, 1.0f));	//Changes ALL windows bg color
	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 1.0f));		//Changes letters color
	//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(50, 0, 50, 1.0f));		// Changes bg of checkbox, radio button, plot, slider, text input

	CreateMainMenu();
	CreateTestMenu(false);
	CreateRandomNumMenu(false);
	CreateGeometryTest(false);

	return true;
}

UPDATE_STATUS ModuleGUI::Update(float dt)
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	CreateMainMenuBar();

	// UI_Elements updates loop
	for (std::vector<UI_Element*>::iterator it = ui_elements.begin(); it != ui_elements.end(); ++it)
	{
		if ((*it)->Active())
			if (!(*it)->Update())
			{
				ret = UPDATE_ERROR;
				break;
			}
	}


	return ret;
}

bool ModuleGUI::CleanUp()
{
	ImGui_ImplSdlGL2_Shutdown();
	return true;
}

void ModuleGUI::PreRender()
{
	
}

UI_Test * ModuleGUI::CreateTestMenu( bool active)
{
	UI_Test* ptr = new UI_Test(active);
	ui_elements.push_back((UI_Element*)ptr);
	return ptr;
}

UI_Rand_Test * ModuleGUI::CreateRandomNumMenu(bool active)
{
	UI_Rand_Test* ptr = new UI_Rand_Test(active);
	ui_elements.push_back((UI_Element*)ptr);
	return ptr;
}

UI_MainMenu * ModuleGUI::CreateMainMenu(bool active)
{
	UI_MainMenu* ptr = new UI_MainMenu(this, active);
	ui_elements.push_back((UI_Element*)ptr);
	return ptr;
}

UI_GeometryCollisionTest * ModuleGUI::CreateGeometryTest(bool active)
{
	UI_GeometryCollisionTest* ptr = new UI_GeometryCollisionTest(active);
	ui_elements.push_back((UI_Element*)ptr);
	return ptr;
}

void ModuleGUI::SwitchActivation(UI_TYPE element)
{
	if (!GetActive(element))
		Activate(element);
	else
		Deactivate(element);
}

void ModuleGUI::Activate(UI_TYPE element)
{
	for (std::vector<UI_Element*>::iterator it = ui_elements.begin(); it != ui_elements.end(); ++it)
		if (element == (*it)->GetType())
		{
			(*it)->Activate();
			break;
		}
}

bool ModuleGUI::GetActive(UI_TYPE element) const
{
	for (std::vector<UI_Element*>::const_iterator it = ui_elements.begin(); it != ui_elements.end(); ++it)
		if (element == (*it)->GetType())
			return (*it)->Active();
}

void ModuleGUI::Deactivate(UI_TYPE element)
{
	for (std::vector<UI_Element*>::iterator it = ui_elements.begin(); it != ui_elements.end(); ++it)
		if (element == (*it)->GetType())
		{
			(*it)->Deactivate();
			break;
		}
}



