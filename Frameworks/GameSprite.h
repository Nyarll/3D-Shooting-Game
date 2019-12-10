#pragma once

#ifndef GAMESPRITE_DEFINED
#define GAMESPRITE_DEFINED

#include "GameContext.h"

#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

class GameContext;

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

	void Draw(GameContext& context, const DirectX::SimpleMath::Vector2& pos);
};

class GameSpritePolygon
{
private:
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Time;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// <���_�V�F�[�_>
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	// <�s�N�Z���V�F�[�_>
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	// <�W�I���g���V�F�[�_>
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader;
	// <�萔�o�b�t�@>
	ID3D11Buffer *m_constantBuffer;

	// <�v���~�e�B�u�o�b�`>
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	// <���̓��C�A�E�g>
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

public:
	GameSpritePolygon() = default;
	~GameSpritePolygon() = default;

	void Load(GameContext& ctx, const wchar_t* file_name);

	void Draw(GameContext& ctx, DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

};

#endif
// GameSprite