#pragma once

#include "Scene.h"


enum SceneID
{
	SceneNone = -1,
	SCENE_TITLE,
	SCENE_PLAY,
	//SCENE_RESULT,

	NumScenes
};

// <Scene Manager>
class SceneManager final
{
private:
	using FactoryMethod = IScene*(*)();
	
private:
	// <���݂̃V�[��>
	std::unique_ptr<IScene>	m_activeScene;
	// <���̃V�[��>(None����Ȃ���ΑJ�ڂ�����)
	SceneID	m_nextSceneID;
	// <�V�[���쐬�p>
	FactoryMethod m_sceneFactoryMethods[SceneID::NumScenes];

public:
	// <�R���X�g���N�^>
	SceneManager();
	// <�f�X�g���N�^>
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

	// <���݃A�N�e�B�u�ȃV�[�����擾>
	IScene& GetActiveScene()
	{
		return *m_activeScene;
	}

private:
	// <�V�[���ύX>
	void ChangeScene(GameContext& context, SceneID _sceneID);

};