#pragma once
#include <xhash>
#include <atomic>

using type_id_t = size_t;

template<typename T>
struct type_id_family
{
	static std::atomic<T> identifier;
};

template<typename T>
std::atomic<T> type_id_family<T>::identifier;

template<typename T>
type_id_t type_id()
{
	static const type_id_t value = type_id_family<type_id_t>::identifier++;
	return value;
}