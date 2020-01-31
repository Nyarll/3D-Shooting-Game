#pragma once

#include "Scene.h"

class GameObject;
class Fade;

// <Scene Play>
class ScenePlay : public IScene
{
private:
	enum CAMERA_MODE
	{
		Free,
		FollowPlayer,
		FPS,
	};

	enum DEBUG_MODE
	{
		ObjectDataMode,
		PlayerDataMode,
		NotDebugMode
	};

private:
	DEBUG_MODE DebugMode = DEBUG_MODE::ObjectDataMode;
	CAMERA_MODE CameraMode = CAMERA_MODE::Free;

	bool debugKey[6];

	std::unique_ptr<Fade>	m_fade;

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