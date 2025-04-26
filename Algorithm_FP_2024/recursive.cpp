#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class DeploymentOptimizer {
   private:
    int maxDiskCapacity;
    vector<int> agentMemory;
    vector<vector<int>> teams;
    vector<int> serverAssignments; // serverAssignments[i] = 該 agent
                                   // 被分配到第幾號 server
    int numAgents;
    int numTeams;

    int calculateTeamCost(const vector<int>& team) {
        set<int> serversUsed;
        for (int agentId : team) {
            serversUsed.insert(serverAssignments[agentId]);
        }
        int numServers = serversUsed.size();

        return (numServers - 1) * (numServers - 1);
    }

    int calculateTotalCost() {
        int totalCost = 0;
        for (const auto& team : teams) {
            totalCost += calculateTeamCost(team);
        }
        return totalCost;
    }

    // 檢查是否皆符合Capacity限制
    bool isValidAssignment(const vector<long long>& serverLoads) {
        for (auto load : serverLoads) {
            if (load > maxDiskCapacity) {
                return false;
            }
        }
        return true;
    }

    // bestAssignments 用來紀錄目前找到的最佳分配方式
    // bestCost        紀錄最佳分配方式所得到的總成本
    vector<int> bestAssignments2Way;
    int bestCost2Way = INT_MAX;

    // 2-way DFS：把第 idx 號 agent 放到 server 0 或 server 1
    // serverLoads[0], serverLoads[1] 分別代表目前兩個伺服器使用的記憶體量
    void dfs2Way(int idx, vector<long long>& serverLoads) {
        if (idx == numAgents) {
            if (isValidAssignment(serverLoads)) {
                int cost = calculateTotalCost();
                if (cost < bestCost2Way) {
                    bestCost2Way = cost;
                    bestAssignments2Way = serverAssignments;
                }
            }
            return;
        }

        // 嘗試放到 server 0
        serverAssignments[idx] = 0;
        serverLoads[0] += agentMemory[idx];
        if (serverLoads[0] <= maxDiskCapacity) {
            dfs2Way(idx + 1, serverLoads);
        }
        // 還原
        serverLoads[0] -= agentMemory[idx];

        // 嘗試放到 server 1
        serverAssignments[idx] = 1;
        serverLoads[1] += agentMemory[idx];
        if (serverLoads[1] <= maxDiskCapacity) {
            dfs2Way(idx + 1, serverLoads);
        }
        // 還原
        serverLoads[1] -= agentMemory[idx];
    }

    void twoWayPartition() {
        cout << "Performing 2-way partitioning (Complete Search)" << endl;

        bestAssignments2Way.assign(numAgents, -1);
        bestCost2Way = INT_MAX;
        vector<long long> serverLoads(2, 0);

        dfs2Way(0, serverLoads);

        serverAssignments = bestAssignments2Way;
        cout << "Best cost (2-way) = " << bestCost2Way << endl;
    }

    vector<int> bestAssignmentsKWay;
    // 允許最多 numAgents 個伺服器（每個 agent 各放一台），
    int bestCostKWay = INT_MAX;

    // k-way DFS：idx 代表目前要分配的 agent 編號
    // maxServers 最多允許多少台 server
    // serverLoads[j] 代表目前第 j 台伺服器的使用量
    void dfsKWay(int idx, vector<long long>& serverLoads, int maxServers) {
        if (idx == numAgents) {
            if (isValidAssignment(serverLoads)) {
                int cost = calculateTotalCost();
                if (cost < bestCostKWay) {
                    bestCostKWay = cost;
                    bestAssignmentsKWay = serverAssignments;
                }
            }
            return;
        }

        for (int s = 0; s < maxServers; s++) {
            // 將 agent idx 分到 server s
            serverAssignments[idx] = s;
            serverLoads[s] += agentMemory[idx];

            // 如果容量沒超過，就繼續遞迴
            if (serverLoads[s] <= maxDiskCapacity) {
                dfsKWay(idx + 1, serverLoads, maxServers);
            }

            // 還原
            serverLoads[s] -= agentMemory[idx];
        }
    }

    void kWayPartition() {
        cout << "Performing k-way partitioning (Complete Search)" << endl;

        long long totalMemory = 0;
        for (int mem : agentMemory) {
            totalMemory += mem;
        }
        // 最少需要的server數量，maxDiskCapacity - 1 是為了無條件進位
        int minServers = (totalMemory + maxDiskCapacity - 1) / maxDiskCapacity;
        // 最多 numAgents 台 (每個 agent 都擁有自己的server)
        int maxServers = numAgents;
        // cout << maxServers << " servers will be used at most" << endl;

        bestAssignmentsKWay.assign(numAgents, -1);
        bestCostKWay = INT_MAX;

        for (int k = minServers; k <= maxServers; k++) {
            vector<long long> serverLoads(k, 0);
            dfsKWay(0, serverLoads, k);
        }

        serverAssignments = bestAssignmentsKWay;
        cout << "Best cost (k-way) = " << bestCostKWay << endl;
    }

   public:
    bool readInput(const string& inputFile) {
        ifstream fin(inputFile);
        if (!fin) return false;

        fin >> maxDiskCapacity;

        string marker;
        fin >> marker; // .agent

        fin >> numAgents;
        agentMemory.resize(numAgents);
        serverAssignments.resize(numAgents);
        for (int i = 0; i < numAgents; i++) {
            fin >> agentMemory[i];
        }

        fin >> marker; // .team

        fin >> numTeams;
        teams.resize(numTeams);
        for (int i = 0; i < numTeams; i++) {
            int teamSize;
            fin >> teamSize;
            teams[i].resize(teamSize);
            for (int j = 0; j < teamSize; j++) {
                fin >> teams[i][j];
            }
        }

        fin.close();
        return true;
    }

    void optimize() {
        long long totalMemory = 0;
        for (int mem : agentMemory) {
            totalMemory += mem;
        }

        if (totalMemory <= 2 * maxDiskCapacity) {
            twoWayPartition();
        } else {
            kWayPartition();
        }
    }

    bool writeOutput(const string& outputFile) {
        ofstream fout(outputFile);
        if (!fout) return false;

        int totalCost = calculateTotalCost();

        int numServers = 0;
        for (int server : serverAssignments) {
            numServers = max(numServers, server + 1);
        }

        fout << totalCost << endl;
        fout << numServers << endl;
        for (int assignment : serverAssignments) {
            fout << assignment << endl;
        }

        fout.close();
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    DeploymentOptimizer optimizer;

    if (!optimizer.readInput(argv[1])) {
        cerr << "Error reading input file" << endl;
        return 1;
    }

    auto start_time = chrono::high_resolution_clock::now();
    optimizer.optimize();
    auto end_time = chrono::high_resolution_clock::now();
    auto duration =
        chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "Execution Time: " << duration.count() << " ms\n";

    if (!optimizer.writeOutput(argv[2])) {
        cerr << "Error writing output file" << endl;
        return 1;
    }

    return 0;
}
