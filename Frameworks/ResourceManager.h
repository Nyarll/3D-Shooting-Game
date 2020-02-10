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
		Hitogata,
		Idle,
		Flair,
		Walk,
		RightWalk,
		LeftWalk,
		Back,
		BreakDance,

		Fighter,

		Star,
		SkyDome,
	};

	enum TextureID
	{
		Floor,
	};

private:
	// <モデル>
	std::unordered_map<ResourceID, std::shared_ptr<FBX_LOADER::FbxModel>>	m_fbxModels;
	std::unordered_map<ResourceID, std::shared_ptr<DirectX::Model>>			m_cmoModels;

	// <テクスチャ>
	std::unordered_map <TextureID, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	m_texture;

	int m_nowProgress = 0;
	const int ProgressEnd = 12;

public:
	ResourceManager() = default;
	~ResourceManager();

	void Initialize(GameContext& context, HWND window);

	std::weak_ptr<FBX_LOADER::FbxModel> GetFbxModel(ResourceID id);
	std::weak_ptr<DirectX::Model>		GetCmoModel(ResourceID id);
	ID3D11ShaderResourceView*			GetTexture(TextureID id);

	int GetNowProgress() { return m_nowProgress; }
	int GetProgressEnd() { return ProgressEnd; }

	void Destroy();

private:
	bool ModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path);
	void AnimationModelRegister(GameContext& context, HWND window, ResourceID id, const char* file_path, int animStackNumber);
	void CmoModelRegister(GameContext& context, ResourceID id, const wchar_t* directory, const wchar_t* file_path);

	void TextureRegister(GameContext& context, TextureID id, const wchar_t* file_path);
};