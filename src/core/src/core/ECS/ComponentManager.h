#pragma once
#include <unordered_map>
#include <memory>

#include "../GlobalTypes.h"
#include "EntityManager.h"
#include "ComponentArray.h"

/**
 * @class ComponentManager
 * @brief Manages all the Component Arrays.
 */
class ComponentManager
{
    std::unordered_map<const char*, ComponentType> mComponentTypes{};
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
    ComponentType mNextComponentType{};

    /**
     * @brief Gets the statically casted pointer to the ComponentArray of type T.
     * @tparam T Component type.
     * @return std::shared_ptr<ComponentArray<T>> The statically casted pointer to the ComponentArray of type T.
     */
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }
public:
    /**
     * @brief Creates a new array of components.
     * @tparam T Component type.
     */
    template<typename T>
    void RegisterComponent()
    {
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

        mComponentTypes.insert({ typeName, mNextComponentType });

        mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

        ++mNextComponentType;
    }

    /**
     * @brief Returns the type of the component. Used for creating signatures.
     * @tparam T Component type.
     * @return ComponentType The type of the component.
     */
    template<typename T>
    ComponentType GetComponentType()
    {
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

        return mComponentTypes[typeName];
    }

    /**
     * @brief Adds a component to the array for an entity.
     * @tparam T Component type.
     * @param entity The entity to which the component is added.
     * @param component The component to be added.
     */
    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        GetComponentArray<T>()->InsertEntity(entity, component);
    }

    /**
     * @brief Removes a component from the array for an entity.
     * @tparam T Component type.
     * @param entity The entity from which the component is removed.
     */
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>()->RemoveEntity(entity);
    }

    /**
     * @brief Gets a reference to a component from the array for an entity.
     * @tparam T Component type.
     * @param entity The entity for which the component is retrieved.
     * @return T& A reference to the component.
     */
    template<typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetData(entity);
    }

    /**
     * @brief Checks if a component type has an entity.
     * @tparam T Component type.
     * @param type The type of the component.
     * @return bool True if the component type has an entity, false otherwise.
     */
    template <typename T>
    bool ComponentHasEntity(T type) const
    {
        const char* typeName = typeid(T).name();

        return mComponentArrays.find(typeName) != mComponentArrays.end();
    }

    /**
     * @brief Notifies each component array that an entity has been destroyed.
     * @param entity The entity that has been destroyed.
     */
    void EntityDestroyed(Entity entity)
    {
        for (auto const& pair : mComponentArrays)
        {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }
};
