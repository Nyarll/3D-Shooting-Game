#pragma once

#include <memory>

// 
template <typename ObjectType>
class ObjectHolder
{
private:
	std::shared_ptr<ObjectType> m_object;
	ObjectHolder(const std::shared_ptr<ObjectType>& object) :m_object(object) {}

public:
	constexpr ObjectHolder() noexcept(nullptr) {};
	constexpr ObjectHolder(nullptr_t) noexcept {};

	ObjectHolder(const ObjectHolder&) = delete;
	ObjectHolder& operator=(const ObjectHolder&) = delete;
	ObjectHolder(ObjectHolder&&) noexcept = default;
	ObjectHolder& operator=(ObjectHolder&&) noexcept = default;

	ObjectHolder& operator=(nullptr_t) noexcept { m_object = nullptr; }
	template<typename U> ObjectHolder(const ObjectHolder<U>& object) : m_object(std::dynamic_pointer_cast<ObjectType>(object.GetWeakPtr().lock())) {}

	ObjectType* operator->() const noexcept { return m_object.get(); }
	ObjectType* operator*() const noexcept { return m_object.get(); }
	explicit operator bool() const noexcept { return m_object; }
	std::weak_ptr<ObjectType> GetWeakPtr() const { return m_object; }

	template<typename U = ObjectType, typename... Args>
	static ObjectHolder<U> Create(Args&&... args)
	{
		return ObjectHolder<U>(std::make_shared<U>(std::forward<Args>(args)...));
	}

	template<typename U = ObjectType>
	static ObjectHolder<U> CreateFromUniqueSharedPtr(std::shared_ptr<U>&& ptr)
	{
		return ObjectHolder<U>(std::move(ptr));
	}
};

// 
template <typename ObjectType>
class ObjectField
{
private:
	std::weak_ptr<ObjectType> m_object;

	template <typename U>
	std::weak_ptr<ObjectType> cast(const std::weak_ptr<U>& wp)
	{
		if (auto p = wp.lock())
			return std::dynamic_pointer_cast<ObjectType>(p);
		return std::weak_ptr<ObjectType>();
	}

public:
	constexpr ObjectField()noexcept(nullptr) {}
	constexpr ObjectField(nullptr_t)noexcept {}

	ObjectField(const ObjectField& object) = default;
	ObjectField& operator=(const ObjectField&) = default;
	ObjectField(ObjectField&& object) = default;
	ObjectField& operator=(ObjectField&&) noexcept = default;

	ObjectField& operator=(nullptr_t) noexcept { m_object = nullptr; }
	ObjectField(const ObjectHolder<ObjectType>& object) : m_object(object.GetWeakPtr()) {}
	template<typename U> ObjectField(const ObjectHolder<U>& object) : m_object(cast<U>(object.GetWeakPtr())) {}
	template<typename U> ObjectField(const ObjectField<U>& object) : m_object(cast<U>(object.GetWeakPtr())) {}

	ObjectType* operator->() const noexcept { if (auto p = m_object.lock()) return p.get(); else return nullptr; }
	ObjectType* operator*() const noexcept { if (auto p = m_object.lock()) return p.get(); return nullptr; }
	explicit operator bool() const noexcept { return !m_object.expired(); }
	std::weak_ptr<ObjectType> GetWeakPtr() const { return m_object; }
};