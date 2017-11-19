#ifndef TIMER
#define TIMER

#include "Globals.h"

class Timer
{
private:
	uint32	started_at;
	uint32	paused_at;
	bool running;

public:
	Timer();

public:
	void Start();
	void Pause();
	void UnPause();
	bool IsRunning();
	uint32 Read() const;
	float ReadSec() const;
	void SetTicks(uint32 ticks);
};

#endif //TIMER