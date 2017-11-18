#ifndef TIME_MANAGER
#define TIME_MANAGER

#include "Module.h"
#include "Timer.h"

class TimeManager : public Module
{
	/*	TODO:
○ Frame_Count: app graphics frames since game start
○ Time: second since game start (Game Clock)
○ Time_Scale: scale at which time is passing (Game Clock)
○ Delta_Time: last frame time expressed in seconds (Game Clock)
○ Real_Time_Since_Startup: seconds since game start (Real Time Clock)
○ Real_Time_Delta_Time: last frame time expressed in seconds (Real Time Clock)
	*/
private:
	Timer real_time_clock;
	Timer game_clock;

public:
	TimeManager(const char* name, bool start_enabled = true);
	~TimeManager();
};

#endif // !TIME_MANAGER