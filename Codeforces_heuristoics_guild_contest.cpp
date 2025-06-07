#include <bits/stdc++.h>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

struct Bastion {
    int index;
    double x, y, v;
};

double speed, T_realise, T_empty;
double total_gold = 0;

double travel_time(double x1, double y1, double x2, double y2) {
    return hypot(x1 - x2, y1 - y2) / speed;
}

double compute_score(const vector<Bastion> &bastions, const vector<int> &path) {
    double time = 0, cx = 0, cy = 0, collected = 0;

    for (int i : path) {
        const auto &b = bastions[i];
        double dt = travel_time(cx, cy, b.x, b.y);
        time += dt;
        cx = b.x;
        cy = b.y;

        if (time >= T_empty) break;
        double val = b.v;
        if (time > T_realise)
            val *= (1.0 - (time - T_realise) / (T_empty - T_realise));
        if (val > 0)
            collected += val;
    }
    return collected;
}

vector<int> greedy_initial(const vector<Bastion> &bastions) {
    int n = bastions.size();
    vector<bool> visited(n, false);
    vector<int> path;
    double time = 0, cx = 0, cy = 0;

    while (true) {
        int best = -1;
        double bestScore = -1;
        for (int i = 0; i < n; ++i) {
            if (visited[i]) continue;
            double dt = travel_time(cx, cy, bastions[i].x, bastions[i].y);
            double arrival = time + dt;
            if (arrival >= T_empty) continue;

            double val = bastions[i].v;
            if (arrival > T_realise)
                val *= (1.0 - (arrival - T_realise) / (T_empty - T_realise));
            if (val > 0) {
                double score = val / dt;
                if (score > bestScore) {
                    bestScore = score;
                    best = i;
                }
            }
        }
        if (best == -1) break;
        visited[best] = true;
        path.push_back(best);
        double dt = travel_time(cx, cy, bastions[best].x, bastions[best].y);
        time += dt;
        cx = bastions[best].x;
        cy = bastions[best].y;
    }

    return path;
}

vector<int> simulated_annealing(const vector<Bastion> &bastions, vector<int> initial, double duration = 1.8) {
    auto current = initial;
    auto best = current;
    double current_score = compute_score(bastions, current);
    double best_score = current_score;

    auto start_time = chrono::steady_clock::now();
    double T = 1.0, cooling = 0.995;

    while (chrono::duration<double>(chrono::steady_clock::now() - start_time).count() < duration) {
        vector<int> neighbor = current;

        int op = rng() % 3;
        if (neighbor.size() >= 2) {
            if (op == 0) { // swap
                int i = rng() % neighbor.size(), j = rng() % neighbor.size();
                if (i != j) swap(neighbor[i], neighbor[j]);
            } else if (op == 1) { // reverse
                int i = rng() % neighbor.size(), j = rng() % neighbor.size();
                if (i > j) swap(i, j);
                reverse(neighbor.begin() + i, neighbor.begin() + j + 1);
            } else { // insert
                int i = rng() % neighbor.size();
                int j = rng() % neighbor.size();
                if (i != j) {
                    int val = neighbor[i];
                    neighbor.erase(neighbor.begin() + i);
                    neighbor.insert(neighbor.begin() + j, val);
                }
            }
        }

        double score = compute_score(bastions, neighbor);
        double delta = score - current_score;

        if (delta > 0 || exp(delta / T) > (rng() % 10000) / 10000.0) {
            current = neighbor;
            current_score = score;
            if (score > best_score) {
                best_score = score;
                best = neighbor;
            }
        }
        T *= cooling;
    }

    return best;
}

int main() {
    int n;
    cin >> n;
    vector<Bastion> bastions(n);
    for (int i = 0; i < n; ++i) {
        cin >> bastions[i].x >> bastions[i].y >> bastions[i].v;
        bastions[i].index = i;
        total_gold += bastions[i].v;
    }

    cin >> speed >> T_realise >> T_empty;

    vector<int> init_path = greedy_initial(bastions);
    vector<int> best_path = simulated_annealing(bastions, init_path);

    // Output result
    cout << best_path.size() << "\n";
    for (int i : best_path) cout << bastions[i].index << " ";
    cout << endl;

    return 0;
}
