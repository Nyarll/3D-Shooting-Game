#pragma once

#include "Scene.h"

class GameObject;

// <Scene Play>
class ScenePlay : public IScene
{

public:
	ScenePlay();
	virtual ~ScenePlay();

	static IScene* Create()
	{
		return{ new ScenePlay };
	}

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;
	virtual void Finalize(GameContext& context) override;
};