#include "builtins.h"
#include "executeProgram.h"
#include "completeRegistry.h"
#include "jobs.h"
#include "history.h"
#include "variables.h"

#include <iostream>
#include <unordered_set>
#include <unistd.h>
#include <unordered_map>


void echoCommand(const std::vector<std::string> &args){
  for (size_t i = 1; i < args.size(); i++) {
    if (i > 1)
        std::cout << " ";

    std::cout << args[i];
  }
  std::cout << std::endl;
}


void pwdCommand(){
  std::cout << getcwd(nullptr, 0) << std::endl;
}


void cdCommand(std::string directory){
  if(directory == "~"){
    directory = getenv("HOME");
  }

  if(chdir(directory.c_str()) != 0){
    std::cout << "cd: " << directory << ": No such file or directory" << std::endl;
  }

}


void typeCommand(const std::string &cmd) {
    std::unordered_set<std::string> builtins = {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd",
        "complete",
        "jobs",
        "history",
        "declare"
    };

    if (builtins.contains(cmd)) {
        std::cout << cmd << " is a shell builtin" << std::endl;
        return;
    }

    std::string path = findExecutable(cmd);

    if (!path.empty())
        std::cout << cmd << " is " << path << std::endl;
    else
        std::cout << cmd << ": not found" << std::endl;
}

void completeCommand(const std::vector<std::string> &args){

  if(args.size() >= 4){
    if(args[1] == "-C"){
      registerCompleter(args[3], args[2]);
    }
  }
  if(args.size() >= 3){
    if(args[1] == "-p"){
      std::string completer = getCompleter(args[2]);

      if(!completer.empty()){
        std::cout << "complete -C '" << completer << "' " << args[2] << std::endl;
      }
      else{
        std::cout << "complete: " << args[2] << ": no completion specification" << std::endl;
      }
    }
  }
  if(args.size() >= 2){
    if(args[1] == "-r"){
      unregisterCompleter(args[2]);
    }
  }
}

void jobCommand() {
    printJobs();
}






// bool isValidIdentifier(const std::string& name){
//     if (name.empty())
//         return false;
  
//     if (!(std::isalpha(name[0]) || name[0] == '_'))
//         return false;

//     for (size_t i = 1; i < name.size(); i++){
//         if (!(std::isalnum(name[i]) || name[i] == '_'))
//             return false;
//     }

//     return true;
// }
void declareCommand(const std::vector<std::string>& args){
    if (args.size() < 2)
        return;

    if (args[1] == "-p"){
        if (args.size() < 3)
            return;

        std::string name = args[2];

        if (hasVariable(name)){
            std::cout << "declare -- " << name << "=\"" << getVariable(name) << "\"" << std::endl;
        }
        else{
            std::cout << "declare: " << name << ": not found" << std::endl;
        }

        return;
    }

    std::string assignment = args[1];
    size_t pos = assignment.find('=');

    if (pos == std::string::npos)
        return;

    std::string name = assignment.substr(0, pos);
    std::string value = assignment.substr(pos + 1);

    if (!isValidIdentifier(name)){
        std::cout << "declare: `" << assignment << "': not a valid identifier" << std::endl;
        return;
    }

    setVariable(name, value);
}
