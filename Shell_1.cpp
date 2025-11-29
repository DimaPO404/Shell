#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

using namespace std;

string get_history_path() {
    const char* home = getenv("HOME");
    if (home == nullptr) {
        cerr << "HOME не найден, история сохраняется локально\n";
        return "./.kubsh_history";
    }
    return string(home) + "/.kubsh_history";
}

void load_history(vector<string>& history) {
    string path = get_history_path();
    ifstream file(path);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (!line.empty()) history.push_back(line);
    }
}

void save_history(const vector<string>& history) {
    string path = get_history_path();
    ofstream file(path, ios::trunc);
    if (!file.is_open()) {
        cerr << "Не удалось открыть " << path << " для записи\n";
        return;
    }

    for (const string& cmd : history) {
        file << cmd << "\n";
    }
}

static string strip_quotes(const string& s) {
    if (s.size() >= 2) {
        if ((s.front() == '"' && s.back() == '"') ||
            (s.front() == '\'' && s.back() == '\'')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}

void my_echo(const string& input) {
    size_t pos = 4;
    while (pos < input.size() && isspace((unsigned char)input[pos])) ++pos;

    if (pos >= input.size()) {
        cout << '\n';
        return;
    }

    string args = input.substr(pos);
    cout << strip_quotes(args) << '\n';
}

int main() {
    vector<string> history;
    load_history(history);

    string input;

    while (true) {

        if (!getline(cin, input)) {
            if (cin.eof()) {
                cout << "\nExit by Ctrl+D\n";
                break;
            }
            cin.clear();
            continue;
        }

        if (input.empty()) {
            continue;
        }

        history.push_back(input);

        if (input == "\\q") {
            cout << "Exit by '\\q'\n";
            break;
        }

        if (input == "history") {
            cout << "История команд:\n";
            for (size_t i = 0; i < history.size(); ++i) {
                cout << (i + 1) << ": " << history[i] << '\n';
            }
            continue;
        }

        if (input.rfind("echo", 0) == 0) {
            my_echo(input);
            continue;
        }

        cout << "Неизвестная команда: " << input << "\n";
    }

    save_history(history);
    return 0;
}
