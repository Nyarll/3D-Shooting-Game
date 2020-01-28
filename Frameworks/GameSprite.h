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

	// <頂点シェーダ>
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	// <ピクセルシェーダ>
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	// <ジオメトリシェーダ>
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader;
	// <定数バッファ>
	ID3D11Buffer *m_constantBuffer;

	// <プリミティブバッチ>
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	// <入力レイアウト>
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

public:
	GameSpritePolygon() = default;
	~GameSpritePolygon() = default;

	void Load(GameContext& ctx, const wchar_t* file_name);

	void Draw(GameContext& ctx, DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

};

class GameSpriteEffect2D
{
private:
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Time;
		DirectX::SimpleMath::Vector4	Mouse;
	};

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// <ピクセルシェーダ>
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	// <頂点シェーダ>
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// <ジオメトリシェーダ>
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

	// <プリミティブバッチ>
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch;
	// <入力レイアウト>
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	// <定数バッファ>
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

	std::vector<DirectX::VertexPositionColorTexture>  m_vertex;

public:
	bool Load(GameContext& context, const wchar_t* file_name, float scale = 1.f);

	void Draw(GameContext& context, DirectX::SimpleMath::Matrix& world, DirectX::SimpleMath::Matrix& view, DirectX::SimpleMath::Matrix& proj);

	void Render(GameContext& context, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Matrix& world, DirectX::SimpleMath::Matrix& view, DirectX::SimpleMath::Matrix& proj);
	void RenderFullWindow(GameContext& context, DirectX::SimpleMath::Vector2& pos);

	void LoadShader(GameContext& context,
		const wchar_t* pixelShaderFile = L"Resources/Shader/ParticlePS.cso",
		const wchar_t* vertexShaderFile = L"Resources/Shader/ParticleVS.cso",
		const wchar_t* geometryShaderFile = L"Resources/Shader/ParticleGS.cso"
	);
};

#endif
// GameSprite