#pragma once

#include <string>
#include <vector>

void setVariable(const std::string& name, const std::string& value);
bool hasVariable(const std::string& name);
std::string getVariable(const std::string& name);
bool isValidIdentifier(const std::string& name);

std::string expandVariable(const std::string& word);
void expandVariables(std::vector<std::string>& args);