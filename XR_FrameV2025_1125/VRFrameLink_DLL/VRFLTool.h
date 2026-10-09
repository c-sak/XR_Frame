#pragma once

#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace VRFL
{ 
	inline std::vector<std::string> SplitString(std::string src, char separator)
	{
		std::vector<std::string> res;
		int offset = 0, pos;

		while ((pos = (int)src.find_first_of(separator, offset)) != std::string::npos)
		{
			int len = pos - offset;

			res.push_back(src.substr(offset, len));

			offset = pos + 1;
		}
		if ((int)src.length() > offset) res.push_back(src.substr(offset));

		// �󔒍폜
		for (int i = 0; i < (int)res.size(); i++)
		{
			int sp, ep;

			sp = (int)res[i].find_first_not_of(" \t\r\n");
			ep = (int)res[i].find_last_not_of(" \t\r\n");

			if (sp == std::string::npos || ep == std::string::npos)
			{
				res[i] = "";
			}
			else if (sp != 0 || ep != res[i].length() - 1)
			{
				if (ep > sp) res[i] = res[i].substr(sp, ep - sp + 1);
			}
		}

		return res;
	}

	inline double DegToRad(double val)
	{
		return val * M_PI / 180.0;
	}

	bool GetFullPath(const char* filename, char* dstbuf, int bufsize, const char* relativebase);
}
