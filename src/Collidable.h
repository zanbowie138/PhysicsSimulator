#pragma once
#include "BoundingBox.h"
class Collidable
{
public:
	Collidable();

	BoundingBox box;

	virtual void updateBoundingBox();
};