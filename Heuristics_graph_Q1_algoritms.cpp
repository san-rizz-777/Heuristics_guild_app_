#include <bits/stdc++.h>
using namespace std;

using Edge = tuple<int, int, int>; // (u, v, weight)

class Graph {
    int V;
    vector<Edge> edges;
    vector<vector<pair<int, int>>> adj;

public:
    Graph(int V, vector<Edge> edgeList) : V(V), edges(edgeList) {
        adj.resize(V);
        for (auto [u, v, w] : edges)
            adj[u].emplace_back(v, w);
    }

    // 1. Cycle Detection in Directed Graph using DFS
    bool hasCycleUtil(int u, vector<bool>& visited, vector<bool>& recStack) {
        visited[u] = recStack[u] = true;
        for (auto [v, _] : adj[u]) {
            if (!visited[v] && hasCycleUtil(v, visited, recStack)) return true;
            if (recStack[v]) return true;
        }
        recStack[u] = false;
        return false;
    }

    bool hasCycle() {
        vector<bool> visited(V, false), recStack(V, false);
        for (int i = 0; i < V; i++)
            if (!visited[i] && hasCycleUtil(i, visited, recStack))
                return true;
        return false;
    }

    // 2. Dijkstra's Algorithm for Shortest Path
    int dijkstra(int src, int dest) {
        vector<int> dist(V, INT_MAX);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

        dist[src] = 0;
        pq.emplace(0, src);

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (u == dest) return d;
            for (auto [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.emplace(dist[v], v);
                }
            }
        }
        return -1; // no path
    }

    // 3. Kruskal’s Algorithm for MST
    int find(int u, vector<int>& parent) {
        if (parent[u] != u)
            parent[u] = find(parent[u], parent);
        return parent[u];
    }

    bool unite(int u, int v, vector<int>& parent, vector<int>& rank) {
        u = find(u, parent);
        v = find(v, parent);
        if (u == v) return false;
        if (rank[u] < rank[v]) parent[u] = v;
        else if (rank[u] > rank[v]) parent[v] = u;
        else {
            parent[v] = u;
            rank[u]++;
        }
        return true;
    }

    int kruskalMST() {
        vector<Edge> allEdges = edges;
        sort(allEdges.begin(), allEdges.end(), [](auto& a, auto& b) {
            return get<2>(a) < get<2>(b);
        });

        vector<int> parent(V), rank(V, 0);
        iota(parent.begin(), parent.end(), 0);

        int mst_weight = 0, edges_used = 0;
        for (auto [u, v, w] : allEdges) {
            if (unite(u, v, parent, rank)) {
                mst_weight += w;
                edges_used++;
                if (edges_used == V - 1) break;
            }
        }
        return edges_used == V - 1 ? mst_weight : -1;
    }

    // 4. Kosaraju's Algorithm for SCC + DAG + TopoSort
    void dfs1(int u, vector<bool>& visited, stack<int>& order) {
        visited[u] = true;
        for (auto [v, _] : adj[u])
            if (!visited[v]) dfs1(v, visited, order);
        order.push(u);
    }

    void dfs2(int u, vector<vector<pair<int, int>>>& radj, vector<bool>& visited, vector<int>& component) {
        visited[u] = true;
        component.push_back(u);
        for (auto [v, _] : radj[u])
            if (!visited[v]) dfs2(v, radj, visited, component);
    }

    pair<vector<vector<int>>, vector<int>> getSCCandTopo() {
        vector<bool> visited(V, false);
        stack<int> order;

        for (int i = 0; i < V; ++i)
            if (!visited[i]) dfs1(i, visited, order);

        vector<vector<pair<int, int>>> radj(V);
        for (auto [u, v, w] : edges)
            radj[v].emplace_back(u, w);

        fill(visited.begin(), visited.end(), false);
        vector<vector<int>> sccs;
        while (!order.empty()) {
            int u = order.top(); order.pop();
            if (!visited[u]) {
                vector<int> component;
                dfs2(u, radj, visited, component);
                sccs.push_back(component);
            }
        }

        map<int, int> nodeToSCC;
        for (int i = 0; i < sccs.size(); ++i)
            for (int node : sccs[i])
                nodeToSCC[node] = i;

        vector<set<int>> dag(sccs.size());
        for (auto [u, v, w] : edges) {
            int cu = nodeToSCC[u], cv = nodeToSCC[v];
            if (cu != cv)
                dag[cu].insert(cv);
        }

        vector<int> indegree(sccs.size(), 0);
        for (auto& adjSet : dag)
            for (int v : adjSet) indegree[v]++;

        queue<int> q;
        for (int i = 0; i < sccs.size(); i++)
            if (indegree[i] == 0) q.push(i);

        vector<int> topoSort;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            topoSort.push_back(u);
            for (int v : dag[u])
                if (--indegree[v] == 0) q.push(v);
        }

        return {sccs, topoSort};
    }
};

int main() {
    int V, E;
    cout << "Enter number of vertices and edges: ";
    cin >> V >> E;
    vector<Edge> edgeList;
    cout << "Enter edges as: u v weight (0-indexed):\n";
    for (int i = 0; i < E; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        edgeList.emplace_back(u, v, w);
    }

    Graph g(V, edgeList);

    cout << "\nCycle Detected? " << (g.hasCycle() ? "Yes" : "No") << "\n";

    int src, dest;
    cout << "Enter source and destination for shortest path: ";
    cin >> src >> dest;
    int sp = g.dijkstra(src, dest);
    if (sp != -1)
        cout << "Shortest Path " << src << " -> " << dest << ": " << sp << "\n";
    else
        cout << "No path exists from " << src << " to " << dest << "\n";

    cout << "MST Weight (undirected assumption): " << g.kruskalMST() << "\n";

    auto [sccs, topo] = g.getSCCandTopo();
    cout << "\nStrongly Connected Components:\n";
    for (int i = 0; i < sccs.size(); ++i) {
        cout << "SCC " << i << ": ";
        for (int node : sccs[i]) cout << node << " ";
        cout << "\n";
    }

    cout << "\nTopological Order of SCC-DAG: ";
    for (int x : topo) cout << x << " ";
    cout << "\n";
    return 0;
}
