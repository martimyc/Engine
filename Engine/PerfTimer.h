#ifndef PERFTIMER
#define PERFTIMER

#include "Globals.h"

class PerfTimer
{

private:
	uint64	started_at;
	static uint64 frequency;

public:
	PerfTimer();

public:
	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;
};

#endif //PERFTIMER