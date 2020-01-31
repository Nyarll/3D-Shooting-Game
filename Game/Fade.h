#pragma once

#include "../Frameworks/GameSprite.h"

class Fade
{
private:
	std::unique_ptr<GameSpriteEffect>	m_effectSprite;

	bool isStart = false;

	int m_count = 0;
	int m_speed = 2;

	float m_alpha = 0.f;

public:
	void Initialize(GameContext& context);
	void Update(GameContext& context);
	void Render(GameContext& context);

	void Start()
	{
		isStart = true;
	}
	void SetCount(int num)
	{
		m_count = num;
	}
	void SetAlpha(float a)
	{
		m_alpha = a;
	}

	void SetSpeed(int s)
	{
		if (s < 1)s = 1;
		m_speed = s;
	}

	int GetCount() { return m_count; }

	int GetBlackoutTime() { return 180 / m_speed; }
};