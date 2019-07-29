#pragma once
#include <xhash>

class Type_ID;
template<>
struct std::hash<Type_ID>;

// https://codereview.stackexchange.com/questions/48594/unique-type-id-no-rtti
class Type_ID
{
	using sig = Type_ID();

	sig* id;
	Type_ID(sig* id) : id{ id } {}

public:
	template<typename T>
	friend Type_ID type_id();

	friend struct std::hash<Type_ID>;

	bool operator==(Type_ID o) const { return id == o.id; }
	bool operator!=(Type_ID o) const { return id != o.id; }
};

template<typename T>
Type_ID type_id() { return &type_id<T>; }

namespace std {
	template <>
	struct hash<Type_ID> 
	{
		size_t operator()(const Type_ID& id) const 
		{
			return reinterpret_cast<size_t>(id.id);
		}
	};
}