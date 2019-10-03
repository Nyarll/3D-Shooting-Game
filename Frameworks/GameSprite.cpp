
#include "GameSprite.h"

bool GameSprite2D::Load(GameContext& ctx, const wchar_t* file_name, float scale)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	auto device = ctx.GetDR().GetD3DDevice();

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());
	D3D11_RESOURCE_DIMENSION dimension;
	resource->GetType(&dimension);
	
	// <2D‚Å‚Í‚È‚¢ê‡>
	if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	{
		return false;
	}
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	resource.As(&texture2D);
	D3D11_TEXTURE2D_DESC desc;
	texture2D->GetDesc(&desc);

	m_scale = scale;

	m_textureSize = DirectX::SimpleMath::Vector2(desc.Width, desc.Height) * m_scale;
	m_center = m_textureSize / 2;
	return true;
}

void GameSprite2D::Draw(const DirectX::SimpleMath::Vector2& pos)
{
	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_texture.Get(), pos - m_center, nullptr, DirectX::Colors::White, 0.f, { 0.f,0.f }, m_scale);
	m_spriteBatch->End();
}
