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
	Utils::Logger logger;

	World(const std::string& loggingFilePath):
	logger(loggingFilePath, true)
	{
		// Create pointers to each manager
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();

		LOG(logger, LOG_INFO) << "World created.\n";
	}

	// Entity methods
	Entity CreateEntity() const
	{
		LOG(logger, LOG_INFO) << "Creating new entity.\n";
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity) const
	{
		LOG(logger, LOG_INFO) << "Destroying entity: " << entity << ".\n";
		mEntityManager->DestroyEntity(entity);
		mComponentManager->EntityDestroyed(entity);
		mSystemManager->EntityDestroyed(entity);
	}

	// Component methods
	template<typename T>
	void RegisterComponent() const
	{
		LOG(logger, LOG_INFO) << "Registering component.\n";
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		LOG(logger, LOG_INFO) << "Adding component to entity: " << entity << ".\n";
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity) const
	{
		LOG(logger, LOG_INFO) << "Removing component from entity: " << entity << ".\n";
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
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		LOG(logger, LOG_INFO) << "Registering system.\n";
		return mSystemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature) const
	{
		LOG(logger, LOG_INFO) << "Setting system signature.\n";
		mSystemManager->SetSignature<T>(signature);
	}

	void Clean() const
	{
		mSystemManager->CleanSystems();
	}
};
