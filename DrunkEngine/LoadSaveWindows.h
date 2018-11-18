#ifndef _LOADSAVE_WINDOWS_
#define _LOADSAVE_WINDOWS_

#include "Window.h"

class SaveSceneWindow : public Window
{
public:
	SaveSceneWindow() : Window("Save Scene") {};
	virtual ~SaveSceneWindow() {};

	void Draw() override;

public:

};

class LoadSceneWindow : public Window
{
public:
	LoadSceneWindow() : Window("Load Scene") {};
	virtual ~LoadSceneWindow() {};

	void Draw() override;

public:

};

class ImportWindow : public Window
{
public:
	ImportWindow() : Window("Import") {};
	virtual ~ImportWindow() {};

	void Draw() override;

public:

};
#endif