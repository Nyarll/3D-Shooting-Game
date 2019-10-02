#pragma once

#include "Scene.h"


enum SceneID
{
	SceneNone = -1,
	//SceneTitle,
	ScenePlay,
	//SceneResult,

	NumScenes
};

// <Scene Manager>
class SceneManager final
{
private:
	using FactoryMethod = IScene*(*)();
	
	std::unique_ptr<IScene>	m_activeScene;
	SceneID	m_nextSceneID;
	FactoryMethod m_sceneFactoryMethods[SceneID::NumScenes];

public:
	SceneManager();
	~SceneManager();

public:

	// <�V�[���̓o�^>
	void RegisterScene(SceneID _sceneID, FactoryMethod _factoryMethod);
	// <�J�n�V�[���ݒ�>
	void SetStartScene(GameContext& context, SceneID _sceneID);
	// <���݂̃V�[���̍X�V>
	void UpdateActiveScene(GameContext& context);
	// <���݂̃V�[���̕`��>
	void RenderActiveScene(GameContext& context);

	// <�V�[���ύX�v��>
	void RequestScene(SceneID _sceneID);

	IScene& GetActiveScene()
	{
		return *m_activeScene;
	}

private:
	// <�V�[���ύX>
	void ChangeScene(GameContext& context, SceneID _sceneID);

};