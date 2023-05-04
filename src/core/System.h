#pragma once
#include <set>
#include "EntityManager.h"
#include "ECS.h"

extern ECSController ecsController;

// A system is any functionality that iterates through a list of entities with a certain set of components
class System 
{
public:
    // Storage of all entities who use this system
    std::set<Entity> mEntities;
};