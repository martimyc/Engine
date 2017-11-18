#ifndef TIMER
#define TIMER

#include "Globals.h"

class Timer
{
private:
	uint32	started_at; 

public:
	Timer();

public:
	void Start();
	uint32 Read() const;
	float ReadSec() const;
	void SetTicks(uint32 ticks);
};

#endif //TIMER