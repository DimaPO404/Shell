#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

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

void my_env(const string& input) {
    size_t pos = 2;
    while (pos < input.size() && isspace((unsigned char)input[pos])) ++pos;
    if (pos >= input.size()) {
        cout << "Использование: \\e $VAR или \\e VAR\n";
        return;
    }

    size_t start = pos;
    while (pos < input.size() && !isspace((unsigned char)input[pos])) ++pos;
    string token = input.substr(start, pos - start);

    if (!token.empty() && token[0] == '$') token = token.substr(1);

    if (token.empty()) {
        cout << "Имя переменной не указано\n";
        return;
    }

    const char* val = getenv(token.c_str());
    if (val == nullptr) {
        cout << "Переменная окружения '" << token << "' не найдена\n";
        return;
    }

    string value = string(val);

    if (value.find(':') != string::npos) {
        stringstream ss(value);
        string part;
        size_t idx = 1;
        while (getline(ss, part, ':')) {
            if (part.empty()) {
                cout << idx << ": (пустой)\n";
            } else {
                cout << idx << ": " << part << '\n';
            }
            ++idx;
        }
    } else {
        cout << value << '\n';
    }
}

vector<string> split_args(const string& input) {
    stringstream sts(input);
    vector<string> args;
    string token;

    while (sts >> token) {
        args.push_back(token);
    }
    return args;
}

bool execute_external(const string& input) {
    vector<string> args = split_args(input);
    if (args.empty()) return false;

    vector<char*> c_args;
    for (string& s : args) c_args.push_back(&s[0]);
    c_args.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return true;
    }

    if (pid == 0) {
        execvp(c_args[0], c_args.data());
        perror("execvp");
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return true;
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

        if (input.empty()) continue;

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

        if (input.rfind("\\e", 0) == 0) {
            my_env(input);
            continue;
        }

        if (input.rfind("echo", 0) == 0) {
            my_echo(input);
            continue;
        }

        if (execute_external(input)) {
            continue;
        }

        cout << "Неизвестная команда: " << input << "\n";
    }

    save_history(history);
    return 0;
}
