#ifndef _OPTIONSWINDOW_H_
#define _OPTIONSWINDOW_H_

#include "Window.h"
#include "Application.h"
#include <vector>
#include <Psapi.h>

class OptionsWindow : public Window
{
public:
	OptionsWindow();
	virtual ~OptionsWindow();

	void Draw() override;

	void CheckEnableDisableOpenGL();

	void CheckInputChange();

	void Application();
	void WindowOptions();
	void RenderOptions();
	void Camera();
	void TextureParameters();
	void InputOptions();
	void HardwareInfo();
	void LibrariesUsed();
	void TimeViewer();

public:

	float fps;
	float max_fps;

	Timer frame_read_time;
	Timer repeated_key_time;

	int fps_h_size = 25;
	int dt_h_size = 60;
	std::vector<float> fps_history;
	std::vector<float> dt_history;

	int input_change;
	bool key_repeated;

	ImVec4 HyperlinkColor = { 30 / 250.f, 139 / 250.f, 18 / 250.f, 1 };

private:
	float realdt, gamedt, gametime, realtime;
	Timer dt_read_time;

private:
	bool change_camera_controls;

	// System Info Read
	bool read_once;

	PROCESS_MEMORY_COUNTERS mem = { 0 };
	Timer info_read_time;

	SDL_version sdl_ver;
	int cpu_count;
	int cache;
	float SystemRam;
	std::string gpu_vendor;
	std::string GPU;
	std::string Drivers;

	GLint budget;
	GLint available;
	GLuint used_mem;

	int gl_major, gl_minor;
	int devil_a, devil_b, devil_c;
	int ai_major, ai_minor, ai_revision;

private:
	void GetFixedSystemData();
	void GetRuntimeSystemData();
};

#endif
