
#include "GameSprite.h"

#include "../StepTimer.h"
#include "ShaderFileImporter.h"

bool GameSprite2D::Load(GameContext& ctx, const wchar_t* file_name, float scale)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	auto device = ctx.GetDR().GetD3DDevice();

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());
	D3D11_RESOURCE_DIMENSION dimension;
	resource->GetType(&dimension);

	// <2D�ł͂Ȃ��ꍇ>
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

void GameSpritePolygon::Load(GameContext & ctx, const wchar_t * file_name)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();
	DirectX::CreateWICTextureFromFile(device, file_name, nullptr, m_texture.GetAddressOf());

	//ShaderFileImporter VS_data = ShaderFileImporter::LoadFile(L"Resources/Shader/GameSpritePolygonVS.cso");
	//ShaderFileImporter PS_data = ShaderFileImporter::LoadFile(L"Resources/Shader/GameSpritePolygonPS.cso");

	//device->CreateVertexShader(VS_data.GetData(), VS_data.GetSize(), NULL, m_VertexShader.GetAddressOf());
	//device->CreatePixelShader(PS_data.GetData(), PS_data.GetSize(), NULL, m_PixelShader.GetAddressOf());

	//// <���_���C�A�E�g>
	//std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	//{
	//	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};
	//device->CreateInputLayout(
	//	&layout[0],
	//	layout.size(),
	//	VS_data.GetData(),
	//	VS_data.GetSize(),
	//	&m_inputLayout);

	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);

	// <�萔�o�b�t�@�̐ݒ�>
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(ConstBuffer);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	(device->CreateBuffer(&cb, NULL, &m_constantBuffer));

	return;
}

void GameSpritePolygon::Draw(GameContext& ctx, DirectX::SimpleMath::Matrix world)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();

	//�萔�o�b�t�@�œn���l�̐ݒ�
	ConstBuffer cbuff;
	cbuff.matView = ctx.Get<DebugFollowCamera>().m_view.Transpose();
	cbuff.matProj = ctx.Get<DebugFollowCamera>().m_proj.Transpose();
	cbuff.matWorld = world.Transpose();

	auto timer = ctx.Get<DX::StepTimer>();
	//Time		x:�o�ߎ���(�g�[�^���b)	y:1F�̌o�ߎ���(�b�j	z:�����i�T�C���J�[�u�j w:���g�p�i�b��łP�j
	cbuff.Time = DirectX::SimpleMath::Vector4(timer.GetTotalSeconds(), timer.GetElapsedSeconds(), sinf(timer.GetTotalSeconds()), 1);

	context->UpdateSubresource(m_constantBuffer, 0, NULL, &cbuff, 0, 0);

	auto& states = ctx.Get<DirectX::CommonStates>();
	ID3D11BlendState* blendstate = states.NonPremultiplied();
	// �������菈��
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// �[�x�o�b�t�@�͎Q�Ƃ̂�
	context->OMSetDepthStencilState(states.DepthRead(), 0);
	// �J�����O�͔����v���
	context->RSSetState(states.CullCounterClockwise());

	// <�萔�o�b�t�@���V�F�[�_�ɓn��>
	ID3D11Buffer* cb[1] = { m_constantBuffer };
	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//�T���v���[�A�V�F�[�_�A�摜�����ꂼ��o�^
	ID3D11SamplerState* sampler[1] = { states.LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	//���̓��C�A�E�g�𔽉f
	context->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertex[0], m_vertex.size());
	m_batch->End();
}
