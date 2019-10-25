#pragma once

#pragma warning(disable : 4100)

#include "Object.h"

class GameContext;
class GameObject;

// <コンポーネント>
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
	// <生成>
	virtual void Initialize(GameContext& context) {}
	// <更新>
	virtual void Update(GameContext& context) {}
	// <描画>
	virtual void Render(GameContext& context) {}
	// <破棄>
	virtual void Finalize(GameContext& context) {}
};