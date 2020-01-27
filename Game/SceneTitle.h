#pragma once

#include "Scene.h"

class GameSprite2D;

// <Scene Play>
class SceneTitle : public IScene
{
private:
	std::unique_ptr<GameSprite2D>	m_titleLogo;

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