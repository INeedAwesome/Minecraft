#pragma once

#include <string>
#include <fstream>
#include <iostream>

class FileUtil
{
public:
	static std::string ReadFile(const std::string& filepath)
	{
		std::ifstream stream;
		stream.open(filepath.c_str(), std::ios::in);

		if (!stream.is_open())
		{
			std::cout << "File '" << filepath << "' does not exist!" << std::endl;
			return "";
		}

		std::string result = "";
		std::string line = "";
		while (getline(stream, line))
		{
			result += (line + '\n');
		}

		stream.close();

		return result;
	}

private:

};

