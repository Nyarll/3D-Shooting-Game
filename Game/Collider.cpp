
#include "Collider.h"

#include "../Frameworks/GameObject.h"

bool Collider::isDraw = false;

void Collider::Render(GameContext & context)
{
	auto& camera = context.GetFollowCamera();
	m_colliderRange->Draw(gameObject->transform->GetMatrix(),
		camera.m_view,
		camera.m_proj,
		DirectX::Colors::Red,
		nullptr,
		true);
}

void SphereCollider::Initialize(GameContext & ctx)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	m_colliderRange = DirectX::GeometricPrimitive::CreateSphere(context, m_radius * 2);
}

void SphereCollider::Update(GameContext& context)
{
	m_bounding.Center = gameObject->transform->localPosition;
}

bool SphereCollider::OnHitCollision(Collider* other)
{
	auto sp = dynamic_cast<SphereCollider*>(other);
	if (sp)
	{
		m_isHit = m_bounding.Intersects(sp->GetBounding());
		return m_isHit;
	}

	auto box = dynamic_cast<BoxCollider*>(other);
	if (box)
	{
		m_isHit = m_bounding.Intersects(box->GetBounding());
		return m_isHit;
	}

	return false;
}

void BoxCollider::Initialize(GameContext & ctx)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	m_colliderRange = DirectX::GeometricPrimitive::CreateBox(context, m_size);
}

void BoxCollider::Update(GameContext& context)
{
	m_bounding.Center = gameObject->transform->localPosition;
}

bool BoxCollider::OnHitCollision(Collider * other)
{
	m_isHit = false;
	auto box = dynamic_cast<BoxCollider*>(other);
	if (box)
	{
		m_isHit = m_bounding.Intersects(box->GetBounding());
		return m_isHit;
	}

	auto sp = dynamic_cast<SphereCollider*>(other);
	if (sp)
	{
		m_isHit = m_bounding.Intersects(sp->GetBounding());
		return m_isHit;
	}

	return m_isHit;
}
