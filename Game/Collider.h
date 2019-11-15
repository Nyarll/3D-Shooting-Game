#pragma once

#include "../Frameworks/Transform.h"

#include "../Frameworks/GameContext.h"

#include <GeometricPrimitive.h>

#include "../Frameworks/Component.h"

class SphereCollider;
class BoxCollider;

// <Collider Component>
class Collider : public Component
{
protected:
	// <�f�o�b�O�\���p>
	std::unique_ptr<DirectX::GeometricPrimitive>	m_colliderRange;

	DirectX::SimpleMath::Vector3 m_scale = { 1.f,1.f,1.f };
	DirectX::SimpleMath::Vector3 m_positionOffset = { 0,0,0 };
	// <�������Ă��邩�ǂ���>
	bool m_isHit = false;
	// <���������Ώۂ̖���>
	std::wstring m_hitObjectName = L"null";

	// <�f�o�b�O�\�����邩�ǂ���>
	static bool isDraw;

public:
	// <�R���X�g���N�^>
	Collider() = default;
	// <�f�X�g���N�^>
	virtual ~Collider() = default;

	// <������>
	virtual void Initialize(GameContext& ctx) = 0;
	// <�X�V>
	virtual void Update(GameContext& context) = 0;
	// <�`��>
	void Render(GameContext& context);

	// <�����蔻��>
	virtual bool OnHitCollision(Collider* other) = 0;

	// <�������Ă��邩�ǂ����̏�Ԏ擾>
	bool IsHit()
	{
		return m_isHit;
	}

	// <�����蔻��̒��S�����炷>
	void SetOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		m_positionOffset = offset;
	}
	// <���������Ώۂ̖��̂��擾>
	std::wstring GetHitObjectName()
	{
		return m_hitObjectName;
	}

	virtual std::wstring GetType()const override
	{
		return L"Collider";
	}
	virtual std::wstring GetName()const override
	{
		return L"Collider";
	}

public:
	// <�f�o�b�O�\���ؑ�>
	static void IsDraw(bool t)
	{
		isDraw = t;
	}
};

// <DirectXCollision���g�p>

// <SphereCollider Component>
class SphereCollider : public Collider
{
private:
	// <�����蔻��p�I�u�W�F�N�g>
	DirectX::BoundingSphere	m_bounding;
	// <���a>
	float m_radius;

public:
	// <������>
	void Initialize(GameContext& ctx)override;
	// <�X�V>
	void Update(GameContext& context)override;
	// <�����蔻��>
	bool OnHitCollision(Collider* other)override;
	// <���a�o�^>
	void SetRadius(float radius)
	{
		m_radius = radius;
		m_bounding.Radius = radius;
	}
	// <�����蔻��p�I�u�W�F�N�g�̎擾>
	DirectX::BoundingSphere& GetBounding()
	{
		return m_bounding;
	}
};

// <BoxCollider Component>
class BoxCollider : public Collider
{
private:
	// <�����蔻��p�I�u�W�F�N�g>
	DirectX::BoundingBox	m_bounding;
	// <���̃T�C�Y>
	DirectX::SimpleMath::Vector3	m_size;

public:
	// <������>
	void Initialize(GameContext& ctx)override;
	// <�X�V>
	void Update(GameContext& context)override;
	// <�����蔻��>
	bool OnHitCollision(Collider* other)override;
	// <�T�C�Y�o�^>
	void SetSize(DirectX::SimpleMath::Vector3& size)
	{
		m_size = size;
		m_bounding.Extents = size;
	}
	// <�����蔻��p�I�u�W�F�N�g�̎擾>
	DirectX::BoundingBox& GetBounding()
	{
		return m_bounding;
	}
};