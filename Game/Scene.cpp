#include "Scene.h"

ObjectField<GameObject> IScene::Add(ObjectHolder<GameObject>&& obj)
{
	auto field = ObjectField<GameObject>(obj);
	m_addingObjects.emplace_back(std::move(obj));
	m_objectMap.insert(std::make_pair(field->GetName(), field));
	return field;
}

ObjectField<GameObject> IScene::Find(const std::wstring& key)
{
	if (m_objectMap.count(key) > 0)
	{
		for (auto itr = m_objectMap.find(key); itr != m_objectMap.end();)
		{
			auto& component = (*itr).second;
			if (component && !component->IsDestroyed())
				return component;
			itr = m_objectMap.erase(itr);
		}
	}
	return nullptr;
}

std::vector<ObjectField<GameObject>> IScene::FindAll(const std::wstring& key)
{
	std::vector<ObjectField<GameObject>> result;
	if (m_objectMap.count(key) > 0)
	{
		for (auto itr = m_objectMap.find(key); itr != m_objectMap.end();)
		{
			auto& component = (*itr).second;
			if (component && !component->IsDestroyed())
			{
				result.push_back(component);
				++itr;
			}
			else
			{
				itr = m_objectMap.erase(itr);
			}
		}
	}
	return result;
}

ObjectField<GameObject> IScene::AddGameObject(const std::wstring& objname)
{
	return Add(GameObject::Create(objname));
}