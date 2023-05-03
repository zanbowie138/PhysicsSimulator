#pragma once
#include <set>
#include "EntityManager.h"
#include "ECS.h"

extern ECS ECS;

class System 
{
public:
    std::set<Entity> mEntities;
};