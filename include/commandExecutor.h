#pragma once

#include <vector>
#include <string>


bool isBuiltin(const std::string& command);
void executeBuiltin(const std::vector<std::string>& args);
bool executeCommand(const std::vector<std::string>& args, bool background, const std::string& commandLine);