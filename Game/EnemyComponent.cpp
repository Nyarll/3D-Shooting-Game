
#include "EnemyComponent.h"

#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "../Frameworks/Astar.hpp"

#include "StatusComponent.h"

#include "../Frameworks/JsonImport.h"

#include "../Frameworks/Random.h"

#include "../GameSystem.h"


void EnemyComponent::Initialize(GameContext & context)
{
	m_geo = DirectX::GeometricPrimitive::CreateTeapot(
		context.GetDR().GetD3DDeviceContext(),
		0.75f
	);

	m_dir = { 0,1 };
	m_angle = DirectX::XMConvertToRadians(180.f);


	std::ifstream ifs(jsonFileName);

	if (!ifs)
	{
		gameObject->AddComponent<StatusComponent>();
		return;
	}

	picojson::value value;
	ifs >> value;

	std::string name = JsonImport::get_as<std::string>(
		JsonImport::get_value(value, "Name")
		);

	int HP = JsonImport::get_as<int>(JsonImport::get_value(value, "HP"));
	int ATK = JsonImport::get_as<int>(JsonImport::get_value(value, "ATK"));
	int DEF = JsonImport::get_as<int>(JsonImport::get_value(value, "DEF"));

	gameObject->AddComponent<StatusComponent>(name, HP, ATK, DEF);
}

void EnemyComponent::Update(GameContext & context)
{
}

void EnemyComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), m_angle);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(gameObject->transform->position));
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Red);
}

