#include "ScenePlay.h"

#include "../Frameworks/DebugCamera.h"
#include "../Frameworks/GridFloorWrapper.h"

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
}

void ScenePlay::Update(GameContext & context)
{
	{
		// Get<DebugCamera>().Update(*this);
		context.Get<DebugFollowCamera>().SetTarget({ 0,0,0 });
		context.Get<DebugFollowCamera>().Update(context);

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
}

void ScenePlay::Render(GameContext & context)
{
	context.Get<GridFloorWrapper>().Render(context);

	auto& dr = context.Get<DX::DeviceResources>();
	DirectX::SimpleMath::Matrix w = DirectX::SimpleMath::Matrix::Identity;
	w *= DirectX::SimpleMath::Matrix::CreateScale(0.02f);
	//w *= Matrix::CreateScale(0.01f);
	//w *= Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f));

	{
		auto model = context.Get<ResourceManager>().GetCmoModel(ResourceManager::ResourceID::SkyDome);
		model.lock()->Draw(
			dr.GetD3DDeviceContext(),
			context.Get<DirectX::CommonStates>(),
			DirectX::SimpleMath::Matrix::Identity,
			context.Get<DebugFollowCamera>().m_view,
			context.Get<DebugFollowCamera>().m_proj);
	}
}

void ScenePlay::Finalize(GameContext & context)
{
}
