#pragma once

#include "../Frameworks/Component.h"

class Stage : public Component
{
private:
	int m_x, m_y;	// <�X�e�[�W�T�C�Y(�f�t�H���g�T�C�Y��10)>

	std::vector<std::vector<int>>	m_stage;	// <�X�e�[�W�f�[�^>

	std::unique_ptr<DirectX::Model>	m_model;

public:
	Stage();

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;

	void SetSize(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
};