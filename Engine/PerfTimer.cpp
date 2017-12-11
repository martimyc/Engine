#include "SDL2\include\SDL_timer.h"
#include "PerfTimer.h"

uint64 PerfTimer::frequency = 0;

PerfTimer::PerfTimer()
{
	if (frequency == 0)
		frequency = SDL_GetPerformanceFrequency();

	Start();
}

void PerfTimer::Start()
{
	started_at = SDL_GetPerformanceCounter();
	running = true;
}

void PerfTimer::Pause()
{
	paused_at = SDL_GetPerformanceCounter();
	running = false;
}

void PerfTimer::UnPause()
{
	started_at += SDL_GetPerformanceCounter() - paused_at;
	running = true;
}

bool PerfTimer::IsRunning()
{
	return running;
}

double PerfTimer::ReadMs() const
{
	if (running)
		return 1000.0 * (double(SDL_GetPerformanceCounter() - started_at) / double(frequency));
	else
		return 1000.0 * (double(paused_at - started_at) / double(frequency));
}

double PerfTimer::ReadSec() const
{
	if (running)
		return (double(SDL_GetPerformanceCounter() - started_at) / double(frequency));
	else
		return (double(paused_at - started_at) / double(frequency));
}

uint64 PerfTimer::ReadTicks() const
{
	if (running)
		return SDL_GetPerformanceCounter() - started_at;
	else
	{
		if (paused_at <= started_at)
			return 0;
		return uint64(paused_at - started_at);
	}
}

void PerfTimer::SetTicks(uint64 ticks)
{
	started_at = ticks;
}