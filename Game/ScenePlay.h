#pragma once

#include "Scene.h"

class GameObject;

// <Scene Play>
class ScenePlay : public IScene
{
private:
	enum CAMERA_MODE
	{
		Free,
		FollowPlayer,
	};

private:
	bool DebugMode = true;
	CAMERA_MODE CameraMode = CAMERA_MODE::Free;
	bool f3_old;
	bool f5_old;


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