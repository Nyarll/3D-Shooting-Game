#include "SceneTitle.h"

#include "SceneManager.h"

#include "../Frameworks/GameFont.h"

SceneTitle::SceneTitle()
{
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Initialize(GameContext & context)
{
}

void SceneTitle::Update(GameContext & context)
{
	auto& manager = context.Get<SceneManager>();
	auto& key = context.Get<DirectX::Keyboard>().GetState();

	if (key.Space)
	{
		manager.RequestScene(SceneID::SCENE_PLAY);
	}
}

void SceneTitle::Render(GameContext & context)
{
	auto& font = context.Get<GameFont>();

	DirectX::SimpleMath::Vector2 pos = { 400 - sizeof("Push to [Space] Key.") * 6, 300 };
	font.Draw(pos, DirectX::Colors::Black, "Push to [Space] Key.");
}

void SceneTitle::Finalize(GameContext & context)
{
}
