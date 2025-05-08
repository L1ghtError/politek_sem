#include <iostream>
#include <string>
#include <locale>
#include <windows.h>

using namespace std;

string encrypt(const string& plaintext, int shift) {
    string ciphertext = "";
    int n = plaintext.length();
    // Повний алфавіт української мови
    string alphabet_upper = "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯ";
    string alphabet_lower = " абвгґдеєжзиіїйклмнопрстуфхцчшщьюя";
    int alphabet_size = alphabet_upper.length();

    for (int i = 0; i < n; i++) {
        char c = plaintext[i];
        // Знаходимо позицію символу у відповідному алфавіті
        size_t index = alphabet_upper.find(c);
        if (index != string::npos) { // Якщо символ знайдено в верхньому регістрі
            c = alphabet_upper[(index + shift) % alphabet_size];
        }
        else { // Якщо символ знайдено в нижньому регістрі
            index = alphabet_lower.find(c);
            if (index != string::npos) {
                c = alphabet_lower[(index + shift) % alphabet_size];
            }
        }
        ciphertext += c;
    }
    return ciphertext;
}
string decrypt(const string& ciphertext, int shift) {
    string plaintext = "";
    int n = ciphertext.length();
    // Повний алфавіт української мови
    string alphabet_upper = "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯ";
    string alphabet_lower = " абвгґдеєжзиіїйклмнопрстуфхцчшщьюя";
    int alphabet_size = alphabet_upper.length();

    for (int i = 0; i < n; i++) {
        char c = ciphertext[i];
        // Знаходимо позицію символу у відповідному алфавіті
        size_t index = alphabet_upper.find(c);
        if (index != string::npos) { // Якщо символ знайдено в верхньому регістрі
            c = alphabet_upper[(index - shift + alphabet_size) % alphabet_size];
        }
        else { // Якщо символ знайдено в нижньому регістрі
            index = alphabet_lower.find(c);
            if (index != string::npos) {
                c = alphabet_lower[(index - shift + alphabet_size) % alphabet_size];
            }
        }
        plaintext += c;
    }
    return plaintext;
}

int main() {
    setlocale(LC_CTYPE, "ukr");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string plaintext, ciphertext;
    int shift;
    cout << "Введіть текст для шифрування: ";
    getline(cin, plaintext);
    cout << "Введіть зсув: ";
    cin >> shift;
    cin.ignore(); // очистка буфера вводу
    shift %= 32; // обрізання зсуву до 33 символів
    ciphertext = encrypt(plaintext, shift);
    cout << "Зашифрований текст: " << ciphertext << endl;
    plaintext = decrypt(ciphertext, shift);
    cout << "Розшифрований текст: " << plaintext << endl;
    return 0;
}