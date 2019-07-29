#pragma once

//#include "GameObject.h"
#include "Component.h"
#include "GridFloor.h"

// <GridFloorクラスのラッパー>
class GridFloorWrapper : public Component
{
	// <グリッド床>
	std::unique_ptr<GridFloor> m_gridFloor;

public:
	std::wstring GetType()const override
	{
		return L"GridFloorWrapper";
	}
	std::wstring GetName()const override
	{
		return L"GridFloorWrapper";
	}

	// <生成>
	void Initialize(GameContext& context)override;
	// <描画>
	void Render(GameContext& context)override;
};