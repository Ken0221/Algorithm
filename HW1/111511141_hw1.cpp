#include <iostream>
#include <vector>

using namespace std;

void sort(vector<int> &arr) {
    int len = arr.size();
    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len; j++) {
            if (arr[i] > arr[j]) swap(arr[i], arr[j]);
        }
    }
}

int main() {
    int power;
    int score = 0;
    vector<int> token;

    cin >> power;
    int token_in;
    while (cin >> token_in && token_in != -1) {
        token.push_back(token_in);
    }

    sort(token);

    int max_score = 0;
    int left = 0, right = token.size() - 1;
    while (left <= right) {
        // cout << "left: " << left << " right: " << right << endl;
        if (power >= token[left]) { // if power can be used to gain score
            power -= token[left];
            score++;
            if (score > max_score) max_score = score;
            left++;
        } else if (score > 0) { // otherwise, use score to gain power
            power += token[right];
            score--;
            right--;
        } else { // if nothing can do, break
            break;
        }
    }

    cout << max_score << endl;

    return 0;
}