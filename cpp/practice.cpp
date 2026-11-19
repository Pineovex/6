#include <iostream>
#include <string>
using namespace std;
struct student {
    string name;
    string id;
    int score;
};

int main() {
    int n = 0;
    cin >> n;
    student arr[n];
    for (int i = 0; i < n; i++) {
        cin >> arr[i].name >> arr[i].id >> arr[i].score;
    }
    int max_score = arr[0].score;
    int min_score = arr[0].score;
    for (int i = 0; i < n; i++) {
        if (arr[i].score > max_score) {
            max_score = arr[i].score;
        }
        if (arr[i].score < min_score) {
            min_score = arr[i].score;
        }
    }
    for (int i = 0; i < n; i++) {
        if (arr[i].score == max_score) {
            cout << arr[i].name << " " << arr[i].id << endl;
        }
        if (arr[i].score == min_score) {
            cout << arr[i].name << " " << arr[i].id << endl;
        }
    }
    return 0;
}
