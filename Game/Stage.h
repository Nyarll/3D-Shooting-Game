#pragma once

#include "../Frameworks/Component.h"
#include "MapChip.h"

class Stage : public Component
{
private:
	int m_x, m_y;	// <ステージサイズ>

	std::vector<std::vector<MapChip>>	m_stage;	// <ステージデータ>

	std::unique_ptr<DirectX::Model>	m_model;

	const int roomMinWidth = 5;
	const int roomMaxWidth = 10;

	const int roomMinHeight = 5;
	const int roomMaxHeight = 10;

	const int roomCountMin = 5;
	const int roomCountMax = 10;

	const int meetPointCount = 1;

	const int mapSizeX = 75;
	const int mapSizeY = 75;

public:
	Stage();

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	void SetSize(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

	bool IsPassable(int x, int y)
	{
		if (x == m_x)
			return false;
		if (y == m_y)
			return false;
		if (x < 0)
			return false;
		if (y < 0)
			return false;

		return m_stage[y][x].IsPassable();
	}

	DirectX::SimpleMath::Vector2 GetStageSize()
	{
		return{ (float)m_x, (float)m_y };
	}

	void ChangeStageData(GameContext& context);

private:
	void SpawnPlayer(GameContext& context);
	void SpawnEnemy(GameContext& context);

	void ResetMapData(int width, int height);
	void CreateSpaceData();
	bool CreateRoomData(int roomWidth, int roomHeight, int roomPointX, int roomPointY);
	void CreateRoadData(int roadStartPointX, int roadStartPointY, int meetPointX, int meetPointY);
};