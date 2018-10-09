#ifndef _ABOUTWINDOW_H_
#define _ABOUTWINDOW_H_

#include "Window.h"

class AboutWindow : public Window
{
public:
	AboutWindow(Application* app);
	virtual ~AboutWindow();

	void Draw() override;

public:

	ImVec4 HyperlinkColor = { 30/250.f, 139/250.f, 18/250.f, 1 };
	ImVec4 AndreuColor = { 40/250.f, 224/250.f, 244/250.f, 1};
	ImVec4 MarcColor = { 46/250.f, 249/25.f, 141/250.f, 1};

};

#endif
