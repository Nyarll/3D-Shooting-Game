#pragma once

class GameSystem
{
private:
	static bool DebugMode;

public:
	static bool GetDebugMode()
	{
		return DebugMode;
	}
	static void SetDebugMode(bool mode = true)
	{
		DebugMode = mode;
	}
};