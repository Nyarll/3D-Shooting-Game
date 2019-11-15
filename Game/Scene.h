#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"

#include "Collider.h"

// <Scene Interface>
class IScene
{
private:
	// <シーン上に存在するゲームオブジェクト>
	std::vector<ObjectHolder<GameObject>> m_gameObjects;
	// <追加されるゲームオブジェクト>
	std::vector<ObjectHolder<GameObject>> m_addingObjects;
	// <削除されるゲームオブジェクト>
	std::vector<GameObject*> m_killObjects;
	// <検索用>
	std::unordered_multimap<std::wstring, ObjectField<GameObject>> m_objectMap;

public:
	// <コンストラクタ>
	IScene() = default;
	// <デストラクタ>
	virtual ~IScene() = default;

	// <ゲームオブジェクトの追加>
	ObjectField<GameObject> Add(ObjectHolder<GameObject>&& obj);
	// <ゲームオブジェクトの名前で検索>
	ObjectField<GameObject> Find(const std::wstring& name);
	std::vector<ObjectField<GameObject>> FindAll(const std::wstring& name);
	// <ゲームオブジェクトを追加>
	ObjectField<GameObject> AddGameObject(const std::wstring& objname = L"GameObject");
	// <全てのゲームオブジェクトを取得>
	std::vector<ObjectHolder<GameObject>>& GetObjects() { return m_gameObjects; }

	virtual void Initialize(GameContext& context) = 0;
	virtual void Update(GameContext& context) = 0;
	virtual void Render(GameContext& context) = 0;
	virtual void Finalize(GameContext& context) = 0;

	// <ゲームオブジェクトの削除>
	void ObjectDestroy(GameObject& object)
	{
		m_killObjects.push_back(&object);
	}

protected:
	// <ゲームオブジェクト関連>

	// <初期化>
	void InitializeGameObject(GameContext& context)
	{
		for (auto& obj : m_addingObjects)
		{
			m_gameObjects.push_back(std::move(obj));
		}
		m_addingObjects.clear();
		for (auto& obj : m_gameObjects)
		{
			obj->Initialize(context);
		}
	}
	// <更新>
	void UpdateGameObject(GameContext& context)
	{
		for (auto& obj : m_addingObjects)
		{
			obj->Initialize(context);
			m_gameObjects.push_back(std::move(obj));
		}
		m_addingObjects.clear();
		for (auto& obj : m_gameObjects)
		{
			obj->Update(context);
		}
		this->CheckCollision();
		KillObjectsDestroy(context);
	}
	// <描画>
	void RenderGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Render(context);
		}
	}
	// <終了処理>
	void FinalizeGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Finalize(context);
		}
	}
	// <当たり判定>
	void CheckCollision()
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			auto& obj = m_gameObjects[i];
			auto col = obj->GetComponentFind<Collider>(L"Collider");
			// <Collider を持っているかチェック>
			if (col)
			{
				for (int j = 0; j < m_gameObjects.size(); j++)
				{
					auto& obj2 = m_gameObjects[j];
					if (obj.GetWeakPtr()._Get() != obj2.GetWeakPtr()._Get())
					{
						auto col2 = obj2->GetComponentFind<Collider>(L"Collider");
						if (col2)
						{
							col->OnHitCollision(*col2);
						}
					}
				}
			}
		}
	}
	// <削除>
	void KillObjectsDestroy(GameContext& context)
	{
		for (auto& obj : m_killObjects)
		{
			obj->Finalize(context);
		}

		int i = 0;
		for (auto& kill : m_killObjects)
		{
			for (auto& obj : m_gameObjects)
			{
				if (obj.GetWeakPtr().lock().get() == kill)
				{
					m_gameObjects.erase(m_gameObjects.begin() + i);
					break;
				}
				i++;
			}
			i = 0;
		}
		m_killObjects.clear();
	}
};