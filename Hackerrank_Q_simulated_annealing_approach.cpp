#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

bool hasEdge(int u, int v, const std::vector<std::vector<int>>& adj) {
    return std::find(adj[u].begin(), adj[u].end(), v) != adj[u].end();
}

bool isValidCycle(const std::vector<int>& path, const std::vector<std::vector<int>>& adj) {
    int n = path.size();
    for (int i = 0; i < n - 1; ++i)
        if (!hasEdge(path[i], path[i + 1], adj))
            return false;
    return hasEdge(path[n - 1], path[0], adj);
}

int evaluate(const std::vector<int>& path, const std::vector<std::vector<int>>& adj) {
    int n = path.size();
    int score = 0;
    for (int i = 0; i < n - 1; ++i)
        score += hasEdge(path[i], path[i + 1], adj) ? 1 : 0;
    score += hasEdge(path[n - 1], path[0], adj) ? 1 : 0;  // return to start
    return score;
}

void simulatedAnnealing(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> current_path(n);
    for (int i = 0; i < n; ++i) current_path[i] = i;

    std::vector<int> best_path = current_path;
    int best_score = evaluate(current_path, adj);

    double temp = 1.0;
    double cooling_rate = 0.995;
    int iterations = 100000;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int iter = 0; iter < iterations; ++iter) {
        int a = 1 + rng() % (n - 1);
        int b = 1 + rng() % (n - 1);
        if (a == b) continue;
        std::swap(current_path[a], current_path[b]);

        int current_score = evaluate(current_path, adj);
        int delta = current_score - best_score;

        if (delta > 0 || dist(rng) < std::exp(delta / temp)) {
            if (current_score > best_score) {
                best_score = current_score;
                best_path = current_path;
            }
        } else {
            std::swap(current_path[a], current_path[b]); // revert
        }

        temp *= cooling_rate;
    }

    if (best_score == n && isValidCycle(best_path, adj)) {
        std::cout << "1\n";
        for (int city : best_path)
            std::cout << city << " ";
        std::cout << best_path[0] << "\n";
    } else {
        std::cout << "-1\n";
    }
}

int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> adj(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    simulatedAnnealing(n, adj);
    return 0;
}
