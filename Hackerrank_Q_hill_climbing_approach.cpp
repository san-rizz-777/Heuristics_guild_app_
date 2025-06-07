#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// Check if edge exists in adjacency list
bool hasEdge(int u, int v, const std::vector<std::vector<int>>& adj) {
    return std::find(adj[u].begin(), adj[u].end(), v) != adj[u].end();
}

int evaluate(const std::vector<int>& path, const std::vector<std::vector<int>>& adj) {
    int valid_edges = 0;
    int n = path.size();
    for (int i = 0; i < n - 1; ++i)
        if (hasEdge(path[i], path[i + 1], adj))
            valid_edges++;

    if (hasEdge(path[n - 1], path[0], adj))  // close the cycle
        valid_edges++;

    return valid_edges;
}

int main() {
    int n, m;
    std::cin >> n >> m;

    std::vector<std::vector<int>> adj(n);  // Adjacency list

    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<int> path(n);
    for (int i = 0; i < n; ++i)
        path[i] = i;

    std::shuffle(path.begin(), path.end(), rng);
    int current_score = evaluate(path, adj);
    const int max_iterations = 500000;

    for (int iter = 0; iter < max_iterations; ++iter) {
        std::vector<int> new_path = path;

        // Perturb: swap two cities
        int i = rng() % n;
        int j = rng() % n;
        if (i != j) std::swap(new_path[i], new_path[j]);

        int new_score = evaluate(new_path, adj);

        if (new_score >= current_score) {
            path = new_path;
            current_score = new_score;

            // Early exit if a valid Hamiltonian cycle is found
            if (current_score == n)
                break;
        }
    }

    if (current_score == n) {
        std::cout << 1 << "\n";
        for (int city : path)
            std::cout << city << " ";
        std::cout << path[0] << "\n";  // Complete the cycle
    } else {
        std::cout << -1 << "\n";
    }

    return 0;
}
