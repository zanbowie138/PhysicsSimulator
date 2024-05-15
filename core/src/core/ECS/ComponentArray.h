#pragma once
#include "EntityManager.h"

/**
 * @brief Interface for a component array
 */
class IComponentArray
{
public:
    /**
     * @brief Virtual function to be overridden that handles the destruction of an entity
     * @param entity The entity to be destroyed
     */
    virtual void EntityDestroyed(Entity entity) = 0;
};

/**
 * @brief Template class for a component array
 * @tparam T The type of the component
 */
template <typename T>
class ComponentArray : public IComponentArray
{
    // Stores the individual components for each entity
    std::array<T, MAX_ENTITIES> mComponentArray;

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, size_t> mEntityToIndexMap;

    // Map from an array index to an entity ID.
    std::unordered_map<size_t, Entity> mIndexToEntityMap;

    // Total size of valid entries in the array.
    size_t mSize;

public:
    ComponentArray() {
		mSize = 0;
	}

    /**
     * @brief Inserts a new entity into the component array
     * @param entity The entity to be inserted
     * @param component The component to be associated with the entity
     */
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

    /**
     * @brief Removes an entity from the component array
     * @param entity The entity to be removed
     */
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

    /**
     * @brief Retrieves the data associated with an entity
     * @param entity The entity whose data is to be retrieved
     * @return A reference to the entity's component
     */
    T& GetData(Entity entity)
    {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

        // Return a reference to the entity's component
        return mComponentArray[mEntityToIndexMap[entity]];
    }

    /**
     * @brief Handles the destruction of an entity
     * @param entity The entity to be destroyed
     */
    void EntityDestroyed(Entity entity) override
    {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
        {
            // Remove the entity's component if it existed
            RemoveEntity(entity);
        }
    }

    /**
     * @brief Checks if the component array contains a specific entity
     * @param entity The entity to be checked
     * @return True if the entity exists in the component array, false otherwise
     */
    bool HasEntity(Entity entity) const
    {
        return mEntityToIndexMap.count(entity);
    }
};