#include "ch_graph.hpp"

#include <queue>
#include <limits>

double CHGraph::importance(
    int v,
    const std::vector<std::vector<std::pair<int,double>>>& adj,
    const std::vector<int>& contracted
) {
    int deg = 0;
    for (auto& [u, _] : adj[v]) {
        if (!contracted[u]) deg++;
    }
    return deg;
}

bool CHGraph::witness_search(
    const std::vector<std::vector<std::pair<int,double>>>& adj,
    int source,
    int target,
    int forbidden,
    double max_dist,
    const std::vector<int>& contracted
) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = adj.size();

    std::vector<double> dist(n, INF);

    typedef std::pair<double, int> QItem;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    dist[source] = 0.0;
    pq.emplace(0.0, source);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // bound reached, so no need to continue
        if (d > max_dist) return false;

        // target found
        if (u == target) return true;

        if (d != dist[u]) continue;

        for (auto& [v, w] : adj[u]) {
            // forbid node v (the contracted node)
            if (v == forbidden) continue;

            // skip already contracted nodes
            if (contracted[v]) continue;

            double nd = d + w;

            if (nd < dist[v] && nd <= max_dist) {
                dist[v] = nd;
                pq.emplace(nd, v);
            }
        }
    }

    return false; // no path found <= max_dist
}

void CHGraph::preproc_graph_bottom_up(
    const CHGraph::Graph &graph,
    CHGraph::PreprocGraph &preproc_graph
) {
    const int n = graph.num_nodes;

    int current_rank = 0;

    std::vector<int> contracted(n, 0); // contracted[i] takes value 0 for not contracted, 1 otherwise
    std::vector<int> rank(n, -1);

    std::vector<CHArc> all_arcs;

   
    // adj list that contains original edges + shortcuts added so far
    std::vector<std::vector<std::pair<int,double>>> adj(n);
    for (int u = 0; u < n; u++) {
        for (int ei = graph.first_out[u]; ei < graph.first_out[u + 1]; ei++) {
            int v = graph.to[ei];
            double w = graph.weights[ei];
            adj[u].push_back({v, w});
        }
    }

    typedef std::pair<double, int> QItem; // (importance, node)
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    // initial importance
    for (int v = 0; v < n; v++) {
        pq.emplace(importance(v, adj, contracted), v);
    }

    while (!pq.empty()) {
        // select next least important uncontracted node
        auto [imp, v] = pq.top();
        pq.pop();

        if (contracted[v]) continue;

        // lazy importance recomputation
        double new_imp = importance(v, adj, contracted);
        if (new_imp > imp) {
            pq.emplace(new_imp, v);
            continue;
        }

        // assuming undirected graph, this gets the weight dv of all u->v->w paths
        // u,w are neighbours of v
        std::vector<std::pair<int,double>> neighbors;
        for (auto& [u, w] : adj[v]) {
            if (!contracted[u]) {
                neighbors.push_back({u, w});
            }
        }

        for (size_t i = 0; i < neighbors.size(); i++) {
            int u = neighbors[i].first;
            double w_uv = neighbors[i].second;

            for (size_t j = i + 1; j < neighbors.size(); j++) {
                int w = neighbors[j].first;
                double w_vw = neighbors[j].second;

                double dv = w_uv + w_vw;

                // after a witness search u->w excluding v,
                // if no path <= dv exists, we add shortcut u->w
                if (!witness_search(adj, u, w, v, dv, contracted)) {
                    adj[u].push_back({w, dv});
                    adj[w].push_back({u, dv});

                    all_arcs.push_back(CHArc{u, w, dv, v});
                    all_arcs.push_back(CHArc{w, u, dv, v});
                }
            }
        }

        contracted[v] = 1; // mark as contracted
        rank[v] = current_rank++;

        // update importance of neighbours lazily
        for (auto& [u, _] : adj[v]) {
            if (!contracted[u]) {
                pq.emplace(importance(u, adj, contracted), u);
            }
        }
    }

    // this adds all original edges to all_arcs
    for (int u = 0; u < n; u++) {
        for (int ei = graph.first_out[u]; ei < graph.first_out[u + 1]; ei++) {
            int v = graph.to[ei];
            double w = graph.weights[ei];
            all_arcs.push_back(CHArc{u, v, w, -1});
        }
    }

    // build forward graph
    preproc_graph.forward_first_out.assign(n + 1, 0);

    for (const auto& arc : all_arcs) {
        if (rank[arc.from] < rank[arc.to]) {
            preproc_graph.forward_first_out[arc.from + 1]++;
        }
    }
    for (int i = 0; i < n; i++) {
        preproc_graph.forward_first_out[i + 1] +=
            preproc_graph.forward_first_out[i];
    }

    preproc_graph.forward_arcs.resize(
        preproc_graph.forward_first_out[n]
    );

    std::vector<int> fpos = preproc_graph.forward_first_out;
    for (const auto& arc : all_arcs) {
        if (rank[arc.from] < rank[arc.to]) {
            preproc_graph.forward_arcs[fpos[arc.from]++] = arc;
        }
    }

    // build backwards graph
    preproc_graph.backward_first_out.assign(n + 1, 0);

    for (const auto& arc : all_arcs) {
        if (rank[arc.from] > rank[arc.to]) {
            preproc_graph.backward_first_out[arc.to + 1]++;
        }
    }

    for (int i = 0; i < n; i++) {
        preproc_graph.backward_first_out[i + 1] +=
            preproc_graph.backward_first_out[i];
    }

    preproc_graph.backward_arcs.resize(
        preproc_graph.backward_first_out[n]
    );

    std::vector<int> bpos = preproc_graph.backward_first_out;
    for (const auto& arc : all_arcs) {
        if (rank[arc.from] > rank[arc.to]) {
            preproc_graph.backward_arcs[bpos[arc.to]++] =
                CHArc{arc.to, arc.from, arc.weight, arc.mid_node};
        }
    }
}

void CHGraph::preproc_graph_top_down(const CHGraph::Graph &graph, CHGraph::PreprocGraph &preproc_graph)
{
}

void CHGraph::query_route(const CHGraph::Graph &graph, const CHGraph::PreprocGraph &preproc_graph,
                          const CHGraph::Destination &destination, CHGraph::Route &route)
{
}