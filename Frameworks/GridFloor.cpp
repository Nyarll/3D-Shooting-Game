#include "pch.h"
#include "GridFloor.h"

// <�R���X�g���N�^>
GridFloor::GridFloor(ID3D11Device * device, ID3D11DeviceContext * context, DirectX::CommonStates * states, float size, int divs)
{
	// <�G�t�F�N�g�̐���>
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// <���_�J���[(�L��)>
	m_basicEffect->SetVertexColorEnabled(true);
	// <�v���~�e�B�u�I�u�W�F�N�g����>
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// <�C���v�b�g���C�A�E�g����>
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.GetAddressOf());
}

// <�f�X�g���N�^>
GridFloor::~GridFloor()
{
	// <�G�t�F�N�g�����>
	m_basicEffect.reset();
	// <���̓��C�A�E�g�����>
	m_pInputLayout.Reset();
}

// <�`��>
void GridFloor::draw(ID3D11DeviceContext * context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::GXMVECTOR color)
{
	DirectX::SimpleMath::Matrix world;

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);


	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	m_primitiveBatch->Begin();

	const DirectX::XMVECTORF32 xAxis = { m_size, 0.f, 0.f };
	const DirectX::XMVECTORF32 yAxis = { 0.f, 0.f, m_size };

	size_t divs = std::max<size_t>(1, m_divs);
	DirectX::FXMVECTOR origin = DirectX::g_XMZero;
	for (size_t i = 0; i <= divs; ++i)
	{
		float fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, yAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, yAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divs; i++)
	{
		FLOAT fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, xAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, xAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}

