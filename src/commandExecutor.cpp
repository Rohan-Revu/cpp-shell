#include "commandExecutor.h"
#include "history.h"
#include "builtins.h"
#include "executeProgram.h"

#include <iostream>
#include <string>
#include <vector>




bool isBuiltin(const std::string& command)
{
    return command == "echo" ||
           command == "type" ||
           command == "pwd" ||
           command == "cd" ||
           command == "complete" ||
           command == "jobs" ||
           command == "exit" ||
           command == "history" || 
           command == "declare";
}

void executeBuiltin(const std::vector<std::string>& args){
    std::string command = args[0];

    if (command == "echo")
        echoCommand(args);

    else if (command == "type")
        typeCommand(args[1]);

    else if (command == "pwd")
        pwdCommand();

    else if (command == "cd")
        cdCommand(args[1]);

    else if (command == "complete")
        completeCommand(args);

    else if (command == "jobs")
        jobCommand();

    else if (command == "history"){
        if (args.size() == 1){
            printHistory();
        }
        else if (args[1] == "-r"){
            if (args.size() >= 3)
                readHistory(args[2]);
        }
        else if(args[1] == "-w"){
            if (args.size() >= 3)
                writeHistory(args[2]);
        }
        else if(args[1] == "-a"){
            if (args.size() >= 3)
                appendHistory(args[2]);
        }
        else{
            printHistory(std::stoi(args[1]));
        }
    }
    else if (command == "declare"){
        declareCommand(args);
    }
}


bool executeCommand(const std::vector<std::string>& args, bool background, const std::string& commandLine){
    std::string command = args[0];

    if (command == "exit")
        return true;

    if (isBuiltin(command)) {
        executeBuiltin(args);
        return false;
    }

    std::string path = findExecutable(command);

    if (path.empty()){
        std::cout << command << ": not found" << std::endl;
    }
    else {
        executeProgram(path, args, background, commandLine);
    }

    return false;
}