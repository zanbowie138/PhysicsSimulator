#pragma once
#include "../GlobalTypes.h"
#include "EntityManager.h"
#include "ComponentArray.h"

inline ComponentType NextComponentId() {
    static ComponentType next = 0;
    return next++;
}

template<typename T>
ComponentType ComponentTypeId() {
    static ComponentType id = NextComponentId();
    return id;
}

/**
 * @class ComponentManager
 * @brief Manages all the Component Arrays.
 */
class ComponentManager
{
    std::vector<std::shared_ptr<IComponentArray>> mComponentArrays;

    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        ComponentType id = ComponentTypeId<T>();
        if (id >= mComponentArrays.size())
            mComponentArrays.resize(id + 1);
        if (!mComponentArrays[id])
            mComponentArrays[id] = std::make_shared<ComponentArray<T>>();
        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[id]);
    }

public:
    template<typename T>
    ComponentType GetComponentType()
    {
        return ComponentTypeId<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        GetComponentArray<T>()->InsertEntity(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>()->RemoveEntity(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity)
    {
        for (auto const& array : mComponentArrays) {
            if (array) array->EntityDestroyed(entity);
        }
    }
};
