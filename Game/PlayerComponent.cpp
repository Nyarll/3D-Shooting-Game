
#include "PlayerComponent.h"

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "BulletComponent.h"

void PlayerComponent::Initialize(GameContext & context)
{
	m_playerState = Idle;
	gameObject->transform->localScale = { 0.01f,0.01f,0.01f };
	m_accele = m_velocity = DirectX::SimpleMath::Vector3::Zero;
}

void PlayerComponent::Update(GameContext & context)
{
	auto& key = context.Get<DirectX::Keyboard>().GetState();
	m_playerState = playerState::Idle;

	DirectX::SimpleMath::Vector3 force(0, 0, -1);

	if (m_forwardSpeed > this->MinSpeed)
	{
		m_forwardSpeed -= 0.002f;
	}

	if (key.W)
	{
		if (m_forwardSpeed < this->MaxSpeed)
		{
			m_forwardSpeed += 0.01f;
		}
	}
	
	if (key.Z)
	{
		m_playerState = playerState::BreakDance;

		if (m_shotCount > ShotDelayCount)
		{
			m_shotCount = 0;

			auto& scene = context.Get<SceneManager>().GetActiveScene();
			auto& obj = scene.AddGameObject(L"Star");
			obj->AddComponent<NormalBulletComponent>();

			DirectX::SimpleMath::Vector3 dir = { 0,0,-1 };
			DirectX::SimpleMath::Vector3 s, t;
			DirectX::SimpleMath::Quaternion rotation;
			context.Get<DebugFollowCamera>().m_view.Decompose(s, rotation, t);
			rotation.Inverse(rotation);
			dir = DirectX::SimpleMath::Vector3::Transform(dir, rotation);
			dir.Normalize();

			obj->GetComponent<NormalBulletComponent>()->SetDirection(dir);
			DirectX::SimpleMath::Vector3 bullet_pos = gameObject->transform->localPosition;
			bullet_pos.y += 1.f;
			obj->GetComponent<NormalBulletComponent>()->SetStartPosition(bullet_pos);
			obj->Initialize(context);
		}
	}

	DirectX::SimpleMath::Vector3 s, t;
	DirectX::SimpleMath::Quaternion rotation;
	context.Get<DebugFollowCamera>().m_view.Decompose(s, rotation, t);
	rotation.Inverse(rotation);
	force = DirectX::SimpleMath::Vector3::Transform(force, rotation);
	//force.y = 0;
	force.Normalize();

	auto flat = DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::Forward, rotation);
	flat.y = 0;
	auto flatRotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(
		DirectX::SimpleMath::Matrix::CreateWorld(
			DirectX::SimpleMath::Vector3::Zero,
			flat,
			DirectX::SimpleMath::Vector3::Up));
	gameObject->transform->localRotation = flatRotation;

	gameObject->transform->localPosition += force * m_forwardSpeed;
	m_shotCount++;
}

void PlayerComponent::Render(GameContext & context)
{
	auto& rm = context.Get<ResourceManager>();
	auto& dr = context.GetDR();

	switch (m_playerState)
	{
	case playerState::Idle:
		rm.GetFbxModel(ResourceManager::ResourceID::Idle).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;

	case playerState::Walk:
		rm.GetFbxModel(ResourceManager::ResourceID::Walk).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	case playerState::RightWalk:
		rm.GetFbxModel(ResourceManager::ResourceID::RightWalk).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	case playerState::LeftWalk:
		rm.GetFbxModel(ResourceManager::ResourceID::LeftWalk).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	case playerState::Back:
		rm.GetFbxModel(ResourceManager::ResourceID::Back).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	case playerState::BreakDance:
		rm.GetFbxModel(ResourceManager::ResourceID::BreakDance).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	default:
		rm.GetFbxModel(ResourceManager::ResourceID::Idle).lock()->Draw(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			gameObject->transform->GetMatrix(),
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
		break;
	}
}
