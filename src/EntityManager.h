#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <stack>

constexpr unsigned int MAX_ENTITIES = 500;
constexpr unsigned int MAX_COMPONENTS = 10;

// EntityID
using Entity = unsigned int;

// Component ID
using ComponentType = uint8_t;

// Basically an array of bools identifying what components are being used
using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager
{
public:
	EntityManager(){}
	~EntityManager();

	Entity CreateEntity()
	{
		assert(livingEntityCount < MAX_ENTITIES && "Entity count exceeds limit");

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
		assert(entity < MAX_ENTITIES && "Entity out of range.");
		signatures[entity].reset();

		availableEntities.push(entity);
		livingEntityCount--;
	}

	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Put this entity's signature into the array
		signatures[entity] = signature;
	}

	Signature GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Get this entity's signature from the array
		return signatures[entity];
	}

private:
	std::stack<Entity> availableEntities{};
	std::array<Signature, MAX_ENTITIES> signatures{};
	unsigned int livingEntityCount{};
};