
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>

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

vector<string> splitByWhitespace(const string& str) {
    vector<string> tokens;
    istringstream stream(str);
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<char*> createArgv(const vector<string>& vec) {
    vector<char*> ret;
    for (size_t i = 0; i < vec.size(); ++i) {
        ret.push_back(const_cast<char*>(vec[i].c_str()));
    }
    ret.push_back(nullptr); // Null-terminate the array
    return ret;
}

void printArgv(vector<char*> vec) {
    cout << "\nArgv array:\n";
    for (int i = 0; i < vec.size(); i++) {
        cout << i << ") " << vec[i] << endl;
    }
    return;
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
        string beforeEquals = command.substr(0,equalsIndex);
        vector<string> beforeEqualsExecute = splitByWhitespace(beforeEquals);
        int beforeEqualsPipeFDs[2];
        
        // Split the string after the = based on ;
        vector<string> afterEquals = split(command.substr(equalsIndex + 1, command.length()), ";");
        
                // Parse beforeEquals into arguments for execvp

        if (beforeEqualsExecute.empty()) {
            cout << "No command to execute before '='\n";
            continue;
        }

        // Convert vector<string> to array of char* for execvp
        vector<char*> argv = createArgv(beforeEqualsExecute);


        if (pipe(beforeEqualsPipeFDs) < 0) {
            cout << "Error occurred with pipe." << endl;
            continue;
        }

        // Fork a child process
        pid_t pid = fork();
        if (pid == -1) {
            // Fork failed
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Close the read head of the pipe
            if (close(beforeEqualsPipeFDs[0])) {
                cerr << "Error with closing read head of pipe" << endl;
                return EXIT_FAILURE;
            }

            // Redirect child process output to the write head of the pipe
            if (dup2(beforeEqualsPipeFDs[1], STDOUT_FILENO) < 0) {
                cerr << "Error with dup2" << endl;
                return EXIT_FAILURE;
            }
            // Child process executes the command
            execvp(argv[0], argv.data());
            cout << "execvp failed" << endl;
            printArgv(argv);
            return EXIT_FAILURE;
        } else {
            // Parent process waits for the child to finish
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
            } else if (WEXITSTATUS(status) == EXIT_FAILURE) {
                cout << "Error executing before command: " << argv[0] << endl;
                continue;
            }
        }

        // Close the write head of the beforeEquals Pipe
        if (close(beforeEqualsPipeFDs[1]) < 0) {
            cerr << "Error with closing write head of pipe" << endl;
            continue;
        }

        // Read the output from the pipe
        char* output = nullptr;
        size_t output_len = 0;
        ssize_t numRead;
        char buffer[1024];
        while ((numRead = read(beforeEqualsPipeFDs[0], buffer, sizeof(buffer))) > 0) {
            // Resize the output buffer
            char* new_output = static_cast<char*>(realloc(output, output_len + numRead));
            if (new_output == nullptr) {
                perror("realloc failed");
                free(output);
                output = nullptr;
                break;
            }
            output = new_output;

            // Copy the buffer into output
            memcpy(output + output_len, buffer, numRead);
            output_len += numRead;
        }

        if (numRead == -1) {
            perror("read failed");
            free(output);
            output = nullptr;
        }

        // Close the read end of the pipe
        if (close(beforeEqualsPipeFDs[0]) < 0) {
            perror("close failed");
        }

        // Null-terminate the output
        if (output != nullptr) {
            // Allocate extra byte for null terminator
            char* new_output = static_cast<char*>(realloc(output, output_len + 1));
            if (new_output == nullptr) {
                perror("realloc failed");
                free(output);
                continue;
            }
            output = new_output;
            output[output_len] = '\0';
        } else {
            cout << "No output from child process." << endl;
            continue;
        }

    // Now use the 'output' variable as the input to the after commands
    // Execute the command by constructing the string executing the command with popen
        bool errorOccurred = false;
        int afterEqualsPipeFDs[afterEquals.size()][2];
        for (int i = 0; i < afterEquals.size(); i++) {
            if (pipe(afterEqualsPipeFDs[i]) < 0) {
                cerr << "Error with after equals pipes" << endl;
                errorOccurred = true;
                break;
            }

            pid = fork();
            if (pid < 0) {
                cerr << "Error occurred with fork" << endl;
                errorOccurred = true;
                break;
            }

            if (pid == 0) {
                // Child process
                // Close write end of the pipe
                if (close(afterEqualsPipeFDs[i][1]) < 0) {
                    perror("close write end failed in child");
                    exit(EXIT_FAILURE);
                }

                // Redirect stdin from the read end of the pipe
                if (dup2(afterEqualsPipeFDs[i][0], STDIN_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }

                // Close the original read end
                if (close(afterEqualsPipeFDs[i][0]) < 0) {
                    perror("close read end failed in child");
                    exit(EXIT_FAILURE);
                }

                // Parse and execute the command
                vector<string> afterEqualsExecute = splitByWhitespace(afterEquals[i]);
                cout << "\nExecuting: " << afterEquals[i];
                cout << "\n----------------\n";
                argv = createArgv(afterEqualsExecute);
                execvp(argv[0], argv.data());
                perror("execvp failed");
                exit(EXIT_FAILURE);
            } else {
                // Parent process
                // Close read end of the pipe
                if (close(afterEqualsPipeFDs[i][0]) < 0) {
                    perror("close read end failed in parent");
                    errorOccurred = true;
                    break;
                }

                // Write output to write end of the pipe
                ssize_t bytesWritten = write(afterEqualsPipeFDs[i][1], output, output_len);
                if (bytesWritten < 0) {
                    perror("Error writing to pipe");
                    errorOccurred = true;
                    break;
                } else if (bytesWritten != output_len) {
                    cerr << "Partial write to pipe" << endl;
                    errorOccurred = true;
                    break;
                }

                // Close write end of the pipe
                if (close(afterEqualsPipeFDs[i][1]) < 0) {
                    perror("Error closing write end of pipe");
                    errorOccurred = true;
                    break;
                }

                // Wait for the child process
                waitpid(pid, NULL, 0);
            }
        }
        if (errorOccurred) {continue;}
    }
    return 0;
}