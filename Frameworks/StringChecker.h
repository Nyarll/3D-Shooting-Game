#pragma once

namespace StringChecker
{
	bool endWith(std::string& str, std::string path)
	{
		int p = path.size() - 1;
		for (int i = str.size() - 1; i > -1; i--)
		{
			if (p < 0)
			{
				return true;
			}
			if (str[i] != path[p])
			{
				return false;
			}
			p--;
		}
	}

	bool HasPNGExtension(std::string& str)
	{
		return (endWith(str, ".png") || endWith(str, ".PNG"));
	}

	bool HasJPGExtension(std::string& str)
	{
		return (endWith(str, ".jpg") || endWith(str, ".JPG"));
	}

	bool HasMP3Extension(std::string& str)
	{
		return (endWith(str, ".mp3") || endWith(str, ".MP3"));
	}

	bool HasWAVExtension(std::string& str)
	{
		return (endWith(str, ".wav"));
	}
}