#include "ScenePlay.h"

#include "../Frameworks/DebugCamera.h"
#include "../Frameworks/InfinityGridFloor.h"

#include "../DeviceResources.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "GridComponent.h"
#include "SkyComponent.h"
#include "BulletComponent.h"

ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
}

void ScenePlay::Initialize(GameContext & context)
{
	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<GridComponent>();

	auto& sky = this->AddGameObject(L"Sky");
	sky->AddComponent<SkyComponent>();

	struct Shoot : public Component
	{
	private:
		int m_lifeTime;
		DirectX::SimpleMath::Vector3 m_vel;

	public:
		void Initialize(GameContext& context)
		{
			m_vel = DirectX::SimpleMath::Vector3::Zero;

			DirectX::SimpleMath::Vector3 force = DirectX::SimpleMath::Vector3::Zero;
			DirectX::SimpleMath::Vector3 s, t;
			DirectX::SimpleMath::Quaternion rotation;
			context.Get<DebugFollowCamera>().m_view.Decompose(s, rotation, t);
			rotation.Inverse(rotation);
			force = DirectX::SimpleMath::Vector3::Transform(force, rotation);
			force.Normalize();
			force *= 0.1f;

			m_vel = force;
			m_lifeTime = 60;

			auto& scene = context.Get<SceneManager>().GetActiveScene();
			gameObject->transform = scene.Find(L"Player")->transform;

			//DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90.f));
			//gameObject->transform->localRotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
		}
		void Update(GameContext& context)
		{
			if (m_lifeTime <= 0)
			{
				auto& scene = context.Get<SceneManager>().GetActiveScene();
				scene.ObjectDestroy(*gameObject);
			}
			//gameObject->transform->localPosition += m_vel;
			m_lifeTime -= 1;
		}
		void Render(GameContext& context)
		{
			auto& rm = context.Get<ResourceManager>();
			auto& dr = context.GetDR();

			auto mat = gameObject->transform->GetMatrix();
			mat = DirectX::SimpleMath::Matrix::Identity;

			rm.GetFbxModel(ResourceManager::ResourceID::Star).lock()->Draw(
				dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
				mat,
				context.Get<DebugFollowCamera>().m_view,
				context.Get<DebugFollowCamera>().m_proj);
		}
	};

	struct Player : public Component
	{
	private:
		enum playerState
		{
			Idle,
			Walk,
			RightWalk,
			LeftWalk,
			Back,
			BreakDance
		}m_playerState;

		DirectX::SimpleMath::Vector3 m_accele, m_velocity;

		int m_shotCount = 0;
		const int ShotDelayCount = 10;

	public:
		void Initialize(GameContext& context)
		{
			m_playerState = Idle;
			gameObject->transform->localScale = { 0.01f,0.01f,0.01f };
			m_accele = m_velocity = DirectX::SimpleMath::Vector3::Zero;
		}

		void Update(GameContext& context)
		{
			auto& key = context.Get<DirectX::Keyboard>().GetState();
			m_playerState = playerState::Idle;

			auto force = DirectX::SimpleMath::Vector3::Zero;
			if (key.A)
			{
				force.x -= 1;
				m_playerState = playerState::LeftWalk;
			}
			if (key.D)
			{
				force.x += 1;
				m_playerState = playerState::RightWalk;
			}
			if (key.W)
			{
				force.z -= 1;
				m_playerState = playerState::Walk;
			}
			if (key.S)
			{
				force.z += 1;
				m_playerState = playerState::Back;
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
			force.y = 0;
			force.Normalize();

			auto flat = DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::Forward, rotation);
			flat.y = 0;
			auto flatRotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(
				DirectX::SimpleMath::Matrix::CreateWorld(
					DirectX::SimpleMath::Vector3::Zero,
					flat,
					DirectX::SimpleMath::Vector3::Up));
			gameObject->transform->localRotation = flatRotation;

			gameObject->transform->localPosition += force * 0.1f;
			m_shotCount++;
		}

		void Render(GameContext& context)
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
	};

	auto& player = this->AddGameObject(L"Player");
	player->AddComponent<Player>();

	this->InitializeGameObject(context);
}

void ScenePlay::Update(GameContext & context)
{
	{
		auto player = Find(L"Player");
		context.Get<DebugFollowCamera>().SetTarget(player->transform->position);
		context.Get<DebugFollowCamera>().Update(context);
	}

	this->UpdateGameObject(context);
}

void ScenePlay::Render(GameContext & context)
{
	this->RenderGameObject(context);
}

void ScenePlay::Finalize(GameContext & context)
{
	this->FinalizeGameObject(context);
}

