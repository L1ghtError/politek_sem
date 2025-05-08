#include <iostream>
#include <string>
#include <locale>
#include <windows.h>

using namespace std;

string encrypt(const string& plaintext, int shift) {
    string ciphertext = "";
    int n = plaintext.length();
    // ������ ������ ��������� ����
    string alphabet_upper = "���å�Ū��Ȳ���������������������";
    string alphabet_lower = " ��������賿��������������������";
    int alphabet_size = alphabet_upper.length();

    for (int i = 0; i < n; i++) {
        char c = plaintext[i];
        // ��������� ������� ������� � ���������� ������
        size_t index = alphabet_upper.find(c);
        if (index != string::npos) { // ���� ������ �������� � ��������� ������
            c = alphabet_upper[(index + shift) % alphabet_size];
        }
        else { // ���� ������ �������� � �������� ������
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
    // ������ ������ ��������� ����
    string alphabet_upper = "���å�Ū��Ȳ���������������������";
    string alphabet_lower = " ��������賿��������������������";
    int alphabet_size = alphabet_upper.length();

    for (int i = 0; i < n; i++) {
        char c = ciphertext[i];
        // ��������� ������� ������� � ���������� ������
        size_t index = alphabet_upper.find(c);
        if (index != string::npos) { // ���� ������ �������� � ��������� ������
            c = alphabet_upper[(index - shift + alphabet_size) % alphabet_size];
        }
        else { // ���� ������ �������� � �������� ������
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
    cout << "������ ����� ��� ����������: ";
    getline(cin, plaintext);
    cout << "������ ����: ";
    cin >> shift;
    cin.ignore(); // ������� ������ �����
    shift %= 32; // �������� ����� �� 33 �������
    ciphertext = encrypt(plaintext, shift);
    cout << "������������ �����: " << ciphertext << endl;
    plaintext = decrypt(ciphertext, shift);
    cout << "������������� �����: " << plaintext << endl;
    return 0;
}