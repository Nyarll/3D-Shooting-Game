#pragma once

#include "../Frameworks/Component.h"

class EnemyComponent : public Component
{
private:
	std::unique_ptr<DirectX::GeometricPrimitive>	m_geo;

	DirectX::SimpleMath::Vector2 m_dir;
	float m_angle = 0;

	std::string jsonFileName = "";

public:

	void SetJsonFileName(std::string fileName)
	{
		jsonFileName = fileName;
	}

	void Initialize(GameContext& context)override;
	void Update(GameContext& context)override;
	void Render(GameContext& context)override;

};