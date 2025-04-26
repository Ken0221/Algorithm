#include <climits>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    vector<int> dp(n + 3, -1);
    // n+3 is the number of element in the vector, not the index
    for (int i = 0; i < 2; i++) cost.push_back(0);

    // initial condition
    dp[0] = 0;
    dp[1] = 0;

    // solve the best solution for each step, from dp[2] to dp[n]
    for (int i = 2; i < n + 3; i++) {
        if (dp[i] == -1) {
            int dp_plus_cost_3 = (i < 3) ? INT_MAX : dp[i - 3] + cost[i - 3];
            dp[i] = min(dp[i - 1] + cost[i - 1], dp_plus_cost_3);
        }
    }

    // for (int i = 0; i < n + 3; i++) {
    //     cout << "dp[" << i << "]: " << dp[i] << "  cost[" << i
    //          << "]: " << cost[i] << endl;
    // }

    return min(min(dp[n], dp[n + 1]), dp[n + 2]);
}

int main() {
    vector<int> cost;
    string str;
    getline(cin, str);
    int start = 0;
    if (str[0] == ' ') {
        str = str.substr(1, str.length() - 1);
    }
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ' ' || i == str.length() - 1) {
            cost.push_back(stoi(str.substr(start, i)));
            start = i + 1;
        }
    }

    cout << minCostClimbingStairs(cost) << endl;

    return 0;
}