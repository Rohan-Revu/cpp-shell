#include <iostream>
#include <string>


#include "parser.h"
#include "executeProgram.h"
#include "builtins.h"
#include "redirection.h"
#include "commandExecutor.h"
#include "autoComplete.h"
#include "terminal.h"
#include "jobs.h"
#include "pipeline.h"
#include "history.h"
#include "variables.h"




int main() {


    initializeTerminal();

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const char* histfile = getenv("HISTFILE");
    if (histfile != nullptr){
        readHistory(histfile);
    }

    while (true) {
        reapJobs();

        std::cout << "$ ";

        std::string input = readCommand();

        if(!input.empty()){
            addHistory(input);
        }

        if (input.find('|') != std::string::npos) {
            executePipeline(input);
            continue;
        }

        ParsedCommand command = parseCommand(input);

        expandVariables(command.args);

        Redirection redir = parseRedirection(command.args);

        setupRedirection(redir);

        if (command.args.empty()) {
            restoreRedirection(redir);
            continue;
        }

        if (executeCommand(command.args, command.background, input)) {
            restoreRedirection(redir);
            if (histfile != nullptr){
                writeHistory(histfile);
            }
            break;
        }

        restoreRedirection(redir);
    }

    restoreTerminal();
}