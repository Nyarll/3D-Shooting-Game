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
	// <現在のシーン>
	std::unique_ptr<IScene>	m_activeScene;
	// <次のシーン>(Noneじゃなければ遷移させる)
	SceneID	m_nextSceneID;
	// <シーン作成用>
	FactoryMethod m_sceneFactoryMethods[SceneID::NumScenes];

public:
	// <コンストラクタ>
	SceneManager();
	// <デストラクタ>
	~SceneManager();

public:

	// <シーンの登録>
	void RegisterScene(SceneID _sceneID, FactoryMethod _factoryMethod);
	// <開始シーン設定>
	void SetStartScene(GameContext& context, SceneID _sceneID);
	// <現在のシーンの更新>
	void UpdateActiveScene(GameContext& context);
	// <現在のシーンの描画>
	void RenderActiveScene(GameContext& context);

	// <シーン変更要求>
	void RequestScene(SceneID _sceneID);

	// <現在アクティブなシーンを取得>
	IScene& GetActiveScene()
	{
		return *m_activeScene;
	}

private:
	// <シーン変更>
	void ChangeScene(GameContext& context, SceneID _sceneID);

};