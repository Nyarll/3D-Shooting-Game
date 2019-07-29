#pragma once

#include <string>

class Object
{
public:
	static float ObjectTime;

protected:
	float m_time = -1;

public:
	Object() = default;
	virtual ~Object() = default;

	virtual std::wstring GetType()const
	{
		return L"Object";
	}

	virtual std::wstring GetName()const
	{
		return L"Object";
	}

	virtual bool IsDestroyed()
	{
		return (m_time >= 0 && ObjectTime >= m_time);
	}

	static void Destroy(Object& object, float delay = 0)
	{
		if (object.m_time < 0)
			object.m_time = ObjectTime + delay;
	}
};