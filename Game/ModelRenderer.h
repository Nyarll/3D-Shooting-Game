#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/GameContext.h"

class CMOModelRenderer : public Component
{
	DirectX::Model*	m_model;

public:
	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	void SetFloorModel(DirectX::Model* model)
	{
		m_model = model;
	}
};
