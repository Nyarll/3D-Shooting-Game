#include "ResourceManager.h"

#include "../DeviceResources.h"

ResourceManager::~ResourceManager()
{
	this->Destroy();
}

void ResourceManager::Initialize(GameContext & context, HWND window)
{
	//ModelRegister(context, window, ResourceID::TestModel, "Resources/Models/sampleModel.fbx");
	ModelRegister(context, window, ResourceID::TestModel, "Resources/Models/humanoid.fbx");
}

std::weak_ptr<FBXSDK_Helper::FBX_Model> ResourceManager::GetFbxModel(ResourceID id)
{
	assert(m_fbxModels[id].get() && "Not Found");
	return m_fbxModels[id];
}

void ResourceManager::Destroy()
{
	m_fbxModels.clear();
}

void ResourceManager::ModelRegister(GameContext& context, HWND window, ResourceID id, const char * file_path)
{
	m_fbxModels[id] = std::make_shared<FBXSDK_Helper::FBX_Model>();
	auto& dr = context.Get<DX::DeviceResources>();
	m_fbxModels[id]->Create(window, dr.GetD3DDevice(), dr.GetD3DDeviceContext(), dr.GetRenderTargetView(), file_path);
}

void ResourceManager::AnimationModelRegister(GameContext & context, HWND window, ResourceID id, const char * file_path)
{
	m_fbxModels[id] = std::make_shared<FBXSDK_Helper::FBX_AnimationModel>();
	auto& dr = context.Get<DX::DeviceResources>();
	m_fbxModels[id]->Create(window, dr.GetD3DDevice(), dr.GetD3DDeviceContext(), dr.GetRenderTargetView(), file_path);
}
