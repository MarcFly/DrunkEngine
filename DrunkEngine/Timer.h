#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	Uint32 Read() const;

	bool IsRunning() const;


private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;

public:
	void LogTime(const char* info = nullptr)
	{
		PLOG("%s took %d ms", info, Read());
	}
};

#endif //__TIMER_H__