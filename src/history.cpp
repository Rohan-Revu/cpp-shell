#include "history.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>

static std::vector<std::string> history;
static size_t lastWritten = 0;


const std::vector<std::string>& getHistory(){
    return history;
}

void addHistory(const std::string& command){
    history.push_back(command);
}

void printHistory(int n){
    int start = 0;

    if (n != -1){
        start = std::max(0, (int)history.size() - n);
    }

    for (int i = start; i < history.size(); i++){
        std::cout << std::setw(5) << i + 1 << "  " << history[i] << std::endl;
    }
}

void readHistory(const std::string& filename){
    std::ifstream file(filename);

    if (!file.is_open())
        return;

    std::string line;

    while (std::getline(file, line)){
        if (!line.empty())
            addHistory(line);
    }
}

void writeHistory(const std::string& filename){
    std::ofstream file(filename);

    if (!file.is_open())
        return;

    for (const auto& command : history){
        file << command << '\n';
    }
}



void appendHistory(const std::string& filename){
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
        return;

    for (size_t i = lastWritten; i < history.size(); i++){
        file << history[i] << '\n';
    }

    lastWritten = history.size();
}