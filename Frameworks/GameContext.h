#pragma once

#include <cassert>
#include <memory>

#include <unordered_map>

#include "../DeviceResources.h"
#include "../StepTimer.h"

#include "TypeID.h"

// <ServiceLocator �p�^�[��>
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

		template <typename T>
		struct ContextHolder : public IContextHolder
		{
			virtual T* Get() const = 0;
		};

		template <typename T>
		struct ContextUniquePtrHolder : public ContextHolder<T>
		{
			std::unique_ptr<T> m_context;
			ContextUniquePtrHolder(std::unique_ptr<T>&& context) : m_context(std::move(context)) {}
			T* Get() const override { return m_context.get(); }
		};

		template <typename T>
		struct ContextSharedPtrHolder : public ContextHolder<T>
		{
			std::shared_ptr<T> m_context;
			ContextSharedPtrHolder(const std::shared_ptr<T>& context) : m_context(context) {}
			T* Get() const override { return m_context.get(); }
		};

		template <typename T>
		struct ContextWeakPtrHolder : public ContextHolder<T>
		{
			std::weak_ptr<T> m_context;
			ContextWeakPtrHolder(const std::weak_ptr<T>& context) : m_context(context) {}
			T* Get() const override { if (auto p = m_context.lock()) return p.get(); else return nullptr; }
		};

		template <typename T>
		struct ContextRefHolder : public ContextHolder<T>
		{
			std::reference_wrapper<T> m_context;
			ContextRefHolder(const std::reference_wrapper<T>& context) : m_context(context) {}
			ContextRefHolder(T& context) : m_context(std::ref(context)) {}
			T* Get() const override { return &m_context; }
		};

		template <typename T>
		struct ContextPtrHolder : public ContextHolder<T>
		{
			T* m_context;
			ContextPtrHolder(T* context) : m_context(context) {}
			T* Get() const override { return m_context; }
		};

		std::unordered_map<type_id_t, std::unique_ptr<IContextHolder>> m_contexts;

		template <typename T> T* Get()
		{
			auto key = type_id<T>();
			if (m_contexts.count(key) > 0)
			{
				auto holder = m_contexts.at(key).get();
				return dynamic_cast<ContextHolder<T>*>(holder)->Get();
			}
			return nullptr;
		}

		template <typename T> void Register(std::unique_ptr<T>&& context) { m_contexts[type_id<T>()] = std::make_unique<ContextUniquePtrHolder<T>>(std::move(context)); }
		template <typename T> void Register(const std::shared_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextSharedPtrHolder<T>>(context); }
		template <typename T> void Register(const std::weak_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextWeakPtrHolder<T>>(context); }
		template <typename T> void Register(const std::reference_wrapper<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template <typename T> void Register(T& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template <typename T> void Register(T* context) { m_contexts[type_id<T>()] = std::make_unique<ContextPtrHolder<T>>(context); }
	};

	ContextLocator m_locator;

public:
	virtual GameContext* GetParent()
	{
		return nullptr;
	}

	template <typename T>
	T* GetPtr()
	{
		auto context = m_locator.Get<T>();
		if (context == nullptr)
		{
			auto parent = GetParent();
			if (parent != nullptr)
				context = parent->GetPtr<T>();
		}
		return context;
	}

	template <typename T>
	T& Get()
	{
		auto context = GetPtr<T>();
		if (context == nullptr)
			throw std::exception("missing service.");
		return *context;
	}

	template <typename T>
	void Register(T&& context)
	{
		m_locator.Register(std::forward<T>(context));
	}
};