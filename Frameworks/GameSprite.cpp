
#include "GameSprite.h"

#include "../StepTimer.h"
#include "ShaderFileImporter.h"

#include "GameContext.h"

bool GameSprite2D::Load(GameContext& ctx, const wchar_t* file_name, float scale)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	auto device = ctx.GetDR().GetD3DDevice();

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());
	D3D11_RESOURCE_DIMENSION dimension;
	resource->GetType(&dimension);

	// <2Dではない場合>
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

void GameSprite2D::Draw(GameContext& context, const DirectX::SimpleMath::Vector2& pos)
{
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, context.Get<DirectX::CommonStates>().NonPremultiplied());

	m_spriteBatch->Draw(m_texture.Get(), pos - m_center, nullptr, DirectX::Colors::White, 0.f, { 0.f,0.f }, m_scale);
	m_spriteBatch->End();
}

void GameSpritePolygon::Load(GameContext & ctx, const wchar_t * file_name)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();
	if (FAILED(DirectX::CreateWICTextureFromFile(device, file_name, nullptr, m_texture.GetAddressOf())))
	{
		assert(false && "Texture Not Found.");
	}

	ShaderFileImporter GS_data = ShaderFileImporter::LoadFile(L"Resources/Shader/GameSpritePolygonGS.cso");
	ShaderFileImporter VS_data = ShaderFileImporter::LoadFile(L"Resources/Shader/GameSpritePolygonVS.cso");
	ShaderFileImporter PS_data = ShaderFileImporter::LoadFile(L"Resources/Shader/GameSpritePolygonPS.cso");

	device->CreateGeometryShader(GS_data.GetData(), GS_data.GetSize(), NULL, m_GeometryShader.GetAddressOf());
	device->CreateVertexShader(VS_data.GetData(), VS_data.GetSize(), NULL, m_VertexShader.GetAddressOf());
	device->CreatePixelShader(PS_data.GetData(), PS_data.GetSize(), NULL, m_PixelShader.GetAddressOf());

	// <頂点レイアウト>
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout(
		&layout[0],
		layout.size(),
		VS_data.GetData(),
		VS_data.GetSize(),
		&m_inputLayout);

	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);

	// <定数バッファの設定>
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	bool result = FAILED(device->CreateBuffer(&bd, nullptr, &m_constantBuffer));
	assert(!result && "Creation failed.");


	return;
}

void GameSpritePolygon::Draw(GameContext& ctx, DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();

	// <定数バッファで渡す値の設定>
	ConstBuffer cbuff;
	cbuff.matView = view.Transpose();;
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = world.Transpose();

	auto& timer = ctx.Get<DX::StepTimer>();
	//Time		x:経過時間(トータル秒)	y:1Fの経過時間(秒）	z:反復（サインカーブ） w:未使用（暫定で１）
	cbuff.Time = DirectX::SimpleMath::Vector4(timer.GetTotalSeconds(), timer.GetElapsedSeconds(), sinf(timer.GetTotalSeconds()), 1);

	// <定数バッファの更新>
	context->UpdateSubresource(m_constantBuffer, 0, NULL, &cbuff, 0, 0);

	auto& states = ctx.Get<DirectX::CommonStates>();
	ID3D11BlendState* blendstate = states.NonPremultiplied();
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthRead(), 0);
	context->RSSetState(states.CullNone());

	// <定数バッファをシェーダに渡す>
	ID3D11Buffer* cb[1] = { m_constantBuffer };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	ID3D11SamplerState* sampler[1] = { states.LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);
	context->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	context->IASetInputLayout(m_inputLayout.Get());

	DirectX::VertexPositionColorTexture vertex =
		DirectX::VertexPositionColorTexture(pos, DirectX::SimpleMath::Vector4::Zero, DirectX::SimpleMath::Vector2::Zero);

	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex, 1);
	m_batch->End();

	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}
