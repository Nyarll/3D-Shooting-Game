
#include "SkyComponent.h"
#include "../Frameworks/ResourceManager.h"

void SkyComponent::Update(GameContext & context)
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

void SkyComponent::Render(GameContext & context)
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
