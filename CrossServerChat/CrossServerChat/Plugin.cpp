#include "Plugin.h"

Plugin& Plugin::Get() {
	static Plugin instance;

	return instance;
}
bool Plugin::isNum(std::string str)
{
	for (int i = 0; i < str.size(); i++)
	{
		int tmp = (int)str[i];
		if (tmp >= 48 && tmp <= 57)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}