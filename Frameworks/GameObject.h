#pragma once

#include "Object.h"
#include "Component.h"
#include "Transform.h"
#include "ObjectField.h"

#include "TypeID.h"

#include <unordered_map>

class GameContext;

template <typename ObjectType>
struct hasParent {
private:
	template <typename U>
	static typename U::Bases test(int);
	template <typename>
	static void test(...);
public:
	enum { value = !std::is_void<decltype(test<ObjectType>(0))>::value };
};

// <ゲームオブジェクト>
class GameObject final : public Object, public std::enable_shared_from_this<GameObject>
{
private:
	// <コンポーネントリスト>
	std::list<ObjectHolder<Component>> m_components;
	// <追加されるコンポーネントリスト>
	std::list<ObjectHolder<Component>> m_addComponents;
	// <検索用>
	std::unordered_map<type_id_t, ObjectField<Component>> m_componentMap;
	// <オブジェクト名>
	std::wstring m_name;

public:
	// <scale, rotation, position>
	ObjectField<Transform> transform;

private:
	struct Impl
	{
		template<typename U>
		static auto AddComponentOne(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> void
		{
			components[type_id<U>()] = component;
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> std::enable_if_t<hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
			AddComponents<U::Base>(components, component);
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> std::enable_if_t<!hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
		}
	};

public:
	// <コンポーネントを追加>
	template<typename ObjectType, typename... Args>
	ObjectField<ObjectType> AddComponent(Args&& ... args)
	{
		auto component = ObjectHolder<ObjectType>::Create(std::forward<Args>(args)...);
		auto field = ObjectField<ObjectType>(component);
		component->gameObject = this;
		m_addComponents.push_back(component);
		Impl::AddComponents<ObjectType>(m_componentMap, component);
		return field;
	}

	// <コンポーネントを取得>
	template<typename ObjectType>
	ObjectField<ObjectType> GetComponent()
	{
		auto key = type_id<ObjectType>();
		if (m_componentMap.count(key) > 0)
		{
			auto& component = m_componentMap.at(key);
			if (component)
				if (!component->IsDestroyed())
					return ObjectField<ObjectType>(component);
			m_componentMap.erase(key);
		}
		return nullptr;
	}

	// <type のコンポーネントを取得>
	template<typename ObjectType>
	ObjectField<ObjectType> GetComponentFind(std::wstring type)
	{
		for (auto& component : m_components)
		{
			if (component->GetType() == type)
			{
				return ObjectField<ObjectType>(component);
			}
		}
		return nullptr;
	}

private:
	// <コンストラクタ>
	GameObject(const std::wstring & name)
		: m_name(name)
	{
		transform = AddComponent<Transform>();
	}

public:
	// <デストラクタ>
	~GameObject() = default;

	// <ゲームオブジェクト作成>
	static ObjectHolder<GameObject> Create(const std::wstring & name = L"GameObject")
	{
		return ObjectHolder<GameObject>::CreateFromUniqueSharedPtr(std::shared_ptr<GameObject>(new GameObject(name)));
	}

	// <オブジェクト名取得>
	std::wstring GetName() const override { return m_name; }
	// <オブジェクトタイプ取得>
	std::wstring GetType() const override { return L"GameObject"; }

public:
	// <初期化>
	void Initialize(GameContext & context)
	{
		for (auto& component : m_components)
			component->Initialize(context);
	}

	// <更新>
	void Update(GameContext & context)
	{
		for (auto& component : m_addComponents)
			component->Initialize(context);
		m_components.splice(m_components.end(), std::move(m_addComponents));

		for (auto& component : m_components)
			component->Update(context);

		for (auto itr = m_components.begin(); itr != m_components.end();)
		{
			auto& component = *itr;
			if (component->IsDestroyed())
			{
				component->Finalize(context);
				itr = m_components.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}

	// <描画>
	void Render(GameContext & context)
	{
		for (auto& component : m_components)
			component->Render(context);
	}

	// <破棄>
	void Finalize(GameContext & context)
	{
		for (auto& component : m_components)
			component->Finalize(context);
	}
};