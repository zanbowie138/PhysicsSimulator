#pragma once
#include "EntityManager.h"

class IComponentArray
{
public:
	virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray
{
public:
	void InsertEntity(Entity entity, T component)
	{
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		mSize++;
	}

	void RemoveEntity(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		mSize--;
	}

	T& GetData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

		// Return a reference to the entity's component
		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			// Remove the entity's component if it existed
			RemoveEntity(entity);
		}
	}

	bool HasEntity(Entity entity) const 
	{
		return mEntityToIndexMap.count(entity);
	}
private:
	// Stores the individual components for each entity
	std::array<T, MAX_ENTITIES> mComponentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> mEntityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, Entity> mIndexToEntityMap;

	// Total size of valid entries in the array.
	size_t mSize;
};