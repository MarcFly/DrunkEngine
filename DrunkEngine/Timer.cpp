// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

// ---------------------------------------------
Uint32 Timer::Read() const
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}

bool Timer::IsRunning() const
{
	return running;
}

void Timer::LogTime(std::string log_nm)
{
	PLOG("%s took %d ms", log_nm.c_str(), Read());
}

void Timer::LogNStart(std::string log_nm)
{
	PLOG("%s took %d ms", log_nm.c_str(), Read());
	Start();
}

///=========================================================================================
// PrecTimer class
PrecTimer::PrecTimer()
{
	QueryPerformanceCounter(&frequency);
	Start();
}

// ---------------------------------------------
void PrecTimer::Start()
{
	started_at = frequency;
	QueryPerformanceCounter(&started_at);
	running = true;
}

// ---------------------------------------------
void PrecTimer::Stop()
{
	running = false;
	stopped_at = frequency;
	QueryPerformanceCounter(&stopped_at);
}

// ---------------------------------------------
uint PrecTimer::Read()
{
	curr = frequency;
	QueryPerformanceCounter(&curr);

	if (running == true)
	{
		return  curr.QuadPart - started_at.QuadPart;
	}
	else
	{
		return stopped_at.QuadPart - started_at.QuadPart;
	}
}

void PrecTimer::LogTime(const char* log_nm)
{
	PLOG("%s took %d us", log_nm, Read());
}

void PrecTimer::LogNStart(const char* log_nm)
{
	PLOG("%s took %d us", log_nm, Read());
	Start();
}
