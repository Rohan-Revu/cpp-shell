#pragma once

#include <string>
#include <vector>

void addHistory(const std::string& command);
void printHistory(int n = -1);

const std::vector<std::string>& getHistory();
void readHistory(const std::string& filename);
void writeHistory(const std::string& filename);
void appendHistory(const std::string& filename);
