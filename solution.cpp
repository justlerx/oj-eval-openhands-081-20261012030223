#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

// Use Dinic's algorithm but optimized for unit capacities
class OptimizedMaxFlow {
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
    OptimizedMaxFlow(int size) : n(size) {
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
                // Early termination: max flow can't exceed 3 due to degree constraint
                if (flow >= 3) return flow;
            }
        }
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    if (m == 0) {
        cout << 0 << endl;
        return 0;
    }
    
    // Build adjacency list for connected components
    vector<vector<int>> adj(n);
    vector<pair<int, int>> edges;
    
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        a--; b--;  // Convert to 0-based
        edges.emplace_back(a, b);
        adj[a].push_back(b);
        adj[b].push_back(a);
    }
    
    long long total_flow = 0;
    
    // Precompute connected components
    vector<int> component(n, -1);
    int comp_count = 0;
    
    for (int i = 0; i < n; i++) {
        if (component[i] == -1) {
            queue<int> q;
            q.push(i);
            component[i] = comp_count;
            
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                for (int u : adj[v]) {
                    if (component[u] == -1) {
                        component[u] = comp_count;
                        q.push(u);
                    }
                }
            }
            comp_count++;
        }
    }
    
    // Group nodes by component
    vector<vector<int>> comp_nodes(comp_count);
    for (int i = 0; i < n; i++) {
        comp_nodes[component[i]].push_back(i);
    }
    
    // Compute max flow only for pairs in the same component
    for (int c = 0; c < comp_count; c++) {
        const auto& nodes = comp_nodes[c];
        if (nodes.size() <= 1) continue;
        
        for (size_t i = 0; i < nodes.size(); i++) {
            for (size_t j = i + 1; j < nodes.size(); j++) {
                int s = nodes[i], t = nodes[j];
                
                OptimizedMaxFlow solver(n);
                
                // Build the flow network for this pair
                for (const auto& edge : edges) {
                    solver.add_edge(edge.first, edge.second, 1);
                    solver.add_edge(edge.second, edge.first, 1);
                }
                
                int flow = solver.max_flow(s, t);
                total_flow += flow;
            }
        }
    }
    
    cout << total_flow << endl;
    
    return 0;
}