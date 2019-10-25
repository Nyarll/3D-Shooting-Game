
#include "PlayerComponent.h"

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "BulletComponent.h"
#include "GameAreaComponent.h"

void PlayerComponent::Initialize(GameContext & context)
{
	m_playerState = Idle;
	gameObject->transform->localScale = { 0.01f,0.01f,0.01f };
	m_accele = m_velocity = DirectX::SimpleMath::Vector3::Zero;
}

void PlayerComponent::Update(GameContext & context)
{
	auto& key = context.Get<DirectX::Keyboard>().GetState();
	auto& mouse = context.Get<DirectX::Mouse>().GetState();
	m_playerState = playerState::Idle;

	DirectX::SimpleMath::Vector3 force(0, 0, -1);

	DirectX::SimpleMath::Vector3 s, t;
	DirectX::SimpleMath::Quaternion rotation;
	context.Get<DebugFollowCamera>().m_view.Decompose(s, rotation, t);
	rotation.Inverse(rotation);
	force = DirectX::SimpleMath::Vector3::Transform(force, rotation);
	force.Normalize();

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
	
	if (mouse.leftButton)
	{
		m_playerState = playerState::BreakDance;

		if (m_shotCount > ShotDelayCount)
		{
			m_shotCount = 0;
			DirectX::SimpleMath::Vector3 spos = gameObject->transform->localPosition;
			spos.y += 1.f;

			auto r = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({0,1,0}, DirectX::XMConvertToRadians(90.f));
			auto d = DirectX::SimpleMath::Vector3::Transform(force * 2, r);
			d.y = 0;

			spos -= d;
			this->Fire(context, spos);
			spos += d * 2;
			this->Fire(context, spos);
		}
	}

	auto flat = DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::Forward, rotation);
	flat.y = 0;
	auto flatRotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(
		DirectX::SimpleMath::Matrix::CreateWorld(
			DirectX::SimpleMath::Vector3::Zero,
			flat,
			DirectX::SimpleMath::Vector3::Up));
	gameObject->transform->localRotation = flatRotation;

	DirectX::SimpleMath::Vector3 pos = gameObject->transform->localPosition;
	DirectX::SimpleMath::Vector3 limit = GameAreaComponent::AreaLimit.range;

	if ((pos.x < limit.x / 2 && pos.x > -limit.x / 2) && (pos.y < limit.y / 2 && pos.y > -limit.y / 2) &&
		(pos.z < limit.z / 2 && pos.z > -limit.z / 2))
	{
		// <エリア限界に到達していなければ移動する>
		gameObject->transform->localPosition += force * m_forwardSpeed;
	}
	else
	{
		// <初期位置へ戻る>
		gameObject->transform->localPosition = DirectX::SimpleMath::Vector3::Zero;
		m_forwardSpeed = 0.1f;
	}
	
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

void PlayerComponent::Fire(GameContext& context, DirectX::SimpleMath::Vector3 shot_pos)
{
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

	obj->GetComponent<NormalBulletComponent>()->SetStartPosition(shot_pos);
	obj->Initialize(context);
}
