#pragma once

#include "GameContext.h"
#include "FBXModel.h"

#include <unordered_map>

// <���\�[�X�Ǘ��N���X>
class ResourceManager final
{
private:


public:
	enum ResourceID
	{
		PlayerModel,
	};

public:
	ResourceManager() = default;
	~ResourceManager();

	void Initialize(GameContext& context);
};