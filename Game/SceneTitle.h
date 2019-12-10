#pragma once

#include "Scene.h"

// <Scene Play>
class SceneTitle : public IScene
{
public:
	SceneTitle();
	virtual ~SceneTitle();

	static IScene* Create()
	{
		return{ new SceneTitle };
	}

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;
	virtual void Finalize(GameContext& context) override;

};