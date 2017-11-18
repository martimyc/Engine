#include "SDL2\include\SDL_timer.h"
#include "Timer.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	started_at = SDL_GetTicks();
}

uint32 Timer::Read() const
{
	return SDL_GetTicks() - started_at;
}

float Timer::ReadSec() const
{
	return float(SDL_GetTicks() - started_at) / 1000.0f;
}

void Timer::SetTicks(uint32 ticks)
{
	started_at = ticks;
}