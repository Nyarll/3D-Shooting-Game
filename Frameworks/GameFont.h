#pragma once

#include "GameContext.h"
#include <string>

// <GameFont>
class GameFont
{
private:
	std::unique_ptr<DirectX::CommonStates> m_state;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

public:
	GameFont() = default;
	~GameFont() = default;

	bool Load(GameContext& ctx, const wchar_t* file_name);

	void Draw(DirectX::SimpleMath::Vector2 pos,DirectX::FXMVECTOR color, std::string fmt_str, ...);
};