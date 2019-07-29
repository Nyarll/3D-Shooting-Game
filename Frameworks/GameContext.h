#pragma once

#include <cassert>
#include <memory>

#include <unordered_map>

#include "../DeviceResources.h"
#include "../StepTimer.h"

#include "TypeID.h"

// <ServiceLocator ƒpƒ^[ƒ“>
class GameContext
{
public:
	GameContext() = default;
	virtual ~GameContext() = default;

private:
	struct ContextLocator
	{
		struct IContextHolder
		{
			virtual ~IContextHolder() = default;
		};

		template <typename ObjectType>
		struct ContextHolder : public IContextHolder
		{
			virtual ObjectType* Get() const = 0;
		};

		template <typename ObjectType>
		struct ContextUniquePtrHolder : public ContextHolder<ObjectType>
		{
			std::unique_ptr<ObjectType> m_context;
			ContextUniquePtrHolder(std::unique_ptr<ObjectType>&& context) : m_context(std::move(context)) {}
			ObjectType* Get() const override { return m_context.get(); }
		};

		template <typename ObjectType>
		struct ContextSharedPtrHolder : public ContextHolder<ObjectType>
		{
			std::shared_ptr<ObjectType> m_context;
			ContextSharedPtrHolder(const std::shared_ptr<ObjectType>& context) : m_context(context) {}
			ObjectType* Get() const override { return m_context.get(); }
		};

		template <typename ObjectType>
		struct ContextWeakPtrHolder : public ContextHolder<ObjectType>
		{
			std::weak_ptr<ObjectType> m_context;
			ContextWeakPtrHolder(const std::weak_ptr<ObjectType>& context) : m_context(context) {}
			ObjectType* Get() const override { if (auto p = m_context.lock()) return p.get(); else return nullptr; }
		};

		template <typename ObjectType>
		struct ContextRefHolder : public ContextHolder<ObjectType>
		{
			std::reference_wrapper<ObjectType> m_context;
			ContextRefHolder(const std::reference_wrapper<ObjectType>& context) : m_context(context) {}
			ContextRefHolder(ObjectType& context) : m_context(std::ref(context)) {}
			ObjectType* Get() const override { return &m_context; }
		};

		template <typename ObjectType>
		struct ContextPtrHolder : public ContextHolder<ObjectType>
		{
			ObjectType* m_context;
			ContextPtrHolder(ObjectType* context) : m_context(context) {}
			ObjectType* Get() const override { return m_context; }
		};

		//
#define GAMECONTEXT_USE_MAP
#ifdef GAMECONTEXT_USE_MAP
		template<typename T>
		struct sparse_set
		{
			std::vector<std::unique_ptr<T>> reserve;

			bool managed(type_id_t id)
			{
				return id < reserve.size() && reserve[id];
			}

			void assure(type_id_t id)
			{
				if (!(id < reserve.size()))
					reserve.resize(id + 1);
			}

			void put(type_id_t id, std::unique_ptr<T>&& value)
			{
				assure(id);
				reserve[id] = std::move(value);
			}

			std::unique_ptr<T>& get(type_id_t id)
			{
				if (managed(id))
					return reserve[id];
				return nullptr;
			}

			std::unique_ptr<T>& getfast(type_id_t id)
			{
				return reserve[id];
			}
		};
		sparse_set<IContextHolder> m_contexts;

		template <typename ObjectType> ObjectType* Get()
		{
			auto key = type_id<ObjectType>();
			if (m_contexts.managed(key))
				return static_cast<ContextHolder<ObjectType>*>(m_contexts.getfast(key).get())->Get();
			return nullptr;
		}

		template<typename T> void Register(std::unique_ptr<T>&& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextUniquePtrHolder<T>>(std::move(context))); }
		template<typename T> void Register(const std::shared_ptr<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextSharedPtrHolder<T>>(context)); }
		template<typename T> void Register(const std::weak_ptr<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextWeakPtrHolder<T>>(context)); }
		template<typename T> void Register(const std::reference_wrapper<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextRefHolder<T>>(context)); }
		template<typename T> void Register(T& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextRefHolder<T>>(context)); }
		template<typename T> void Register(T* context) { m_contexts.put(type_id<T>(), std::make_unique<ContextPtrHolder<T>>(context)); }
#else
		std::unordered_map<Type_ID, std::unique_ptr<IContextHolder>> m_contexts;

		template <typename ObjectType> 
		ObjectType* Get()
		{
			auto key = type_id<ObjectType>();
			if (m_contexts.count(key) > 0)
			{
				auto holder = m_contexts.at(key).get();
				return dynamic_cast<ContextHolder<ObjectType>*>(holder)->Get();
			}
			return nullptr;
		}

		template <typename ObjectType> void Register(std::unique_ptr<ObjectType>&& context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextUniquePtrHolder<ObjectType>>(std::move(context)); }
		template <typename ObjectType> void Register(const std::shared_ptr<ObjectType>& context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextSharedPtrHolder<ObjectType>>(context); }
		template <typename ObjectType> void Register(const std::weak_ptr<ObjectType>& context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextWeakPtrHolder<ObjectType>>(context); }
		template <typename ObjectType> void Register(const std::reference_wrapper<ObjectType>& context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextRefHolder<ObjectType>>(context); }
		template <typename ObjectType> void Register(ObjectType& context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextRefHolder<ObjectType>>(context); }
		template <typename ObjectType> void Register(ObjectType* context) { m_contexts[type_id<ObjectType>()] = std::make_unique<ContextPtrHolder<ObjectType>>(context); }
#endif
	};

	ContextLocator m_locator;

public:
	virtual GameContext* GetParent()
	{
		return nullptr;
	}

	template <typename ObjectType>
	ObjectType* GetPtr()
	{
		auto context = m_locator.Get<ObjectType>();
		if (context == nullptr)
		{
			auto parent = GetParent();
			if (parent != nullptr)
				context = parent->GetPtr<ObjectType>();
		}
		return context;
	}

	template <typename ObjectType>
	ObjectType& Get()
	{
		auto context = GetPtr<ObjectType>();
		if (context == nullptr)
			throw std::exception("missing service.");
		return *context;
	}

	template <typename ObjectType>
	void Register(ObjectType&& context)
	{
		m_locator.Register(std::forward<ObjectType>(context));
	}
};