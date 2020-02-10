#include "ResourceManager.h"

#include "../DeviceResources.h"

ResourceManager::~ResourceManager()
{
	this->Destroy();
}

void ResourceManager::Initialize(GameContext& context, HWND window)
{
	//ModelRegister(context, window, ResourceID::TestModel, "Resources/Models/sampleModel.fbx");
	//ModelRegister(context, window, ResourceID::TestModel, "Resources/Models/Flair.fbx");
	//this->AnimationModelRegister(context, window, ResourceID::TestModel, "Resources/Models/humanoid.fbx", 1);
	this->ModelRegister(context, window, ResourceID::Hitogata, "Resources/Models/hitogata/hitogata.FBX");
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::Idle, "Resources/Models/hitogata/Idle.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::Flair, "Resources/Models/hitogata/Flair.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::Walk, "Resources/Models/hitogata/Walking.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::RightWalk, "Resources/Models/hitogata/RightWalk.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::LeftWalk, "Resources/Models/hitogata/LeftWalk.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::Back, "Resources/Models/hitogata/Back.fbx", 1);
	m_nowProgress++;
	this->AnimationModelRegister(context, window, ResourceID::BreakDance, "Resources/Models/hitogata/BreakDance.fbx", 1);
	m_nowProgress++;
	this->ModelRegister(context, window, ResourceID::Fighter, "Resources/Models/f.FBX");
	m_nowProgress++;
	this->ModelRegister(context, window, ResourceID::Star, "Resources/Models/star.fbx");
	m_nowProgress++;
	this->ModelRegister(context, window, ResourceID::SkyDome, "Resources/Models/Sky/SkyDoom.FBX");
	m_nowProgress++;

	this->TextureRegister(context, TextureID::Floor, L"Resources/Sprite/Floor.png");
	m_nowProgress++;
	// Progress : 12
}

std::weak_ptr<FBX_LOADER::FbxModel> ResourceManager::GetFbxModel(ResourceID id)
{
	assert(m_fbxModels[id].get() && "Not Found");
	return m_fbxModels[id];
}

std::weak_ptr<DirectX::Model> ResourceManager::GetCmoModel(ResourceID id)
{

	assert(m_cmoModels[id].get() && "Not Found");
	return m_cmoModels[id];

}

ID3D11ShaderResourceView * ResourceManager::GetTexture(TextureID id)
{
	assert(m_texture[id].Get() && "Not Found");
	return m_texture[id].Get();
}

void ResourceManager::Destroy()
{
	m_fbxModels.clear();
}

bool ResourceManager::ModelRegister(GameContext& context, HWND window, ResourceID id, const char * file_path)
{
	m_fbxModels[id] = std::make_shared<FBX_LOADER::FbxModel>();
	auto& dr = context.Get<DX::DeviceResources>();
	return m_fbxModels[id]->Load(window,
		dr.GetD3DDevice(),
		dr.GetD3DDeviceContext(),
		dr.GetRenderTargetView(),
		file_path);
}

void ResourceManager::AnimationModelRegister(GameContext & context, HWND window, ResourceID id, const char * file_path, int animStackNumber)
{
	m_fbxModels[id] = std::make_shared<FBX_LOADER::FbxModel>();
	auto& dr = context.Get<DX::DeviceResources>();
	m_fbxModels[id]->Load(window,
		dr.GetD3DDevice(),
		dr.GetD3DDeviceContext(),
		dr.GetRenderTargetView(),
		file_path,
		true,
		animStackNumber);
}

void ResourceManager::CmoModelRegister(GameContext & context, ResourceID id, const wchar_t* directory, const wchar_t* file_path)
{
	auto& dr = context.Get<DX::DeviceResources>();
	DirectX::EffectFactory* factory = new DirectX::EffectFactory(dr.GetD3DDevice());

	factory->SetDirectory(directory);
	std::unique_ptr<DirectX::Model> temp = DirectX::Model::CreateFromCMO(dr.GetD3DDevice(), file_path, *factory);
	delete factory;
	DirectX::Model* model = temp.get();
	temp.release();
	m_cmoModels[id] = std::make_shared<DirectX::Model>(*model);
}

void ResourceManager::TextureRegister(GameContext & context, TextureID id, const wchar_t * file_path)
{
	if (FAILED(DirectX::CreateWICTextureFromFile(context.GetDR().GetD3DDevice(),
		file_path,
		nullptr, m_texture[id].GetAddressOf())))
	{
		MessageBox(0, L"CreateWICTextureFromFile Failed.", NULL, MB_OK);
		return;
	}
}
