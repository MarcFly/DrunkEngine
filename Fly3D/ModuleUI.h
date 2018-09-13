#pragma once

#include "Module.h"
#include "Globals.h"


// Module for IMGUI use and implementation
// Includes for imgui...

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imstb_rectpack.h"
#include "imgui/imstb_textedit.h"
#include "imgui/imstb_truetype.h"

class ModuleUI : public Module
{
public:
	ModuleUI(Application* app, bool start_enabled = true);

	~ModuleUI();
	
	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

};