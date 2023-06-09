#pragma once
#include <unordered_map>
#include <memory>

#include "GlobalTypes.h"
#include "EntityManager.h"
#include "ComponentArray.h"

// In charge of managing all the Component Arrays
class ComponentManager
{
public:
	// Creates a new array of components 
	template<typename T>
	void RegisterComponent()
	{
		// Get const char array representation of a type T. That pointer (which is just an integer) can be used as a unique key into a map of ComponentTypes.
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		// Increment the value so that the next component registered will be different
		++mNextComponentType;
	}

	// Return this component's type - used for creating signatures
	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return mComponentTypes[typeName];
	}

	// Add a component to the array for an entity
	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->InsertEntity(entity, component);
	}

	// Remove a component from the array for an entity
	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveEntity(entity);
	}

	// Get a reference to a component from the array for an entity
	template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	template <typename T>
	bool ComponentHasEntity(T type) const
	{
		const char* typeName = typeid(T).name();

		return mComponentArrays.find(typeName) != mComponentArrays.end();
	}

	void EntityDestroyed(Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type to be assigned to the next registered component - starting at 0
	ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};