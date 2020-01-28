
#include "GameSprite.h"

#include "../StepTimer.h"
#include "ShaderFileImporter.h"

#include "GameContext.h"

#include "../Frameworks/BinaryFile.h"

#include "../GameSystem.h"

const std::vector<D3D11_INPUT_ELEMENT_DESC> GameSpriteEffect::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


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

bool GameSpriteEffect::Load(GameContext & context, const wchar_t * file_name, float scale)
{
	auto& dr = context.GetDR();
	auto device = dr.GetD3DDevice();
	auto deviceContext = dr.GetD3DDeviceContext();

	m_scale = scale;

	// <テクスチャ読み込み>
	if (FAILED(DirectX::CreateWICTextureFromFile(device, file_name, nullptr, m_texture.GetAddressOf())))
	{
		MessageBox(0, L"CreateWICTextureFromFile Failed.", NULL, MB_OK);
		return false;
	}

	this->LoadShader(context);

	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(deviceContext);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);

	return false;
}

void GameSpriteEffect::Render(GameContext & context, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Matrix & world, DirectX::SimpleMath::Matrix & view, DirectX::SimpleMath::Matrix & proj)
{
	m_vertex.clear();
	DirectX::VertexPositionColorTexture vertex(pos,
		DirectX::SimpleMath::Vector4::Zero,
		DirectX::SimpleMath::Vector2::Zero);

	m_vertex.push_back(vertex);
	this->Draw(context, world, view, proj);
}

void GameSpriteEffect::Render2D(GameContext & context, DirectX::SimpleMath::Vector2 & pos)
{
	DirectX::SimpleMath::Vector2 windowSize = GameSystem::GetWindowSize();
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(pos.x / windowSize.x - 0.5f, pos.y / windowSize.y - 0.5f, 0);
	DirectX::SimpleMath::Matrix identity = DirectX::SimpleMath::Matrix::Identity;
	this->Render(context, position, DirectX::SimpleMath::Matrix::CreateScale(m_scale), identity, identity);
}

void GameSpriteEffect::Draw(GameContext & context, DirectX::SimpleMath::Matrix& world, DirectX::SimpleMath::Matrix& view, DirectX::SimpleMath::Matrix& proj)
{
	auto& dr = context.GetDR();
	auto deviceContext = dr.GetD3DDeviceContext();
	auto& state = context.Get<DirectX::CommonStates>();
	auto& timer = context.Get<DX::StepTimer>();

	//定数バッファで渡す値の設定
	ConstBuffer cbuff;
	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = world.Transpose();

	if (cbuf_time == DirectX::SimpleMath::Vector4::Zero)
	{
		cbuff.Any = DirectX::SimpleMath::Vector4(timer.GetTotalSeconds(), timer.GetElapsedSeconds(), sinf(timer.GetTotalSeconds()), 1);
	}
	else
	{
		cbuff.Any = cbuf_time;
	}
	auto& mouse = DirectX::Mouse::Get();
	auto ms = mouse.GetState();
	cbuff.Mouse = DirectX::SimpleMath::Vector4(static_cast<float>(ms.x) / 800.f, static_cast<float>(ms.y) / 600.f,
		ms.leftButton, ms.rightButton);

	//定数バッファの内容更新
	deviceContext->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	ID3D11BlendState* blendstate = state.NonPremultiplied();
	// 透明判定処理
	deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// 深度バッファは参照のみ
	deviceContext->OMSetDepthStencilState(state.DepthRead(), 0);
	// カリングは反時計回り
	deviceContext->RSSetState(state.CullCounterClockwise());

	//定数バッファをシェーダに渡す（とりあえずPSは要らないのでコメントアウト）
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	deviceContext->VSSetConstantBuffers(0, 1, cb);
	deviceContext->GSSetConstantBuffers(0, 1, cb);
	deviceContext->PSSetConstantBuffers(0, 1, cb);


	//サンプラー、シェーダ、画像をそれぞれ登録
	ID3D11SamplerState* sampler[1] = { state.LinearWrap() };
	deviceContext->PSSetSamplers(0, 1, sampler);
	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	//入力レイアウトを反映
	deviceContext->IASetInputLayout(m_inputLayout.Get());

	//バッチに頂点情報を渡す
	m_primitiveBatch->Begin();
	//m_batch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertex[0], m_vertex.size());
	m_primitiveBatch->End();


	//他のモデルに影響が出る可能性があるので使い終わったらシェーダを外す
	deviceContext->VSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);
}

void GameSpriteEffect::LoadShader(GameContext & context, const wchar_t * pixelShaderFile, const wchar_t * vertexShaderFile, const wchar_t * geometryShaderFile)
{
	BinaryFile PSData = BinaryFile::LoadFile(pixelShaderFile);
	BinaryFile VSData = BinaryFile::LoadFile(vertexShaderFile);
	BinaryFile GSData = BinaryFile::LoadFile(geometryShaderFile);

	auto& dr = context.GetDR();
	auto device = dr.GetD3DDevice();

	device->CreateInputLayout(&INPUT_LAYOUT[0],
		INPUT_LAYOUT.size(),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.ReleaseAndGetAddressOf()
	);

	// ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}
	// 頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}
	// ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
}
