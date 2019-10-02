#include "SceneManager.h"

SceneManager::SceneManager()
{
	m_activeScene = nullptr;
	m_nextSceneID = SceneNone;

	for (int i = 0; i < NumScenes; i++)
	{
		m_sceneFactoryMethods[i] = nullptr;
	}
}

SceneManager::~SceneManager()
{
	m_activeScene.reset();
}

void SceneManager::RegisterScene(SceneID _sceneID, FactoryMethod _factoryMethod)
{
	assert(_sceneID >= 0 && _sceneID < NumScenes && "Error : Invalid scene ID.");
	assert(_factoryMethod && "Error : Generation function is not specified.");

	m_sceneFactoryMethods[_sceneID] = _factoryMethod;
}

void SceneManager::SetStartScene(GameContext& context, SceneID _sceneID)
{
	assert(_sceneID >= 0 && _sceneID < NumScenes && "Error : Invalid scene ID.");

	ChangeScene(context, _sceneID);
}

void SceneManager::UpdateActiveScene(GameContext& context)
{
	assert(m_activeScene && "Error : There are no active scenes.");

	if (m_nextSceneID != SceneNone)
	{
		ChangeScene(context, m_nextSceneID);
		m_nextSceneID = SceneNone;
	}
	m_activeScene->Update(context);
}

void SceneManager::RenderActiveScene(GameContext& context)
{
	assert(m_activeScene && "Error : There are no active scenes.");

	m_activeScene->Render(context);
}

void SceneManager::RequestScene(SceneID _sceneID)
{
	assert(_sceneID >= 0 && _sceneID < NumScenes && "Error : Invalid scene ID.");

	m_nextSceneID = _sceneID;
}

void SceneManager::ChangeScene(GameContext& context, SceneID _sceneID)
{
	assert(_sceneID >= 0 && _sceneID < NumScenes && "Error : Invalid scene ID.");
	assert(m_sceneFactoryMethods[_sceneID] && "Error : Generation function is not specified.");

	if (m_activeScene != nullptr)
	{
		m_activeScene->Finalize(context);
		m_activeScene.reset();
	}

	m_activeScene.reset(m_sceneFactoryMethods[_sceneID]());
	m_activeScene->Initialize(context);
}
