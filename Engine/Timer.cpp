#include "SDL2\include\SDL_timer.h"
#include "Timer.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	started_at = SDL_GetTicks();
	running = true;
}

void Timer::Pause()
{
	paused_at = SDL_GetTicks() - started_at;
	running = false;
}

void Timer::UnPause()
{
	started_at = SDL_GetTicks() - paused_at;
	running = true;
}

bool Timer::IsRunning()
{
	return running;
}

uint32 Timer::Read() const
{
	if (running)
		return SDL_GetTicks() - started_at;
	else
		return paused_at - started_at;
}

float Timer::ReadSec() const
{
	if (running)
		return float(SDL_GetTicks() - started_at) / 1000.0f;
	else
		return (paused_at - started_at) / 1000.0f;
}

void Timer::SetTicks(uint32 ticks)
{
	started_at = ticks;
}