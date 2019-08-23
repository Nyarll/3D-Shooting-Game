#pragma once

#include "GameContext.h"
#include "FBXModel.h"

#include <unordered_map>

// <リソース管理クラス>
class ResourceManager final
{
public:
	enum ResourceID
	{
		TestModel,
	};

private:
	// <モデル>
	std::unordered_map<ResourceID, std::shared_ptr<FBXSDK_Helper::FBX_Model>>	m_fbxModels;
	
public:
	ResourceManager() = default;
	~ResourceManager();

	void Initialize(GameContext& context, HWND window);

	std::weak_ptr<FBXSDK_Helper::FBX_Model> GetFbxModel(ResourceID id);

	void Destroy();

private:
	void ModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path);
	void AnimationModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path);
};