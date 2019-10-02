#pragma once
#include "CommonStates.h"
#include "PrimitiveBatch.h"
#include "Effects.h"
#include "VertexTypes.h"
#include "SimpleMath.h"

class GameContext;

// <�����O���b�h��>
class InfinityGridFloor
{
	// <�G�t�F�N�g>
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// <�v���~�e�B�u�o�b�`>
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// <�C���v�b�g���C�A�E�g>
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// <1�}�X�̈�ӂ̃T�C�Y>
	float m_cellsize;

	// <�T�C�Y>
	DirectX::SimpleMath::Vector2 m_size;

public:
	// <�R���X�g���N�^>
	InfinityGridFloor();
	
	// <�f�X�g���N�^>
	~InfinityGridFloor();

	void init(GameContext& context, float cellsize, const DirectX::SimpleMath::Vector2& size);
	// <�`��>
	void draw(GameContext& context, DirectX::GXMVECTOR color = DirectX::Colors::Gray);
};