#include <iostream>
#include <vector>

bool isHamiltonian(std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::vector<int>& path, int n) {
    if ((int)path.size() == n) {
        // Check if the last node connects back to the first to form a cycle
        int last = path.back();
        int first = path.front();
        for (int neighbor : adj[last]) {
            if (neighbor == first) {
                path.push_back(first);  // Close the cycle
                return true;
            }
        }
        return false;
    }

    int current = path.back();
    for (int neighbor : adj[current]) {
        if (!visited[neighbor]) {
            visited[neighbor] = true;
            path.push_back(neighbor);

            if (isHamiltonian(adj, visited, path, n)) return true;

            // Backtrack
            visited[neighbor] = false;
            path.pop_back();
        }
    }

    return false;
}

int main() {
    int n, m;
    std::cin >> n >> m;

    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<bool> visited(n, false);
    std::vector<int> path;

    visited[0] = true;
    path.push_back(0);

    if (isHamiltonian(adj, visited, path, n)) {
        std::cout<<1<<"\n";
        for (int node : path)
            std::cout << node << " ";
        std::cout << "\n";
    } else {
        std::cout << -1 << "\n";
    }

    return 0;
}