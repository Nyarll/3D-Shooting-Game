#pragma once

#include "Scene.h"

class GameSpriteEffect;
class Fade;

// <Scene Play>
class SceneTitle : public IScene
{
private:
	std::unique_ptr<GameSpriteEffect>	m_titleLogo;

	std::unique_ptr<Fade>				m_fade;

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