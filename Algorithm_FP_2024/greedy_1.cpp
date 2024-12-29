#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
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
    vector<int> serverAssignments;
    int numAgents;
    int numTeams;

    int bestCost;

    // Helper function to calculate cost for a team
    int calculateTeamCost(const vector<int>& team) {
        set<int> serversUsed;
        for (int agentId : team) {
            serversUsed.insert(serverAssignments[agentId]);
        }
        int numServers = serversUsed.size();
        return (numServers - 1) * (numServers - 1);
    }

    // Calculate total cost across all teams
    int calculateTotalCost() {
        int totalCost = 0;
        for (const auto& team : teams) {
            totalCost += calculateTeamCost(team);
        }
        return totalCost;
    }

    // Check if assignment is valid (respects memory constraints)
    bool isValidAssignment(vector<int>& serverLoads) {
        for (int load : serverLoads) {
            if (load > maxDiskCapacity) {
                return false;
            }
        }
        return true;
    }

    vector<int> bestAssignments2Way;
    void dfs2Way(int idx, vector<int>& serverLoads) {
        if (idx == numAgents) {
            if (isValidAssignment(serverLoads)) {
                int cost = calculateTotalCost();
                if (cost < bestCost) {
                    bestCost = cost;
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

    // Perform 2-way partitioning
    void twoWayPartition() {
        cout << "Performing 2-way partitioning" << endl;
        vector<int> serverLoads(2, 0);

        if (numAgents <= 30) {
            // Complete search
            cout << "Complete search" << endl;
            bestAssignments2Way.assign(numAgents, -1);
            bestCost = INT_MAX;

            dfs2Way(0, serverLoads);

            serverAssignments = bestAssignments2Way;
        } else {
            // Initial greedy assignment
            cout << "Greedy assignment" << endl;
            for (int i = 0; i < numAgents; i++) {
                if (serverLoads[0] <= serverLoads[1] &&
                    serverLoads[0] + agentMemory[i] <= maxDiskCapacity) {
                    serverAssignments[i] = 0;
                    serverLoads[0] += agentMemory[i];
                } else if (serverLoads[1] + agentMemory[i] <= maxDiskCapacity) {
                    serverAssignments[i] = 1;
                    serverLoads[1] += agentMemory[i];
                } else {
                    // If we can't assign to either server, try to swap with
                    // existing assignments
                    bool assigned = false;
                    for (int j = 0; j < i && !assigned; j++) {
                        int oldServer = serverAssignments[j];
                        int newLoad0 = serverLoads[0];
                        int newLoad1 = serverLoads[1];

                        if (oldServer == 0) {
                            newLoad0 -= agentMemory[j];
                            newLoad0 += agentMemory[i];
                            if (newLoad0 <= maxDiskCapacity) {
                                serverLoads[0] = newLoad0;
                                serverAssignments[i] = 0;
                                assigned = true;
                            }
                        } else {
                            newLoad1 -= agentMemory[j];
                            newLoad1 += agentMemory[i];
                            if (newLoad1 <= maxDiskCapacity) {
                                serverLoads[1] = newLoad1;
                                serverAssignments[i] = 1;
                                assigned = true;
                            }
                        }
                    }
                    if (!assigned) {
                        // If we still can't assign, try to split across both
                        // servers
                        serverAssignments[i] =
                            serverLoads[0] <= serverLoads[1] ? 0 : 1;
                        serverLoads[serverAssignments[i]] += agentMemory[i];
                    }
                }
            }

            // Local search optimization
            bool improved;
            bestCost = calculateTotalCost();
            do {
                improved = false;
                for (int i = 0; i < numAgents; i++) {
                    int orgCost = calculateTotalCost();
                    int originalServer = serverAssignments[i];
                    int targetServer = 1 - originalServer;

                    // Try swapping to other server
                    serverLoads[originalServer] -= agentMemory[i];
                    serverLoads[targetServer] += agentMemory[i];
                    serverAssignments[i] = targetServer;

                    int newCost = calculateTotalCost();
                    if (isValidAssignment(serverLoads) && newCost < orgCost) {
                        improved = true;
                        bestCost = newCost;
                    } else {
                        // Revert if no improvement
                        serverLoads[targetServer] -= agentMemory[i];
                        serverLoads[originalServer] += agentMemory[i];
                        serverAssignments[i] = originalServer;
                    }
                }
            } while (improved);
        }
    }

    // Perform k-way partitioning
    void kWayPartition() {
        cout << "Performing k-way partitioning" << endl;
        // Calculate minimum number of servers needed based on total memory
        long long totalMemory = 0;
        for (int mem : agentMemory) {
            totalMemory += mem;
        }
        int minServers = (totalMemory + maxDiskCapacity - 1) / maxDiskCapacity;

        // Initialize with greedy assignment to minServers
        vector<int> serverLoads(minServers, 0);
        for (int i = 0; i < numAgents; i++) {
            // Find server with minimum load that can accommodate this agent
            int bestServer = -1;
            int minLoad = maxDiskCapacity + 1;

            for (int j = 0; j < minServers; j++) {
                if (serverLoads[j] + agentMemory[i] <= maxDiskCapacity &&
                    serverLoads[j] < minLoad) {
                    minLoad = serverLoads[j];
                    bestServer = j;
                }
            }

            if (bestServer == -1) {
                // Need to add new server
                serverLoads.push_back(0);
                bestServer = serverLoads.size() - 1;
            }

            serverAssignments[i] = bestServer;
            serverLoads[bestServer] += agentMemory[i];
        }

        // Local search optimization
        bool improved;
        do {
            improved = false;
            bestCost = calculateTotalCost();
            for (int i = 0; i < numAgents; i++) {
                int originalServer = serverAssignments[i];
                int orgCost = calculateTotalCost();

                // Try all other servers
                for (int j = 0; j < int(serverLoads.size()); j++) {
                    if (j != originalServer) {
                        serverLoads[originalServer] -= agentMemory[i];
                        serverLoads[j] += agentMemory[i];
                        serverAssignments[i] = j;

                        int newCost = calculateTotalCost();
                        if (isValidAssignment(serverLoads) &&
                            newCost < orgCost) {
                            improved = true;
                            bestCost = newCost;
                            break;
                        } else {
                            // Revert if no improvement
                            serverLoads[j] -= agentMemory[i];
                            serverLoads[originalServer] += agentMemory[i];
                            serverAssignments[i] = originalServer;
                        }
                    }
                }
            }
        } while (improved);
    }

   public:
    bool readInput(const string& inputFile) {
        ifstream fin(inputFile);
        if (!fin) return false;

        // Read max disk capacity
        fin >> maxDiskCapacity;

        string marker;
        fin >> marker; // Read ".agent"

        // Read agent information
        fin >> numAgents;
        agentMemory.resize(numAgents);
        serverAssignments.resize(numAgents);
        for (int i = 0; i < numAgents; i++) {
            fin >> agentMemory[i];
        }

        fin >> marker; // Read ".team"

        // Read team information
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

        for (int i = 0; i < numAgents; i++) {
            cout << "Agent " << i << " Memory: " << agentMemory[i] << endl;
        }

        return true;
    }

    void optimize() {
        // Determine whether to use 2-way or k-way partitioning based on memory
        // constraints
        long long totalMemory = 0;
        for (int mem : agentMemory) {
            totalMemory += mem;
        }

        if (totalMemory <= 2 * maxDiskCapacity) {
            twoWayPartition();
        } else {
            kWayPartition();
        }
        cout << "Best cost = " << bestCost << endl;
    }

    bool writeOutput(const string& outputFile) {
        ofstream fout(outputFile);
        if (!fout) return false;

        // Calculate total cost
        int totalCost = calculateTotalCost();

        // Find number of servers used
        int numServers = 0;
        for (int server : serverAssignments) {
            numServers = max(numServers, server + 1);
        }

        // Write output
        fout << totalCost << endl;
        fout << numServers << endl;
        for (int assignment : serverAssignments) {
            fout << assignment << endl;
        }

        fout.close();

        for (int i = 0; i < numAgents; i++) {
            cout << "Agent " << i << " assigned to server "
                 << serverAssignments[i] << endl;
        }

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