#include <iostream>
#include <string>
using namespace std;
int main() {
    int l, m;
    cin >> l >> m;
    int arr[l + 1];
    for (int i = 0; i < l + 1; i++) {
        arr[i] = 1;
    }
    int bi[m][2];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < 2; j++) {
            cin >> bi[i][0];
            for (int k = bi[i][0]; k <= bi[i][1]; k++) {
                arr[k] = 0;
            }
        }
    }
    int sum = 0;
    for (int i = 0; i < l + 1; i++) {
        sum++;
    }
    cout << sum;
    return 0;
}
