#include "variables.h"

#include <unordered_map>
#include <vector>

static std::unordered_map<std::string, std::string> variables;

void setVariable(const std::string& name, const std::string& value){
    variables[name] = value;
}

bool hasVariable(const std::string& name){
    return variables.find(name) != variables.end();
}

std::string getVariable(const std::string& name){
    return variables[name];
}

bool isValidIdentifier(const std::string& name)
{
    if (name.empty())
        return false;

    if (!(std::isalpha(name[0]) || name[0] == '_'))
        return false;

    for (size_t i = 1; i < name.size(); i++){
        if (!(std::isalnum(name[i]) || name[i] == '_'))
            return false;
    }

    return true;
}

std::string expandVariable(const std::string& word){
    std::string result;

    for (size_t i = 0; i < word.size();){
        if (word[i] != '$'){
            result += word[i];
            i++;
            continue;
        }
        i++;

        std::string name;
        if (i < word.size() && word[i] == '{'){
            i++; 

            while (i < word.size() && word[i] != '}'){
                name += word[i];
                i++;
            }

            if (i < word.size() && word[i] == '}')
                i++;
        }
        else{
            while (i < word.size() && (std::isalnum(static_cast<unsigned char>(word[i])) || word[i] == '_')) {
                name += word[i];
                i++;
            }
        }

        if (hasVariable(name))
            result += getVariable(name);
    }

    return result;
}

void expandVariables(std::vector<std::string>& args){
    std::vector<std::string> expanded;

    expanded.push_back(args[0]);

    for (size_t i = 1; i < args.size(); i++){
        std::string value = expandVariable(args[i]);

        if (!value.empty())
            expanded.push_back(value);
    }

    args = std::move(expanded);
}