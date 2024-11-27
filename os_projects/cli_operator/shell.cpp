
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

vector<string> split(string str, string delimeter) {
    vector<string> strings;
    int start = 0;
    int end = str.find(delimeter);
    while (end != string::npos) {
        strings.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimeter, start);
    }
    strings.push_back(str.substr(start));
    return strings;
}

int main() {
    cout << "Welcome to a custom shell!\nEnter a command when ready..."  << endl;
    while (true) {
        cout << "SHELL $ ";

        // Get the command from the user
        string command;
        getline(cin,command);
        if (command == "q") {
            cout << "Exiting Shell\n";
            break;
        }

        // Ensure that the command has an = and no illegal characters
        int equalsIndex = command.find("=");
        if (equalsIndex == string::npos) {
            cout << "= operator not found! Enter a new command..." << endl;
            continue;
        }

        bool hasIllegalChar = false;
        for (int i = 0; i < command.length(); i++) {
            if (command[i] == '<' || command[i] == '>' || command[i] == '&') {
                printf("Illegal character [%c] at position %d\n", command[i], i);
                hasIllegalChar = true;
            }
        }
        if (hasIllegalChar) {continue;}

        // Split the input based on the =
        string beforeEquals = command.substr(0,equalsIndex
        );
        // Split the string after the = based on ;
        vector<string> afterEquals = split(command.substr(equalsIndex + 1, command.length()), ";");
        
        // Execute the command by constructing the string executing the command with popen
        bool errorOccurred = false;
        for (int i = 0; i < afterEquals.size(); i++) {
            string fullCommand = beforeEquals + "|" + afterEquals[i];
            cout << "\nExecuting: " << afterEquals[i] << endl;
            FILE* pipe = popen(fullCommand.c_str(), "r");
            if (!pipe) {
               cout << "Error occurred while processing..." << endl;
               errorOccurred = true;
               break; 
            }
            cout << "----------------------------" << endl;
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                cout << buffer;
            }
            cout<<endl;
            pclose(pipe);
        }
        if (errorOccurred) {continue;}        
    }
    return 0;
}