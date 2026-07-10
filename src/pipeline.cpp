#include "pipeline.h"
#include "parser.h"
#include "executeProgram.h"
#include "commandExecutor.h"

#include <string>
#include <vector>
#include <sstream>
#include <array>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

std::string trim(std::string s)
{
    size_t start = s.find_first_not_of(" ");
    if (start == std::string::npos)
        return "";

    size_t end = s.find_last_not_of(" ");
    return s.substr(start, end - start + 1);
}

void executePipeline(const std::string& input)
{
    // Split commands
    std::vector<std::string> commands;

    std::stringstream ss(input);
    std::string command;

    while (std::getline(ss, command, '|'))
        commands.push_back(trim(command));

    int n = commands.size();

    if (n == 0)
        return;

    // Create pipes
    std::vector<std::array<int, 2>> pipes(n - 1);
    for (int i = 0; i < n - 1; i++){
        if (pipe(pipes[i].data()) == -1){
            perror("pipe");
            return;
        }
    }

    std::vector<pid_t> pids;
    for (int i = 0; i < n; i++){
        auto args = parseArguments(commands[i]);

        if (args.empty())
            continue;

        std::string path = findExecutable(args[0]);
        std::vector<char*> argv;
        for (auto& arg : args)
            argv.push_back(const_cast<char*>(arg.c_str()));

        argv.push_back(nullptr);
        pid_t pid = fork();

        if (pid == 0)
        {
            // Read from previous pipe
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Write to next pipe
            if (i < n - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close every pipe
            for (auto& p : pipes)
            {
                close(p[0]);
                close(p[1]);
            }

            // Builtin
            if (isBuiltin(args[0]))
            {
                executeBuiltin(args);
                exit(0);
            }

            // External command
            execv(path.c_str(), argv.data());

            perror("execv");
            exit(1);
        }

        pids.push_back(pid);
    }

    // Parent closes every pipe
    for (auto& p : pipes)
    {
        close(p[0]);
        close(p[1]);
    }

    // Wait for all children
    for (pid_t pid : pids)
    {
        waitpid(pid, nullptr, 0);
    }
}