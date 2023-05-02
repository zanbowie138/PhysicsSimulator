#pragma once
#include "BoundingBox.h"
class Collidable
{
public:
	BoundingBox boundingBox;

	virtual void UpdateBoundingBox() = 0;
};