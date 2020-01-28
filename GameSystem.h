#pragma once

class GameSystem
{
private:
	static bool DebugMode;

	static DirectX::SimpleMath::Vector2 WindowSize;

public:
	static bool GetDebugMode()
	{
		return DebugMode;
	}
	static void SetDebugMode(bool mode = true)
	{
		DebugMode = mode;
	}

	static void SetWindowSize(float width, float height)
	{
		WindowSize.x = width;
		WindowSize.y = height;
	}

	static DirectX::SimpleMath::Vector2 GetWindowSize()
	{
		return WindowSize;
	}
};