#pragma once

#include "GameContext.h"
#include "FbxModel.h"
#include <Model.h>

#include <unordered_map>

// <リソース管理クラス>
class ResourceManager final
{
public:
	enum ResourceID
	{
		TestModel,
		SkyDome,
	};

private:
	// <モデル>
	std::unordered_map<ResourceID, std::shared_ptr<FBX_LOADER::FbxModel>>	m_fbxModels;
	std::unordered_map<ResourceID, std::shared_ptr<DirectX::Model>>			m_cmoModels;

public:
	ResourceManager() = default;
	~ResourceManager();

	void Initialize(GameContext& context, HWND window);

	std::weak_ptr<FBX_LOADER::FbxModel> GetFbxModel(ResourceID id);
	std::weak_ptr<DirectX::Model>		GetCmoModel(ResourceID id);

	void Destroy();

private:
	void ModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path);
	void AnimationModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path, int animStackNumber);
	void CmoModelRegister(GameContext& context, ResourceID id, const wchar_t* directory, const wchar_t* file_path);
};