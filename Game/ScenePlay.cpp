#include "ScenePlay.h"

#include "../Frameworks/DebugCamera.h"
#include "../Frameworks/InfinityGridFloor.h"

#include "../DeviceResources.h"
#include "../Frameworks/ResourceManager.h"

ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
}

void ScenePlay::Initialize(GameContext & context)
{
	struct DrawGrid : public Component
	{
		void Initialize(GameContext& context)
		{
			context.Register(std::make_unique<InfinityGridFloor>());
			context.Get<InfinityGridFloor>().init(context, 1.f, { 300.f,300.f });
		}
		void Render(GameContext& context)
		{
			context.Get<InfinityGridFloor>().draw(context);
		}
	};
	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<DrawGrid>();

	struct Sky : public Component
	{
		void Update(GameContext& context)
		{
			auto model = context.Get<ResourceManager>().GetCmoModel(ResourceManager::ResourceID::SkyDome);
			model.lock()->UpdateEffects([&](DirectX::IEffect* effect)
			{
				DirectX::IEffectLights* lights = dynamic_cast<DirectX::IEffectLights*>(effect);
				if (lights)
				{
					lights->SetAmbientLightColor(DirectX::SimpleMath::Vector3(0, 0, 0));
					//lights->SetLightingEnabled(false);
					lights->SetLightEnabled(0, false);
					lights->SetLightEnabled(1, false);
					lights->SetLightEnabled(2, false);
				}
				DirectX::BasicEffect* basic_effect = dynamic_cast<DirectX::BasicEffect*>(effect);
				if (basic_effect)
				{
					basic_effect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
				}
			});
		}

		void Render(GameContext& context)
		{
			auto& dr = context.GetDR();
			auto model = context.Get<ResourceManager>().GetCmoModel(ResourceManager::ResourceID::SkyDome);
			model.lock()->Draw(
				dr.GetD3DDeviceContext(),
				context.Get<DirectX::CommonStates>(),
				DirectX::SimpleMath::Matrix::Identity,
				context.Get<DebugFollowCamera>().m_view,
				context.Get<DebugFollowCamera>().m_proj);
		}
	};
	auto& sky = this->AddGameObject(L"Sky");
	sky->AddComponent<Sky>();

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

