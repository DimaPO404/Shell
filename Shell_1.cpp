#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

vector<string> load_history() {
    vector<string> history;
    ifstream file("kubsh_history.txt"); 
    string line;
    
    while (getline(file, line)) {
        if (!line.empty()) {
            history.push_back(line);
        }
    }
    return history;
}

void save_history(const vector<string>& history) {
    ofstream file("kubsh_history.txt");
    for (const string& cmd : history) {
        file << cmd << "\n";
    }
}

void my_echo(const string& input) {
    size_t echo_pos = input.find("echo");
    if (echo_pos == string::npos) {
        return;
    }
    
    size_t text_start = echo_pos + 4;
    
    while (text_start < input.length() && input[text_start] == ' ') {
        text_start++;
    }
    
    if (text_start < input.length()) {
        string text = input.substr(text_start);
        cout << text << '\n';
    } else {
        cout << '\n';
    }
}

int main() {
    vector<string> history = load_history();
    string input;
    
    while (true) {
        cout << "Cmd: ";
        
        if (!getline(cin, input)) {
            if (cin.eof()) {
                cout << "Exit by Ctrl+d" << '\n';
                break;
            }
            cin.clear();
            continue;
        }
        
        if (input == "\\q") {
            cout << "Exit by '\\q'" << '\n';
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        history.push_back(input);
        
        if (input == "history") {
            cout << "История команд:" << '\n';
            for (size_t i = 0; i < history.size(); i++) {
                cout << i+1 << ": " << history[i] << '\n';
            }
            continue;
        }
        
        if (input.find("echo") == 0) {
            my_echo(input);
            continue;
        }
        
        cout << input << '\n';
    }
    
    save_history(history);
    
    return 0;
}
