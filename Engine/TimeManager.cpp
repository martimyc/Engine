#include "imgui\imgui.h"
#include "Application.h"
#include "UID.h"
#include "AssetDirectory.h"
#include "ImportManager.h"
#include "TimeManager.h"

TimeManager::TimeManager(const char* name, bool start_enabled) : Module(name, start_enabled)
{
}

TimeManager::~TimeManager()
{
}

bool TimeManager::Start()
{
	show_timers = false;
	game_clock_paused = false; 
	can_pause = false; 
	real_time_clock.Start();
	game_clock.Start();
	game_clock.Pause();
	dt_timer.Start();
	frame_count = 0;
	return true;
}

UPDATE_STATUS TimeManager::Update(float dt)
{
	if (App->BeginDockWindow("Timers", &show_timers))
	{
		ImGui::Text("Real Time: %.3f", real_time_clock.ReadSec());
		ImGui::Text("Game Time: %.3f", game_clock.ReadSec());
		ImGui::Separator();
		ImGui::Text("Real Time dt: %.3f", real_time_dt);
		ImGui::Text("Game Time dt: %.3f", game_dt);
		ImGui::Separator();
		ImGui::Text("Frame Count: %.3f", (double)frame_count);
	}
	App->EndDockWindow();

	real_time = real_time_clock.ReadSec();
	game_time = game_clock.ReadSec();

	frame_count++;

	//dt
	real_time_dt = dt_timer.ReadSec();
	if(!game_clock_paused && can_pause)
		game_dt = dt_timer.ReadSec();
	else 
		game_dt = 0.0f;
	dt_timer.Start();
	//--

	return UPDATE_CONTINUE;
}

void TimeManager::PlayGame()
{
	if (!game_clock.IsRunning())
	{
		UID scene_uid = App->import_manager->SaveScene();
		game_clock.Start();
		game_clock_paused = false;
		can_pause = true;
	}
	else
	{
		game_clock.Start();
		game_clock.Pause();
		can_pause = false;
		//Load Previous Scene
		//App->import_manager->MetaLoad();
	}
}

void TimeManager::PauseGame()
{
	if (can_pause)
	{
		if (!game_clock_paused)
		{
			game_clock.Pause();
			game_clock_paused = true;
		}
		else
		{
			game_clock.UnPause();
			game_clock_paused = false;
		}
	}
}

void TimeManager::DoOneUpdate()
{
	//TODO
}

double TimeManager::GetRealTimeSinceStart() const
{
	return real_time;
}

double TimeManager::GetGameTimeSinceStart() const
{
	return game_time;
}

void TimeManager::OpenCloseTimers()
{
	show_timers = !show_timers;
}
