#pragma once

#include "GameContext.h"
#include "FBXModel.h"

#include <unordered_map>

// <リソース管理クラス>
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