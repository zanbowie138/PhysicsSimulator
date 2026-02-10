#pragma once

#include <array>
#include <bitset>

#include "../GlobalTypes.h"
#include "utils/Exceptions.h"

// In charge of distributing Entity IDs and keeping track of what entities are in use
class EntityManager
{
	std::stack<Entity> availableEntities{};
	std::array<Signature, MAX_ENTITIES> signatures{};
	unsigned int livingEntityCount;

public:
	EntityManager() = default;

	Entity CreateEntity()
	{
		if (livingEntityCount >= MAX_ENTITIES) {
			throw ECSException("Entity count exceeds limit");
		}

		livingEntityCount++;
		if (availableEntities.empty())
		{
			return livingEntityCount - 1;
		}

		Entity newEntity = availableEntities.top();
		availableEntities.pop();
		return newEntity;
	}

	void DestroyEntity(const Entity entity)
	{
		if (entity >= MAX_ENTITIES) {
			throw ECSException("Entity out of range");
		}
		signatures[entity].reset();

		availableEntities.push(entity);
		livingEntityCount--;
	}

	void SetSignature(Entity entity, Signature signature)
	{
		if (entity >= MAX_ENTITIES) {
			throw ECSException("Entity out of range");
		}

		// Put this entity's signature into the array
		signatures[entity] = signature;
	}

	Signature GetSignature(Entity entity)
	{
		if (entity >= MAX_ENTITIES) {
			throw ECSException("Entity out of range");
		}

		// Get this entity's signature from the array
		return signatures[entity];
	}
};