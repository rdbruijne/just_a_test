#pragma once

#include <optional>
#include <string>

// wrappers
void SplitPath(const std::string& path,
			   std::optional<std::reference_wrapper<std::string>> drive,
			   std::optional<std::reference_wrapper<std::string>> dir,
			   std::optional<std::reference_wrapper<std::string>> filename,
			   std::optional<std::reference_wrapper<std::string>> ext);

std::string MakePath(std::optional<std::reference_wrapper<const std::string>> drive,
					 std::optional<std::reference_wrapper<const std::string>> dir,
					 std::optional<std::reference_wrapper<const std::string>> filename,
					 std::optional<std::reference_wrapper<const std::string>> ext);

// path components
std::string PathComponents(const std::string& path, bool drive, bool dir, bool filename, bool ext);
std::string Drive(const std::string& path);
std::string Directory(const std::string& path);
std::string FileName(const std::string& path);
std::string FileExtension(const std::string& path);
std::string FileNameExt(const std::string& path);

// path locations
std::string CurrentDirectory();
std::string GlobalPath(const std::string& path);
std::string RelativeFilePath(const std::string& path);

// file handling
std::string ReadFile(const std::string filePath);
void WriteFile(const std::string filePath, const std::string& text);
bool FileExists(const std::string& filePath);

// file times
uint64_t FileCreateTime(const std::string& filePath);
uint64_t FileLastAccessTime(const std::string& filePath);
uint64_t FileLastWriteTime(const std::string& filePath);
uint64_t FileSize(const std::string& filePath);
