
#include "BulletComponent.h"
#include "SceneManager.h"

#include "GameAreaComponent.h"

void NormalBulletComponent::Initialize(GameContext & context)
{
	m_vel = m_direction * 25.0f;
	std::random_device seedGenerator;
	std::mt19937 engine(seedGenerator());
	{
		gameObject->transform->localScale = DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f);
	}
	{
		std::uniform_real_distribution<float> randVelocity(0.0f, 1.0f);
		float rx = randVelocity(engine);
		float ry = randVelocity(engine);
		float rz = randVelocity(engine);
		DirectX::SimpleMath::Matrix x, y, z;
		x = DirectX::SimpleMath::Matrix::CreateRotationX(rx);
		y = DirectX::SimpleMath::Matrix::CreateRotationY(ry);
		z = DirectX::SimpleMath::Matrix::CreateRotationZ(rz);
		DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
		rot *= rx * ry * rz;
		gameObject->transform->localRotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
	}
	m_lifeTime = 120;
}

void NormalBulletComponent::Update(GameContext & context)
{
	if (m_lifeTime <= 0)
	{
		auto& scene = context.Get<SceneManager>().GetActiveScene();
		scene.ObjectDestroy(*gameObject);
	}
	DirectX::SimpleMath::Vector3 pos = gameObject->transform->localPosition;
	DirectX::SimpleMath::Vector3 limit = GameAreaComponent::AreaLimit.range;
	if (((pos.x < limit.x / 2 && pos.x > -limit.x / 2) && (pos.y < limit.y / 2 && pos.y > -limit.y / 2) &&
		(pos.z < limit.z / 2 && pos.z > -limit.z / 2)))
	{

	}
	else
	{
		auto& scene = context.Get<SceneManager>().GetActiveScene();
		scene.ObjectDestroy(*gameObject);
	}
	gameObject->transform->localPosition += m_vel;
	m_lifeTime -= 1;
	m_angle += DirectX::XMConvertToRadians(5.f);

	rotate = DirectX::SimpleMath::Matrix::CreateRotationZ(m_angle);
}

void NormalBulletComponent::Render(GameContext & context)
{
	auto& rm = context.Get<ResourceManager>();
	auto& dr = context.GetDR();

	auto& camera = context.GetFollowCamera();

	DirectX::SimpleMath::Matrix mat = rotate * DirectX::SimpleMath::Matrix::CreateBillboard(
		gameObject->transform->localPosition,
		camera.m_eye,
		DirectX::SimpleMath::Vector3::UnitY); 

	rm.GetFbxModel(ResourceManager::ResourceID::Star).lock()->Draw(
		dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
		mat,
		context.Get<DebugFollowCamera>().m_view,
		context.Get<DebugFollowCamera>().m_proj);
}

void NormalBulletComponent::SetDirection(const DirectX::SimpleMath::Vector3 & direction)
{
	m_direction = direction;
	m_direction.Normalize();
	m_direction *= 0.1f;
}

void NormalBulletComponent::SetStartPosition(const DirectX::SimpleMath::Vector3 & position)
{
	gameObject->transform->localPosition = position;
}
