#include "TimeManager.h"

TimeManager::TimeManager(const char* name, bool start_enabled) : Module(name, start_enabled)
{
}

TimeManager::~TimeManager()
{
}

bool TimeManager::Start()
{
	real_time_clock.Start();
	return true;
}

void TimeManager::PlayGame()
{
	if (!game_clock.IsRunning())
		game_clock.Start();
	else
	{
		game_clock.Start();
		game_clock.Pause();
	}
}

void TimeManager::PauseGame()
{
	if (!game_clock.IsRunning())
		game_clock.Pause();
	else
		game_clock.UnPause();
}

void TimeManager::DoOneUpdate()
{
	//TODO
}