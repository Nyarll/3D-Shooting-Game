#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/Component.h"
#include "../Frameworks/GameObject.h"

class SkyComponent : public Component
{
public:
	void Update(GameContext& context);
	void Render(GameContext& context);

	void SetLocalPosition(const DirectX::SimpleMath::Vector3& pos)
	{
		gameObject->transform->localPosition = pos;
	}
};