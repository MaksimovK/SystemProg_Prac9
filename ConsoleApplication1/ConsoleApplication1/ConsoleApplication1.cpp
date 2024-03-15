#include <windows.h>
#include <iostream>
#include <locale>

using namespace std;

const int MAX_N = 100;
const int MAX_W = 10000;

int maxValue = 0;
CRITICAL_SECTION cs;

int knapSack(int W, int wt[], int val[], int n) {
    int** K = new int* [n + 1];
    for (int i = 0; i <= n; i++) {
        K[i] = new int[W + 1];
    }

    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (wt[i - 1] <= w)
                K[i][w] = max(val[i - 1] + K[i - 1][w - wt[i - 1]], K[i - 1][w]);
            else
                K[i][w] = K[i - 1][w];
        }
    }

    int result = K[n][W];

    for (int i = 0; i <= n; i++) {
        delete[] K[i];
    }
    delete[] K;

    return result;
}

DWORD WINAPI ThreadFunc(LPVOID data) {
    int* params = (int*)data;
    int result = knapSack(params[0], params + 4, params + 4 + params[3], params[3]);

    EnterCriticalSection(&cs);
    maxValue = max(maxValue, result);
    LeaveCriticalSection(&cs);

    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    int W = 50;
    int wt[] = { 10, 20, 30 };
    int val[] = { 60, 100, 120 };
    int n = sizeof(val) / sizeof(val[0]);

    InitializeCriticalSection(&cs);

    HANDLE threads[MAX_N];
    for (int i = 0; i < n; i++) {
        int* params = new int[4 + n * 2];
        params[0] = W;
        params[1] = (int)wt;
        params[2] = (int)val;
        params[3] = i + 1;
        memcpy(params + 4, wt, n * sizeof(int));
        memcpy(params + 4 + n, val, n * sizeof(int));
        threads[i] = CreateThread(NULL, 0, ThreadFunc, params, 0, NULL);
    }

    WaitForMultipleObjects(n, threads, TRUE, INFINITE);

    for (int i = 0; i < n; i++) {
        CloseHandle(threads[i]);
    }

    DeleteCriticalSection(&cs);

    cout << "Максимальная стоимость: " << maxValue << endl;

    return 0;
}
