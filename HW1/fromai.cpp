#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// Custom sorting function
void customSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int bagOfTokensScore(vector<int>& tokens, int power) {
    customSort(tokens);
    int left = 0, right = tokens.size() - 1;
    int score = 0, maxScore = 0;

    while (left <= right && (power >= tokens[left] || score > 0)) {
        while (left <= right && power >= tokens[left]) {
            power -= tokens[left];
            score++;
            left++;
        }

        maxScore = max(maxScore, score);

        if (left <= right && score > 0) {
            power += tokens[right];
            score--;
            right--;
        }
    }

    return maxScore;
}

int main() {
    int power;
    cout << "power: ";
    cin >> power;

    vector<int> tokens;
    int token;
    cout << "tokens: ";
    while (cin >> token && token != -1) {
        tokens.push_back(token);
    }

    int result = bagOfTokensScore(tokens, power);
    cout << result << endl;

    return 0;
}