#include "Stage.h"

#include "../Frameworks/GameContext.h"
#include "SceneManager.h"

#include "ModelRenderer.h"

Stage::Stage()
	: m_x(10)
	, m_y(10)
{
}

void Stage::Initialize(GameContext & context)
{
	assert((m_x != 0 || m_y != 0) && "Invalid value.");

	for (int y = 0; y < m_y; y++)
	{
		m_stage.push_back(std::vector<int>());
		for (int x = 0; x < m_x; x++)
		{
			m_stage[y].push_back(0);
		}
	}

	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& dr = context.GetDR();

	DirectX::EffectFactory factory(dr.GetD3DDevice());
	factory.SetDirectory(L"Resources/Models");
	m_model = DirectX::Model::CreateFromCMO(dr.GetD3DDevice(), L"Resources/Models/Panel1.cmo", factory);

	for (int y = 0; y < m_stage.size(); y++)
	{
		for (int x = 0; x < m_stage[y].size(); x++)
		{
			auto& obj = scene.AddGameObject(L"Floor");
			obj->AddComponent<ModelRenderer>();
			obj->GetComponent<ModelRenderer>()->SetFloorModel(m_model.get());
			float _x = -(static_cast<float>(m_stage[y].size()) / 2.f) + static_cast<float>(x) + 0.5f;
			float _y = -(static_cast<float>(m_stage.size()) / 2.f) + static_cast<float>(y) + 0.5f;
			float scale = 0.028f;

			obj->transform->localScale = DirectX::SimpleMath::Vector3(scale, scale, scale);
			obj->transform->localPosition = DirectX::SimpleMath::Vector3(_x, 0.f, _y);
		}
	}
}

void Stage::Update(GameContext & context)
{
}
