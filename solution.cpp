#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

class MaxFlowSolver {
private:
    struct Edge {
        int to, cap, rev;
        Edge(int t, int c, int r) : to(t), cap(c), rev(r) {}
    };
    
    vector<vector<Edge>> graph;
    vector<int> level, iter;
    int n;
    
    void bfs(int s) {
        level.assign(n, -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (const Edge& e : graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }
    
    int dfs(int v, int t, int f) {
        if (v == t) return f;
        for (int& i = iter[v]; i < static_cast<int>(graph[v].size()); i++) {
            Edge& e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    
public:
    MaxFlowSolver(int size) : n(size) {
        graph.resize(n);
    }
    
    void add_edge(int from, int to, int cap) {
        graph[from].emplace_back(to, cap, graph[to].size());
        graph[to].emplace_back(from, 0, graph[from].size() - 1);
    }
    
    int max_flow(int s, int t) {
        int flow = 0;
        while (true) {
            bfs(s);
            if (level[t] < 0) return flow;
            iter.assign(n, 0);
            while (true) {
                int f = dfs(s, t, 1);
                if (f == 0) break;
                flow += f;
            }
        }
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    // Store original edges
    vector<pair<int, int>> edges;
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        a--; b--;  // Convert to 0-based
        edges.emplace_back(a, b);
    }
    
    long long total_flow = 0;
    
    // Compute max flow for each pair (a, b) where a < b
    for (int s = 0; s < n; s++) {
        for (int t = s + 1; t < n; t++) {
            MaxFlowSolver solver(n);
            
            // Build the flow network for this pair
            for (const auto& edge : edges) {
                solver.add_edge(edge.first, edge.second, 1);
                solver.add_edge(edge.second, edge.first, 1);  // Undirected edge
            }
            
            int flow = solver.max_flow(s, t);
            total_flow += flow;
        }
    }
    
    cout << total_flow << endl;
    
    return 0;
}