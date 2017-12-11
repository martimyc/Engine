#ifndef PERFTIMER
#define PERFTIMER

#include "Globals.h"

class PerfTimer
{

private:
	uint64	started_at;
	uint64	paused_at;
	bool running; 
	static uint64 frequency;

public:
	PerfTimer();

public:
	void Start();
	void Pause();
	void UnPause();
	bool IsRunning();
	double ReadMs() const;
	double ReadSec() const;
	uint64 ReadTicks() const;
	void SetTicks(uint64 ticks);
};

#endif //PERFTIMER