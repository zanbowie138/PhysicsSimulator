#pragma once
#include <utils/Logger.h>

#include "core/ECS/ComponentManager.h"
#include "core/ECS/SystemManager.h"

class World
{
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;

public:
	World()
	{
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}

	// Entity methods
	Entity CreateEntity() const
	{
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity) const
	{
		mEntityManager->DestroyEntity(entity);
		mComponentManager->EntityDestroyed(entity);
		mSystemManager->EntityDestroyed(entity);
	}

	void ClearAllEntities() const
	{
		// Get all living entities and destroy them
		for (Entity e = 0; e < MAX_ENTITIES; e++) {
			// Check if entity is alive by checking if it has any components
			if (mEntityManager->GetSignature(e).any()) {
				DestroyEntity(e);
			}
		}
		LOG(LOG_INFO) << "Cleared all entities\n";
	}

	// Component methods
	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity) const
	{
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity) const
	{
		return mComponentManager->GetComponent<T>(entity);
	}



	template<typename T>
	ComponentType GetComponentType() const
	{
		return mComponentManager->GetComponentType<T>();
	}

	Signature GetEntitySignature(Entity entity) const 
	{
		return mEntityManager->GetSignature(entity);
	}


	// System methods
	template<typename TSystem, typename... TComponents>
	std::shared_ptr<TSystem> RegisterSystem()
	{
		Signature signature;
		(signature.set(mComponentManager->GetComponentType<TComponents>()), ...);
		return mSystemManager->RegisterSystem<TSystem>(signature);
	}

	void Clean() const
	{
		mSystemManager->CleanSystems();
	}
};