#pragma once
#include <set>

#include "../GlobalTypes.h"

// A system is any functionality that iterates through a list of entities with a certain set of components
class System 
{
public:
    // Storage of all entities who use this system
    std::set<Entity> mEntities;

    // Cleans the system
    virtual void Clean() = 0;
};