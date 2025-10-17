#include <iostream>
#include <string>
using namespace std;

int main() {
    string input;
    
    cout << "Введите строку: ";
    while (getline(cin, input)) {
        if (input == "\\q") {
            cout << "Выход по команде '\\q'" << endl;
            break;
        }
        
        cout << input << endl;
        cout << "Введите строку: ";
    }

    if (cin.eof()) {
        cout << "\nВыход по команде Ctrl+D" << endl;
    }
    
    return 0;
}
