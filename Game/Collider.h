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
	// <デバッグ表示用>
	std::unique_ptr<DirectX::GeometricPrimitive>	m_colliderRange;

	DirectX::SimpleMath::Vector3 m_scale = { 1.f,1.f,1.f };
	DirectX::SimpleMath::Vector3 m_positionOffset = { 0,0,0 };
	// <当たっているかどうか>
	bool m_isHit = false;
	// <当たった対象の名称>
	std::wstring m_hitObjectName = L"null";

	// <デバッグ表示するかどうか>
	static bool isDraw;

public:
	// <コンストラクタ>
	Collider() = default;
	// <デストラクタ>
	virtual ~Collider() = default;

	// <初期化>
	virtual void Initialize(GameContext& ctx) = 0;
	// <更新>
	virtual void Update(GameContext& context) = 0;
	// <描画>
	void Render(GameContext& context);

	// <当たり判定>
	virtual bool OnHitCollision(Collider* other) = 0;

	// <当たっているかどうかの状態取得>
	bool IsHit()
	{
		return m_isHit;
	}

	// <当たり判定の中心をずらす>
	void SetOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		m_positionOffset = offset;
	}
	// <当たった対象の名称を取得>
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
	// <デバッグ表示切替>
	static void IsDraw(bool t)
	{
		isDraw = t;
	}
};

// <DirectXCollisionを使用>

// <SphereCollider Component>
class SphereCollider : public Collider
{
private:
	// <当たり判定用オブジェクト>
	DirectX::BoundingSphere	m_bounding;
	// <半径>
	float m_radius;

public:
	// <初期化>
	void Initialize(GameContext& ctx)override;
	// <更新>
	void Update(GameContext& context)override;
	// <当たり判定>
	bool OnHitCollision(Collider* other)override;
	// <半径登録>
	void SetRadius(float radius)
	{
		m_radius = radius;
		m_bounding.Radius = radius;
	}
	// <当たり判定用オブジェクトの取得>
	DirectX::BoundingSphere& GetBounding()
	{
		return m_bounding;
	}
};

// <BoxCollider Component>
class BoxCollider : public Collider
{
private:
	// <当たり判定用オブジェクト>
	DirectX::BoundingBox	m_bounding;
	// <箱のサイズ>
	DirectX::SimpleMath::Vector3	m_size;

public:
	// <初期化>
	void Initialize(GameContext& ctx)override;
	// <更新>
	void Update(GameContext& context)override;
	// <当たり判定>
	bool OnHitCollision(Collider* other)override;
	// <サイズ登録>
	void SetSize(DirectX::SimpleMath::Vector3& size)
	{
		m_size = size;
		m_bounding.Extents = size;
	}
	// <当たり判定用オブジェクトの取得>
	DirectX::BoundingBox& GetBounding()
	{
		return m_bounding;
	}
};