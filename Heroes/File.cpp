#include "stdafx.h"
#include "File.h"

std::vector<std::string> GetFilesInDirectory(const std::string& directory, bool returnFiles)
{
	std::vector<std::string> files;
	HANDLE dir;
	WIN32_FIND_DATA file_data;
	std::wstring wdirectory(directory.begin(), directory.end());
	if ((dir = FindFirstFile((wdirectory + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
	{
		return files;
	}

	do {
		const std::wstring file_name = file_data.cFileName;
		const std::wstring full_file_name = wdirectory + L"/" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		
		if (is_directory == returnFiles)
			continue;

		std::string fileName(full_file_name.begin(), full_file_name.end());
		files.push_back(fileName);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
	return files;
}

std::vector<std::string> GetDirectoriesInDirectory(const std::string& directory)
{
	return GetFilesInDirectory(directory, false);
}

std::vector<std::string> GetFilesInDirectory(const std::string& directory)
{
	return GetFilesInDirectory(directory, true);
}