#include "CollisionEngine.h"

CollisionEngine* CollisionEngine::instance;

CollisionEngine * CollisionEngine::GetInstance()
{
	if (instance == nullptr)
		instance = new CollisionEngine();
	return instance;
}

CollisionEngine::CollisionEngine()
{
	colliders = std::map<std::string, Collider*>();
}

void CollisionEngine::SetActive(bool isActive)
{
	this->isActive = isActive;
}

void CollisionEngine::CollisionLoop()
{
#ifdef MULTITHREADED
	while (isActive)
#else
	if (isActive)
#endif
	{
		int size = colliders.size();
		bool *collidersStates = new bool[size];
		int i = 0, j = 0;
		for (auto collider : colliders)
		{
			collider.second->CollidedObjects->clear();
			collidersStates[i] = false;
			i++;
		}
		i = 0;
		for (auto collider : colliders)
		{
			for (auto otherCollider : colliders)
			{
				if (i == j || collidersStates[j]) 
				{
					j++;
					continue;
				}
				if (collider.second->CheckCollision(otherCollider.second))
				{
					collider.second->CollidedObjects->push_back(otherCollider.first);
					otherCollider.second->CollidedObjects->push_back(collider.first);
				}
				
				j++;
			}
			collidersStates[i] = true;
			i++;
		}
	}
}

void CollisionEngine::AddCollider(std::string objectName, Collider * collider)
{
	std::pair kv = std::make_pair(objectName, collider);
	colliders.insert(kv);
}

void CollisionEngine::RemoveCollider(std::string objectName)
{
	colliders.erase(objectName);
}

CollisionEngine::~CollisionEngine()
{
	isActive = false;
}
