#pragma once

#include "../Frameworks/Transform.h"

#include "../Frameworks/GameContext.h"

#include <GeometricPrimitive.h>

#include "../Frameworks/Component.h"

class SphereCollider;
class BoxCollider;

class Collider : public Component
{
protected:

	std::unique_ptr<DirectX::GeometricPrimitive>	m_colliderRange;
	DirectX::SimpleMath::Vector3 m_scale = { 1.f,1.f,1.f };
	DirectX::SimpleMath::Vector3 m_positionOffset = { 0,0,0 };
	bool m_isHit = false;

	static bool isDraw;

public:
	virtual ~Collider() = default;

	virtual void Initialize(GameContext& ctx) = 0;
	virtual void Update(GameContext& context) = 0;
	void Render(GameContext& context);

	virtual bool OnHitCollision(Collider* other) = 0;

	bool IsHit()
	{
		return m_isHit;
	}

	void SetOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		m_positionOffset = offset;
	}

public:
	static void IsDraw(bool t)
	{
		isDraw = IsDraw;
	}
};

class SphereCollider : public Collider
{
private:
	DirectX::BoundingSphere	m_bounding;
	float m_radius;

public:
	void Initialize(GameContext& ctx)override;
	void Update(GameContext& context)override;
	bool OnHitCollision(Collider* other)override;

	void SetRadius(float radius)
	{
		m_radius = radius;
		m_bounding.Radius = radius;
	}

	DirectX::BoundingSphere& GetBounding()
	{
		return m_bounding;
	}
};

class BoxCollider : public Collider
{
private:
	DirectX::BoundingBox	m_bounding;
	DirectX::SimpleMath::Vector3	m_size;

public:
	void Initialize(GameContext& ctx)override;
	void Update(GameContext& context)override;
	bool OnHitCollision(Collider* other)override;

	void SetSize(DirectX::SimpleMath::Vector3& size)
	{
		m_size = size;
		m_bounding.Extents = size;
	}

	DirectX::BoundingBox& GetBounding()
	{
		return m_bounding;
	}
};