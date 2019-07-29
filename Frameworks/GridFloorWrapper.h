#pragma once

//#include "GameObject.h"
#include "Component.h"
#include "GridFloor.h"

// <GridFloor�N���X�̃��b�p�[>
class GridFloorWrapper : public Component
{
	// <�O���b�h��>
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

	// <����>
	void Initialize(GameContext& context)override;
	// <�`��>
	void Render(GameContext& context)override;
};