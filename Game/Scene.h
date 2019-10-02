#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"


// <Scene Interface>
class IScene
{
private:
	std::vector<ObjectHolder<GameObject>> m_gameObjects;
	std::vector<ObjectHolder<GameObject>> m_addingObjects;
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
		for (auto& obj : m_gameObjects)
		{
			obj->Update(context);
		}
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
};