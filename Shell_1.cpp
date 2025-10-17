#include <iostream>
#include <string>
using namespace std;

int main() {
    string input;
    
    cout << "Введите строку: ";
    while (getline(cin, input)) {
        cout << input << '\n';
        cout << "Введите строку: ";
    }
    
    cout << "\nВыход по нажатию Ctrl+D" << '\n';
    return 0;
}
