#pragma once

#include "../Frameworks/Component.h"

class StatusComponent : public Component
{
private:
	std::string m_entityName = "null : Please set a name.";
	int m_maxHP = 1, m_nowHP = 1;
	int m_atk = 1;
	int m_def = 1;

	int m_level = 1;

public:
	StatusComponent() = default;
	StatusComponent(std::string _name)
		: m_entityName(_name) {}
	StatusComponent(std::string _name, int _maxHP, int _atk, int _def)
		: m_entityName(_name), m_maxHP(_maxHP), m_nowHP(_maxHP), m_atk(_atk), m_def(_def) {}

	void SetEntityName(std::string _name) { m_entityName = _name; }
	std::string GetEntityName() { return m_entityName; }

	void SetMaxHP(int _maxHP) { m_maxHP = _maxHP; }
	int GetMaxHP() { return m_maxHP; }
	void SetNowHP(int _nowHP) { m_nowHP = _nowHP; }
	int GetNowHP() { return m_nowHP; }

	void SetATK(int _atk) { m_atk = _atk; }
	void SetDEF(int _def) { m_def = _def; }
	int GetATK() { return m_atk; }
	int GetDEF() { return m_def; }

	void SetLevel(int _lv) { m_level = _lv; }
	int GetLevel() { return m_level; }
};