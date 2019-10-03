#pragma once

#include "GameContext.h"

// <GameSprite2D>
class GameSprite2D
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_center;
	DirectX::SimpleMath::Vector2 m_textureSize;

	float m_scale;

public:
	GameSprite2D() = default;
	~GameSprite2D() = default;

	bool Load(GameContext& ctx, const wchar_t* file_name, float scale = 1.f);

	void Draw(const DirectX::SimpleMath::Vector2& pos);
};