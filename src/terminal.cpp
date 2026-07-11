#include "terminal.h"
#include "autoComplete.h"
#include "completeRegistry.h"
#include "parser.h"
#include "history.h"

#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cerrno> 


void initializeTerminal(){
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void restoreTerminal(){
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    orig_termios.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

std::string readCommand(){
    std::string input;
    char c;
    bool lastWasTab = false;

    const auto& history = getHistory();
    int historyIndex = history.size();

    while (true) {
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n < 0) {
            if (errno == EINTR) {
                input.clear();
                return "";
            }
            break;
        }

        if (n == 0)
            break;
       
        if (c == '\n'){
          std::cout << std::endl;
            break;
        }

        //handle backspace
        if (c == 127 || c == '\b') {
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b" << std::flush;
            }
            lastWasTab = false;
            continue;
        }

        //handle escape
        if (c == '\033'){
            char seq[2];

            if (read(STDIN_FILENO, &seq[0], 1) <= 0)
                continue;

            if (read(STDIN_FILENO, &seq[1], 1) <= 0)
                continue;

            if (seq[0] == '['){
                // Up Arrow
                if (seq[1] == 'A'){
                    if (!history.empty() && historyIndex > 0){
                        historyIndex--;

                        std::cout << "\r";
                        std::cout << "$ ";
                        std::cout << "\033[K";

                        input = history[historyIndex];
                        std::cout << input;
                    }
                }

                // Down Arrow (later)
                else if (seq[1] == 'B'){
                    if (!history.empty() && historyIndex < history.size() - 1){
                        historyIndex++;

                        std::cout << "\r";
                        std::cout << "$ ";
                        std::cout << "\033[K";

                        input = history[historyIndex];
                        std::cout << input;
                    }
                }
            }

            continue;
        }
        if (c == '\t') {

            size_t pos = input.find_last_of(' ');
            std::string prefix;

            std::vector<std::string> matches;

            if (pos == std::string::npos)
            {
                // Completing command
                prefix = input;
                matches = getCompletions(prefix);
            }
           else
            {
                prefix = input.substr(pos + 1);

                // Parse the whole command line
                std::vector<std::string> words = parseArguments(input);

                std::string command = words[0];

                // Does the input end with a space?
                bool endsWithSpace = !input.empty() && input.back() == ' ';

                // Current word being completed
                std::string currentWord;

                if (endsWithSpace)
                    currentWord = "";
                else
                    currentWord = words.back();

                // Previous word
                std::string previousWord;

                if (endsWithSpace)
                {
                    if (!words.empty())
                        previousWord = words.back();
                    else
                        previousWord = "";
                }
                else
                {
                    if (words.size() >= 2)
                        previousWord = words[words.size() - 2];
                    else
                        previousWord = "";
                }

                if (!getCompleter(command).empty())
                {
                    matches = runCompleter(command, currentWord, previousWord, input);
                }
                else
                {
                    matches = getFileCompletions(prefix);
                }
            }

            if (matches.empty()) {
                std::cout << '\a';
            }
            else if (matches.size() == 1) {
                std::string remain = matches[0].substr(prefix.size());

                bool isDirectory =
                    !matches[0].empty() &&
                    matches[0].back() == '/';

                if (isDirectory){
                    std::cout << remain;

                    if (pos == std::string::npos)
                        input = matches[0];
                    else
                        input = input.substr(0, pos + 1) + matches[0];
                }
                else{
                    std::cout << remain << ' ';

                    if (pos == std::string::npos)
                        input = matches[0] + ' ';
                    else
                        input = input.substr(0, pos + 1) + matches[0] + ' ';
                }
                lastWasTab = false;
            }
            else {
              std::string lcp = longestCommonPrefix(matches);
                if (lcp.size() > prefix.size()) {

                    std::string remain = lcp.substr(prefix.size());
                    std::cout << remain;

                    if (pos == std::string::npos)
                        input = lcp;
                    else
                        input = input.substr(0, pos + 1) + lcp;

                    lastWasTab = false;
            }
            else {
                if (!lastWasTab) {
                    std::cout << '\a';
                    lastWasTab = true;
                  }
                else {
                    std::cout << '\n';
                    for (const auto &m : matches)
                        std::cout << m << "  ";
                    std::cout << "\n$ " << input;
                    lastWasTab = false;
                }
            }
          }
          continue;
        }

        input += c;
        std::cout << c;
        lastWasTab = false;
    }
    return input;
}