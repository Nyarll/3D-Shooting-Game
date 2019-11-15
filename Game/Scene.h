#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/GameObject.h"

#include "Collider.h"

// <Scene Interface>
class IScene
{
private:
	// <�V�[����ɑ��݂���Q�[���I�u�W�F�N�g>
	std::vector<ObjectHolder<GameObject>> m_gameObjects;
	// <�ǉ������Q�[���I�u�W�F�N�g>
	std::vector<ObjectHolder<GameObject>> m_addingObjects;
	// <�폜�����Q�[���I�u�W�F�N�g>
	std::vector<GameObject*> m_killObjects;
	// <�����p>
	std::unordered_multimap<std::wstring, ObjectField<GameObject>> m_objectMap;

public:
	// <�R���X�g���N�^>
	IScene() = default;
	// <�f�X�g���N�^>
	virtual ~IScene() = default;

	// <�Q�[���I�u�W�F�N�g�̒ǉ�>
	ObjectField<GameObject> Add(ObjectHolder<GameObject>&& obj);
	// <�Q�[���I�u�W�F�N�g�̖��O�Ō���>
	ObjectField<GameObject> Find(const std::wstring& name);
	std::vector<ObjectField<GameObject>> FindAll(const std::wstring& name);
	// <�Q�[���I�u�W�F�N�g��ǉ�>
	ObjectField<GameObject> AddGameObject(const std::wstring& objname = L"GameObject");
	// <�S�ẴQ�[���I�u�W�F�N�g���擾>
	std::vector<ObjectHolder<GameObject>>& GetObjects() { return m_gameObjects; }

	virtual void Initialize(GameContext& context) = 0;
	virtual void Update(GameContext& context) = 0;
	virtual void Render(GameContext& context) = 0;
	virtual void Finalize(GameContext& context) = 0;

	// <�Q�[���I�u�W�F�N�g�̍폜>
	void ObjectDestroy(GameObject& object)
	{
		m_killObjects.push_back(&object);
	}

protected:
	// <�Q�[���I�u�W�F�N�g�֘A>

	// <������>
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
	// <�X�V>
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
	// <�`��>
	void RenderGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Render(context);
		}
	}
	// <�I������>
	void FinalizeGameObject(GameContext& context)
	{
		for (auto& obj : m_gameObjects)
		{
			obj->Finalize(context);
		}
	}
	// <�����蔻��>
	void CheckCollision()
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			auto& obj = m_gameObjects[i];
			auto col = obj->GetComponentFind<Collider>(L"Collider");
			// <Collider �������Ă��邩�`�F�b�N>
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
	// <�폜>
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