
#include "Collider.h"

#include "../Frameworks/GameObject.h"

#include "SceneManager.h"

bool Collider::isDraw = false;

void Collider::Render(GameContext & context)
{
	if (isDraw)
	{
		auto& scene = context.Get<SceneManager>().GetActiveScene().Find(L"Camera");
		auto& camera = scene->GetComponent<ICameraComponent>();
		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
		world *= DirectX::SimpleMath::Matrix::CreateScale(m_scale);
		world *= DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->localPosition + m_positionOffset);
		m_colliderRange->Draw(world,
			camera->GetViewMatrix(),
			camera->GetProjectionMatrix(),
			DirectX::Colors::Red,
			nullptr,
			true);
	}
}

void SphereCollider::Initialize(GameContext & ctx)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	m_colliderRange = DirectX::GeometricPrimitive::CreateSphere(context, m_radius * 2);
	float s = (m_radius * 2) / 2;
	m_scale = { s,s,s };
}

void SphereCollider::Update(GameContext& context)
{
	m_bounding.Center = gameObject->transform->localPosition;
}

bool SphereCollider::OnHitCollision(Collider* other)
{
	auto sp = dynamic_cast<SphereCollider*>(other);
	m_hitObjectName = L"null";
	auto name = gameObject->GetName();
	
	if (sp)
	{
		m_isHit = m_bounding.Intersects(sp->GetBounding());
		if (m_isHit)
		{
			m_hitObjectName = other->gameObject->GetName();
		}
		return m_isHit;
	}

	auto box = dynamic_cast<BoxCollider*>(other);
	if (box)
	{
		m_isHit = m_bounding.Intersects(box->GetBounding());
		if (m_isHit)
		{
			m_hitObjectName = other->gameObject->GetName();
		}
		return m_isHit;
	}
	
	return false;
}

void BoxCollider::Initialize(GameContext & ctx)
{
	auto context = ctx.GetDR().GetD3DDeviceContext();
	m_colliderRange = DirectX::GeometricPrimitive::CreateBox(context, m_size);
	m_scale = m_size;
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
		if (m_isHit)
		{
			m_hitObjectName = other->gameObject->GetName();
		}
		return m_isHit;
	}

	auto sp = dynamic_cast<SphereCollider*>(other);
	if (sp)
	{
		m_isHit = m_bounding.Intersects(sp->GetBounding());
		if (m_isHit)
		{
			m_hitObjectName = other->gameObject->GetName();
		}
		return m_isHit;
	}
	m_hitObjectName = L"null";
	return m_isHit;
}
