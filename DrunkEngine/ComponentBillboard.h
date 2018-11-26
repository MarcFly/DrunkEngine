#ifndef _COMPONENT_BILLBOARD_
#define _COMPONENT_BILLBOARD_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Component.h"

class GameObject;
class ComponentCamera;
struct ResourceTexture;

enum BBMode
{
	BB_Error = -1,
	BB_ScreenAligned,
	BB_CamAligned,
	BB_X_AxisAligned,
	BB_Y_AxisAligned,
	BB_Z_AxisAligned
};

class ComponentBillboard : public Component
{
public:
	ComponentBillboard();
	~ComponentBillboard();

public:
	std::vector<ResourceTexture*> bbtexs;
	BBMode bb_alignment;

public:
	void SetBaseVals()
	{
		type = CT_Billboard;
		multiple = true;

		to_pop = false;
	}
};

#endif