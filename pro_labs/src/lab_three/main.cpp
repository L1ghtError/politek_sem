#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#define N 4
using namespace std;

void displayArray(vector<vector<int>> x, int n);
vector<vector<int>> z;
vector<vector<int>> Mul(vector<vector<int>> x, vector<vector<int>> y, int n);
void MulRec(vector<vector<int>> x, vector<vector<int>> y, int n);
int local_counter = 0;
int rec_counter = 0;

int main() {
    srand(time(NULL));
    // cout << "Write N :";
    // int N;
    // cin >> N;
    vector<vector<int>> a(N, vector<int>(N));
    vector<vector<int>> b(N, vector<int>(N));
    z.resize(N);
    for (int i = 0; i < N; i++)
        z[i].resize(N);

    // Fill mat А
    int half = N / 2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == 0 || i == N - 1 || j == 0 || j == N - 1) {
                a[i][j] = 1;
            } else {
                a[i][j] = 0;
            }
        }
    }
    cout << "Matrix A:" << endl;
    displayArray(a, N);

    // fill mat B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if ((i + j) >= N - 1)
                b[i][j] = 1 + rand() % 9;
        }
    }

    cout << "Matrix B:" << endl;
    displayArray(b, N);
    cout << "Multiply 1:" << endl;
    displayArray(Mul(a, b, N), N);
    cout << "Count1: " << local_counter << endl;
    cout << endl;
    cout << "Multiply 2 " << endl;
    MulRec(a, b, N);
    displayArray(z, N);
    cout << "Count2: " << rec_counter << endl;
}

void displayArray(vector<vector<int>> x, int n) {
    cout << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << x[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

vector<vector<int>> Mul(vector<vector<int>> x, vector<vector<int>> y, int n) {
    vector<vector<int>> z(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            z[i][j] = 0;
            for (int k = 0; k < n; k++) {
                z[i][j] += x[i][k] * y[k][j];
                local_counter += 2;
            }
        }
    }
    return z;
}

void MulRec(vector<vector<int>> x, vector<vector<int>> y, int n) {
    static int i = 0, j = 0, k = 0;
    if (i < n) {
        if (j < n) {
            if (k < n) {
                z[i][j] += x[i][k] * y[k][j];
                if (x[i][k] != 0 && y[k][j] != 0) {
                    if (x[i][k] == 1 || y[k][j] == 1)
                        rec_counter += 1;
                    else
                        rec_counter += 2;
                }
                k++;
                MulRec(x, y, n);
            }
            k = 0;
            j++;
            MulRec(x, y, n);
        }
        j = 0;
        i++;
        MulRec(x, y, n);
    }
}
