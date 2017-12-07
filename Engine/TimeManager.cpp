#include "imgui\imgui.h"
#include "TimeManager.h"

TimeManager::TimeManager(const char* name, bool start_enabled) : Module(name, start_enabled)
{
}

TimeManager::~TimeManager()
{
}

bool TimeManager::Start()
{
	show_timers = true;
	game_clock_paused = false; 
	can_pause = false; 
	real_time_clock.Start();
	game_clock.Start();
	game_clock.Pause();
	return true;
}

UPDATE_STATUS TimeManager::Update(float dt)
{
	if (show_timers)
	{
		ImGui::Begin("Timers");
		ImGui::Text("Real Time: %.3f", real_time_clock.ReadSec());
		ImGui::Text("Game Time: %.3f", game_clock.ReadSec());

		ImGui::Asset("Test", ImVec2(100, 100), false, (void*)5);

		ImGui::End();
	}

	real_time_since_start = real_time_clock.ReadSec();
	game_time_since_start = game_clock.ReadSec();

	return UPDATE_CONTINUE;
}

void TimeManager::PlayGame()
{
	if (!game_clock.IsRunning())
	{
		game_clock.Start();
		game_clock_paused = false;
		can_pause = true;
	}
	else
	{
		game_clock.Start();
		game_clock.Pause();
		can_pause = false;
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

const float TimeManager::GetRealTimeSinceStart() const
{
	return real_time_since_start;
}

const float TimeManager::GetGameTimeSinceStart() const
{
	return game_time_since_start;
}
