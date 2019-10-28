#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"

#include "Collider.h"

// <Scene Interface>
class IScene
{
private:
	std::vector<ObjectHolder<GameObject>> m_gameObjects;
	std::vector<ObjectHolder<GameObject>> m_addingObjects;
	std::vector<GameObject*> m_killObjects;
	std::unordered_multimap<std::wstring, ObjectField<GameObject>> m_objectMap;

public:
	IScene() = default;
	virtual ~IScene() = default;

	ObjectField<GameObject> Add(ObjectHolder<GameObject>&& obj);
	ObjectField<GameObject> Find(const std::wstring& name);
	std::vector<ObjectField<GameObject>> FindAll(const std::wstring& name);
	ObjectField<GameObject> AddGameObject(const std::wstring& objname = L"GameObject");
	std::vector<ObjectHolder<GameObject>>& GetObjects() { return m_gameObjects; }

	virtual void Initialize(GameContext& context) = 0;
	virtual void Update(GameContext& context) = 0;
	virtual void Render(GameContext& context) = 0;
	virtual void Finalize(GameContext& context) = 0;

	void ObjectDestroy(GameObject& object)
	{
		m_killObjects.push_back(&object);
	}

protected:
	void InitializeGameObject(GameContext& context)
	{
		for (auto& obj : m_addingObjects)
		{
			m_gameObjects.push_back(std::move(obj));
		}
		m_addingObjects.clear();
		for (auto& obj : m_gameObjects)
		{
			obj->Initialize(context);
		}
	}
	void UpdateGameObject(GameContext& context)
	{
		for (auto& obj : m_addingObjects)
		{
			obj->Initialize(context);
			m_gameObjects.push_back(std::move(obj));
		}
		m_addingObjects.clear();
		for (auto& obj : m_gameObjects)
		{
			obj->Update(context);
		}
		this->CheckCollision();
		KillObjectsDestroy(context);
	}
	void RenderGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Render(context);
		}
	}
	void FinalizeGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Finalize(context);
		}
	}

	void CheckCollision()
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			auto& obj = m_gameObjects[i];
			auto col = obj->GetComponent<Collider>();
			// <Collider を持っているかチェック>
			if (col)
			{
				for (int j = (i + 1); j < m_gameObjects.size(); j++)
				{
					auto& obj2 = m_gameObjects[j];
					auto col2 = obj2->GetComponent<Collider>();
					if (col2)
					{
						col->OnHitCollision(*col2);
					}
				}
			}
		}
	}

	void KillObjectsDestroy(GameContext& context)
	{
		for (auto& obj : m_killObjects)
		{
			obj->Finalize(context);
		}

		int i = 0;
		for (auto& kill : m_killObjects)
		{
			for (auto& obj : m_gameObjects)
			{
				if (obj.GetWeakPtr().lock().get() == kill)
				{
					m_gameObjects.erase(m_gameObjects.begin() + i);
					break;
				}
				i++;
			}
			i = 0;
		}
		m_killObjects.clear();
	}
};