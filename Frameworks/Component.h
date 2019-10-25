#pragma once

#pragma warning(disable : 4100)

#include "Object.h"

class GameContext;
class GameObject;

// <�R���|�[�l���g>
class Component : public Object
{
public:
	Component() = default;
	virtual ~Component() = default;

public:
	GameObject* gameObject = nullptr;

	std::wstring GetType()const override
	{
		return L"Component";
	}
	std::wstring GetName()const override
	{
		return L"Component";
	}

public:
	// <����>
	virtual void Initialize(GameContext& context) {}
	// <�X�V>
	virtual void Update(GameContext& context) {}
	// <�`��>
	virtual void Render(GameContext& context) {}
	// <�j��>
	virtual void Finalize(GameContext& context) {}
};