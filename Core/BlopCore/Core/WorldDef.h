#pragma once

#include "Graphics/Camera.h"

class DLLTYPE CWorld
{
public:
	CCamera *Camera();
};
typedef CWorld *PWorld;

PWorld DLLTYPE World();
void InitWorld();