#pragma once

#include "Scene.h"

// <Scene Play>
class ScenePlay : public IScene
{
private:


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

private:
};