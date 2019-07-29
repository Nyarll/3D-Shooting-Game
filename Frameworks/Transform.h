#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Property.h"

// <Transform Class>
class Transform : public Component
{
public:
	// <ローカル位置>
	DirectX::SimpleMath::Vector3 localPosition;
	// <ローカル回転>
	DirectX::SimpleMath::Quaternion localRotation;
	// <ローカルスケール>
	DirectX::SimpleMath::Vector3 localScale;
	// <親オブジェクトの情報>
	const Transform* parent;

	// <位置>
	Property<DirectX::SimpleMath::Vector3> position
	{
		[&]()
		{
			if (parent)
				return DirectX::SimpleMath::Vector3::Transform(localPosition, parent->GetMatrix());
			else
				return localPosition;
		},
		[&](const auto& value)
		{
			if (parent)
				localPosition = DirectX::SimpleMath::Vector3::Transform(value, parent->GetMatrix().Invert());
			else
				localPosition = value;
		}
	};
	// <回転>
	Property<DirectX::SimpleMath::Quaternion> rotation
	{
		[&]()
		{
			if (parent)
				return localRotation * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(parent->GetMatrix());
			else
				return localRotation;
		},
		[&](const auto& value)
		{
			if (parent)
				localRotation = value * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(parent->GetMatrix().Invert());
			else
				localRotation = value;
		}
	};
	// <スケール>
	Property<DirectX::SimpleMath::Vector3> lossyScale
	{
		[&]()
		{
			if (parent)
			{
				DirectX::SimpleMath::Vector3 parentPosition;
				DirectX::SimpleMath::Quaternion parentRotation;
				DirectX::SimpleMath::Vector3 parentScale;
				parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
				auto mat = DirectX::SimpleMath::Matrix::CreateScale(parentScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentRotation);
				return DirectX::SimpleMath::Vector3::Transform(localScale, mat);
			}
			else
				return localScale;
		},
		[&](const auto& value)
		{
			if (parent)
			{
			DirectX::SimpleMath::Vector3 parentPosition;
			DirectX::SimpleMath::Quaternion parentRotation;
			DirectX::SimpleMath::Vector3 parentScale;
			parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
			auto mat = DirectX::SimpleMath::Matrix::CreateScale(parentScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentRotation);
			localScale = DirectX::SimpleMath::Vector3::Transform(value, mat.Invert());
			}
			else
				localScale = value;
		}
	};

	void SetParent(const Transform* value)
	{
		DirectX::SimpleMath::Vector3 worldPosition = position;
		DirectX::SimpleMath::Quaternion worldRotation = rotation;
		DirectX::SimpleMath::Vector3 worldScale = lossyScale;
		parent = value;
		position = worldPosition;
		rotation = worldRotation;
		lossyScale = worldScale;
	}

	DirectX::SimpleMath::Matrix GetMatrix()const
	{
		return{
			DirectX::SimpleMath::Matrix::CreateScale(localScale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(localRotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(localPosition) *
			(parent != nullptr ? parent->GetMatrix() : DirectX::SimpleMath::Matrix::Identity)
		};
	}
};