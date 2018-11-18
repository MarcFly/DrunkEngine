#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

#define FREQUENCY LARGE_INTEGER freq

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();
	void Resume();

	Uint32 Read() const;

	bool IsRunning() const;


	void LogTime(std::string log_nm);
	void LogNStart(std::string log_nm);

private:

	bool	running;

	Uint32	started_at;
	Uint32	stopped_at;
};

class PrecTimer
{
public:

	// Constructor
	PrecTimer();

	void Start();
	void Stop();

	uint Read();

	void LogTime(const char* log_nm);
	void LogNStart(const char* log_nm);

private:

	bool	running;
	LARGE_INTEGER	frequency;
	LARGE_INTEGER	curr;
	LARGE_INTEGER	started_at;
	LARGE_INTEGER	stopped_at;
};

#endif //__TIMER_H__