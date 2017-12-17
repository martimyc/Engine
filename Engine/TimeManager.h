﻿#ifndef TIME_MANAGER
#define TIME_MANAGER

#include "UID.h"
#include "Module.h"
#include "PerfTimer.h"

class TimeManager : public Module
{
private:
	PerfTimer real_time_clock;
	PerfTimer game_clock;

	PerfTimer dt_timer;

	uint64 frame_count = 0.0000;

	double game_time = 0.0000;
	float game_time_scale = 0.0000;
	float game_dt = 0.0000;

	double real_time = 0.0000;
	float real_time_dt = 0.0000;

	bool game_clock_paused = true;
	bool can_pause = false;
	bool show_timers;

	UID scene_uid;

public:
	TimeManager(const char* name, bool start_enabled = true);
	~TimeManager();

	bool Start();

	UPDATE_STATUS Update(float dt);

	void PlayGame();
	void PauseGame();
	void DoOneUpdate();

	//Getters
	double GetRealTimeSinceStart() const;
	double GetGameTimeSinceStart() const;

	void OpenCloseTimers();
};

#endif // !TIME_MANAGER