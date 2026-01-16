#include "ch_graph.hpp"

#include <vector>
#include <queue>
#include <limits>
#include <functional>
#include <utility>
#include <cstddef>
#include <algorithm>

double CHGraph::importance(
    int v,
    const std::vector<std::vector<std::pair<int,double>>>& adj,
    const std::vector<int>& contracted
) {
    int deg = 0;
    for (auto& [u, _] : adj[v]) {
        if (!contracted[u]) deg++;
    }
    return deg * deg + deg;
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
    const int n = adj.size();

    static std::vector<double> dist;
    static std::vector<int> touched;

    if ((int)dist.size() < n)
        dist.assign(n, INF);

    auto reset = [&]() {
        for (int v : touched) dist[v] = INF;
        touched.clear();
    };

    reset();

    using QItem = std::pair<double,int>;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    dist[source] = 0.0;
    touched.push_back(source);
    pq.emplace(0.0, source);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > max_dist)
            break;  //stop expanding, not return false

        if (u == target)
            return true;

        if (d != dist[u]) continue;

        for (auto &[v, w] : adj[u]) {
            if (v == forbidden || contracted[v]) continue;

            const double nd = d + w;
            if (nd < dist[v] && nd <= max_dist) {
                if (dist[v] == INF)
                    touched.push_back(v);
                dist[v] = nd;
                pq.emplace(nd, v);
            }
        }
    }
    return false;
}

void CHGraph::preproc_graph_bottom_up(
    const CHGraph::Graph &graph,
    CHGraph::PreprocGraph &preproc_graph
) {
    const int n = graph.first_out.size() - 1;

    // Build directed adjacency lists

    struct Edge {
        int to;
        double weight;
    };
    // out_adj[u]: all edges u -> v
    // in_adj[v]:  all edges v -> w
    std::vector<std::vector<Edge>> out_adj(n), in_adj(n);

    for (int u = 0; u < n; ++u) {
        for (int e = graph.first_out[u]; e < graph.first_out[u + 1]; ++e) {
            int v = graph.to[e];
            double w = graph.weights[e];
            out_adj[u].push_back({v, w});
            in_adj[v].push_back({u, w});
        }
    }

    // Bookkeeping arrays

    std::vector<int> contracted(n, 0);   // 1 if node already contracted
    std::vector<int> rank(n, -1);        // contraction order
    int current_rank = 0;

    std::vector<CHArc> all_arcs;          // original edges + shortcuts

    // importance function per node

    auto importance = [&](int v) {
        int in_deg = 0, out_deg = 0;

        // count active incoming edges
        for (auto &e : in_adj[v])
            if (!contracted[e.to])
                in_deg++;

        // count active outgoing edges
        for (auto &e : out_adj[v])
            if (!contracted[e.to])
                out_deg++;

        return in_deg * out_deg + out_deg;
    };

    // priority queue with lazy recomputation of importance

    using QItem = std::pair<int, int>; // (importance, node)
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    for (int v = 0; v < n; ++v)
        pq.emplace(importance(v), v);

    // witness search

    auto witness_search = [&](int source, int target, int forbidden, double max_dist) {
        const double INF = std::numeric_limits<double>::infinity();
        std::vector<double> dist(n, INF);

        using DItem = std::pair<double, int>;
        std::priority_queue<DItem, std::vector<DItem>, std::greater<DItem>> pqw;

        dist[source] = 0.0;
        pqw.emplace(0.0, source);

        while (!pqw.empty()) {
            auto [d, u] = pqw.top();
            pqw.pop();

            if (d > max_dist)
                return false;

            if (u == target)
                return true;

            if (d != dist[u])
                continue;

            for (auto &e : out_adj[u]) {
                int v = e.to;
                if (v == forbidden || contracted[v])
                    continue;

                double nd = d + e.weight;
                if (nd < dist[v] && nd <= max_dist) {
                    dist[v] = nd;
                    pqw.emplace(nd, v);
                }
            }
        }
        return false;
    };

    // contraction loop

    while (!pq.empty()) {
        auto [old_imp, v] = pq.top();
        pq.pop();

        if (contracted[v])
            continue;

        // lazy recomputation (only update importance when you pop a node, if the new imporance is bigger than the old one)
        int new_imp = importance(v);
        if (new_imp > old_imp) {
            pq.emplace(new_imp, v);
            continue;
        }

        // collect in and out edges

        std::vector<Edge> incoming, outgoing;

        for (auto &e : in_adj[v])
            if (!contracted[e.to])
                incoming.push_back(e);

        for (auto &e : out_adj[v])
            if (!contracted[e.to])
                outgoing.push_back(e);

        //try shortcuts for u->v->w

        for (auto &in_e : incoming) {
            int u = in_e.to;
            double w_uv = in_e.weight;

            for (auto &out_e : outgoing) {
                int w = out_e.to;
                double w_vw = out_e.weight;

                if (u == w)
                    continue;

                double shortcut_weight = w_uv + w_vw;

                // if there is a witness u->w then create shortcut
                if (!witness_search(u, w, v, shortcut_weight)) {
                    bool found = false;
                    for (auto &e : out_adj[u]) {
                        if (e.to == w) {
                            found = true;
                            if (shortcut_weight < e.weight)
                                e.weight = shortcut_weight;
                            break;
                        }
                    }
                    if (!found) {
                        out_adj[u].push_back({w, shortcut_weight});
                        in_adj[w].push_back({u, shortcut_weight});
                        all_arcs.push_back(CHArc{u, w, shortcut_weight, v});
                    }
                }
            }
        }

        // contract v

        contracted[v] = 1;
        rank[v] = current_rank++;

        // neighbours change importance 
        for (auto &e : in_adj[v])
            if (!contracted[e.to])
                pq.emplace(importance(e.to), e.to);

        for (auto &e : out_adj[v])
            if (!contracted[e.to])
                pq.emplace(importance(e.to), e.to);
    }

    // add the original edges

    for (int u = 0; u < n; ++u) {
        for (int e = graph.first_out[u]; e < graph.first_out[u + 1]; ++e) {
            int v = graph.to[e];
            double w = graph.weights[e];
            all_arcs.push_back(CHArc{u, v, w, -1});
        }
    }

    // build the forward and backward graphs
    preproc_graph.ranks = rank;

    preproc_graph.forward_first_out.assign(n + 1, 0);
    preproc_graph.backward_first_out.assign(n + 1, 0);

    for (auto &a : all_arcs) {
        if (rank[a.from] < rank[a.to])
            preproc_graph.forward_first_out[a.from + 1]++;
        else if (rank[a.from] > rank[a.to])
            preproc_graph.backward_first_out[a.to + 1]++;
    }

    for (int i = 0; i < n; ++i) {
        preproc_graph.forward_first_out[i + 1] += preproc_graph.forward_first_out[i];
        preproc_graph.backward_first_out[i + 1] += preproc_graph.backward_first_out[i];
    }

    preproc_graph.forward_arcs.resize(preproc_graph.forward_first_out[n]);
    preproc_graph.backward_arcs.resize(preproc_graph.backward_first_out[n]);

    std::vector<int> fpos = preproc_graph.forward_first_out;
    std::vector<int> bpos = preproc_graph.backward_first_out;

    for (auto &a : all_arcs) {
        if (rank[a.from] < rank[a.to])
            preproc_graph.forward_arcs[fpos[a.from]++] = a;
        else if (rank[a.from] > rank[a.to])
            preproc_graph.backward_arcs[bpos[a.to]++] =
                CHArc{a.to, a.from, a.weight, a.mid_node};
    }
}


static std::vector<int> rank_importance(const std::vector<std::vector<int>> &in_deg,
                const std::vector<std::vector<int>> &out_deg)
{
    const int n = in_deg.size();
    std::vector<std::pair<int,int>> scored; // (importance, node)
    scored.reserve(n);

    for (int v = 0; v < n; ++v) {
        int in_d  = in_deg[v].size();
        int out_d = out_deg[v].size();
        int importance = in_d * out_d + out_d;
        scored.emplace_back(importance, v);
    }

    // smaller importance = contracted earlier
    std::sort(scored.begin(), scored.end());

    std::vector<int> rank(n);
    for (int i = 0; i < n; ++i)
        rank[scored[i].second] = i;

    return rank;
}


void CHGraph::preproc_graph_top_down(const CHGraph::Graph &graph, CHGraph::PreprocGraph &preproc_graph)
{
    const int n = graph.first_out.empty() ? 0 : static_cast<int>(graph.first_out.size()) - 1;

    preproc_graph = CHGraph::PreprocGraph{};
    preproc_graph.ranks.assign(n, 0);

    struct OverlayEdge
    {
        int to;
        double weight;
        int mid;
    };

    std::vector<std::vector<OverlayEdge>> out_edges(n);
    std::vector<std::vector<OverlayEdge>> in_edges(n);

    if (n > 0 && static_cast<int>(graph.first_out.size()) >= n + 1)
    {
        for (int u = 0; u < n; ++u)
        {
            const int begin = graph.first_out[u];
            const int end = graph.first_out[u + 1];
            for (int e = begin; e < end; ++e)
            {
                const int v = graph.to[e];
                const double w = graph.weights[e];
                out_edges[u].push_back(OverlayEdge{v, w, -1});
                if (0 <= v && v < n)
                    in_edges[v].push_back(OverlayEdge{u, w, -1});
            }
        }
    }

    const int MID_ORIGINAL = -1;
    auto add_or_decrease = [&](int from, int to, double weight, int mid_node)
    {
        if (from < 0 || from >= n || to < 0 || to >= n)
            return;

        bool found = false;
        for (std::size_t i = 0; i < out_edges[from].size(); ++i)
        {
            if (out_edges[from][i].to == to)
            {
                found = true;
                if (weight < out_edges[from][i].weight)
                {
                    out_edges[from][i].weight = weight;
                    out_edges[from][i].mid = mid_node;
                }
                break;
            }
        }
        if (!found)
            out_edges[from].push_back(OverlayEdge{to, weight, mid_node});

        found = false;
        for (std::size_t i = 0; i < in_edges[to].size(); ++i)
        {
            if (in_edges[to][i].to == from)
            {
                found = true;
                if (weight < in_edges[to][i].weight)
                {
                    in_edges[to][i].weight = weight;
                    in_edges[to][i].mid = mid_node;
                }
                break;
            }
        }
        if (!found)
            in_edges[to].push_back(OverlayEdge{from, weight, mid_node});
    };

    std::vector<std::vector<int>> in_deg(n), out_deg(n);
    for (int v = 0; v < n; ++v) {
        for (auto &e : in_edges[v])
            in_deg[v].push_back(e.to);
        for (auto &e : out_edges[v])
            out_deg[v].push_back(e.to);
    }

    preproc_graph.ranks = rank_importance(in_deg, out_deg);

    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return preproc_graph.ranks[a] < preproc_graph.ranks[b];
    });

    std::vector<unsigned char> contracted(n, 0);

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n, INF);
    std::vector<int> touched;
    touched.reserve(1024);

    auto reset_dist = [&]()
    {
        for (std::size_t i = 0; i < touched.size(); ++i)
            dist[touched[i]] = INF;
        touched.clear();
    };

    typedef std::pair<double, int> QItem;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    for (int idx = 0; idx < n; ++idx)
    {
        const int v = order[idx];
        if (contracted[v])
            continue;

        std::vector<std::pair<int, double>> incoming;
        incoming.reserve(in_edges[v].size());
        for (std::size_t i = 0; i < in_edges[v].size(); ++i)
        {
            const int u = in_edges[v][i].to;
            const double w_uv = in_edges[v][i].weight;
            if (u == v || u < 0 || u >= n || contracted[u])
                continue;

            bool found = false;
            for (std::size_t k = 0; k < incoming.size(); ++k)
            {
                if (incoming[k].first == u)
                {
                    found = true;
                    if (w_uv < incoming[k].second)
                        incoming[k].second = w_uv;
                    break;
                }
            }
            if (!found)
                incoming.push_back(std::make_pair(u, w_uv));
        }

        std::vector<std::pair<int, double>> outgoing;
        outgoing.reserve(out_edges[v].size());
        for (std::size_t i = 0; i < out_edges[v].size(); ++i)
        {
            const int w = out_edges[v][i].to;
            const double w_vw = out_edges[v][i].weight;
            if (w == v || w < 0 || w >= n || contracted[w])
                continue;

            bool found = false;
            for (std::size_t k = 0; k < outgoing.size(); ++k)
            {
                if (outgoing[k].first == w)
                {
                    found = true;
                    if (w_vw < outgoing[k].second)
                        outgoing[k].second = w_vw;
                    break;
                }
            }
            if (!found)
                outgoing.push_back(std::make_pair(w, w_vw));
        }

        if (incoming.empty() || outgoing.empty())
        {
            contracted[v] = 1;
            continue;
        }

        for (std::size_t iu = 0; iu < incoming.size(); ++iu)
        {
            const int u = incoming[iu].first;
            const double w_uv = incoming[iu].second;

            std::vector<std::pair<int, double>> targets;
            targets.reserve(outgoing.size());
            double Pmax = 0.0;
            for (std::size_t iw = 0; iw < outgoing.size(); ++iw)
            {
                const int w = outgoing[iw].first;
                const double w_vw = outgoing[iw].second;
                if (u == w)
                    continue;
                const double Pw = w_uv + w_vw;
                targets.push_back(std::make_pair(w, Pw));
                if (Pw > Pmax)
                    Pmax = Pw;
            }
            if (targets.empty())
                continue;

            reset_dist();
            while (!pq.empty())
                pq.pop();

            dist[u] = 0.0;
            touched.push_back(u);
            pq.push(QItem(0.0, u));

            while (!pq.empty())
            {
                const QItem top = pq.top();
                const double du = top.first;
                const int x = top.second;
                pq.pop();

                if (du != dist[x])
                    continue;
                if (du > Pmax)
                    break;
                if (x == v)
                    continue;
                if (contracted[x] && x != u)
                    continue;

                for (std::size_t ei = 0; ei < out_edges[x].size(); ++ei)
                {
                    const int y = out_edges[x][ei].to;
                    if (y == v || y < 0 || y >= n || contracted[y])
                        continue;

                    const double nd = du + out_edges[x][ei].weight;
                    if (nd > Pmax)
                        continue;

                    if (nd < dist[y])
                    {
                        if (dist[y] == INF)
                            touched.push_back(y);
                        dist[y] = nd;
                        pq.push(QItem(nd, y));
                    }
                }
            }

            for (std::size_t it = 0; it < targets.size(); ++it)
            {
                const int w = targets[it].first;
                const double Pw = targets[it].second;
                if (dist[w] > Pw)
                    add_or_decrease(u, w, Pw, v);
            }
        }

        contracted[v] = 1;
    }

    std::vector<std::vector<CHGraph::CHArc>> f_adj(n), b_adj(n);

    for (int u = 0; u < n; ++u)
    {
        for (std::size_t i = 0; i < out_edges[u].size(); ++i)
        {
            const int v = out_edges[u][i].to;
            if (v < 0 || v >= n || u == v)
                continue;

            if (preproc_graph.ranks[u] < preproc_graph.ranks[v])
                f_adj[u].push_back(CHGraph::CHArc{u, v, out_edges[u][i].weight, out_edges[u][i].mid});
            else if (preproc_graph.ranks[u] > preproc_graph.ranks[v])
                b_adj[v].push_back(CHGraph::CHArc{v, u, out_edges[u][i].weight, out_edges[u][i].mid});
        }
    }

    preproc_graph.forward_first_out.assign(n + 1, 0);
    preproc_graph.backward_first_out.assign(n + 1, 0);

    std::size_t f_total = 0, b_total = 0;
    for (int i = 0; i < n; ++i)
    {
        f_total += f_adj[i].size();
        b_total += b_adj[i].size();
    }

    preproc_graph.forward_arcs.clear();
    preproc_graph.backward_arcs.clear();
    preproc_graph.forward_arcs.reserve(f_total);
    preproc_graph.backward_arcs.reserve(b_total);

    for (int i = 0; i < n; ++i)
    {
        preproc_graph.forward_first_out[i + 1] =
            preproc_graph.forward_first_out[i] + static_cast<int>(f_adj[i].size());
        preproc_graph.forward_arcs.insert(preproc_graph.forward_arcs.end(), f_adj[i].begin(), f_adj[i].end());

        preproc_graph.backward_first_out[i + 1] =
            preproc_graph.backward_first_out[i] + static_cast<int>(b_adj[i].size());
        preproc_graph.backward_arcs.insert(preproc_graph.backward_arcs.end(), b_adj[i].begin(), b_adj[i].end());
    }
}

void CHGraph::query_route(const CHGraph::Graph &graph, const CHGraph::PreprocGraph &preproc_graph, const CHGraph::Destination &destination, CHGraph::Route &route)
{
    route.nodes.clear();
    route.total_weight = std::numeric_limits<double>::infinity();

    const int n = static_cast<int>(preproc_graph.ranks.size());
    const int s = destination.source; //source 
    const int t = destination.target; //destination

    if (n <= 0) return;

    if (s < 0 || s >= n || t < 0 || t >= n) return;

    if (s == t) { 
        route.total_weight = 0.0; 
        return; 
    }


    const double INF = std::numeric_limits<double>::infinity();

    //Stores distances for the forward and backward graph.  Sets inital distance to INF
    std::vector<double> dist_f(n, INF), dist_b(n, INF);

    // Forward and bacwkard graph predecesor arrays for unpacking
    std::vector<int> prev_f(n, -1), prev_b(n, -1);

    using QItem = std::pair<double,int>;
    //Sets pqf (priority queue for forward graph) and pqb (priority queue for backward graph)
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pqf, pqb; 

    dist_f[s] = 0.0; pqf.push(QItem(0.0, s));
    dist_b[t] = 0.0; pqb.push(QItem(0.0, t));

    double best_dist = INF; //set current best distance from s to t
    int meeting_node = -1;  // stores node where both searches meet and achieve best distance

    //Returns distance of an element at the top of a given queue
    auto top_dist = [](const auto &pq)->double { return pq.empty() ? std::numeric_limits<double>::infinity() : pq.top().first; };

    while (!pqf.empty() || !pqb.empty()) {
        double forward_min_dist = top_dist(pqf);
        double backward_min_dist = top_dist(pqb);


        bool forward_can_improve = forward_min_dist < best_dist;
        bool backward_can_improve = backward_min_dist < best_dist;
        
        if (!forward_can_improve && !backward_can_improve) break; //stop serach if forward and backward search cannot improve best_dist

        // Continue with direction that can still improve and has smaller distance at the top of the queue
        bool do_forward = forward_can_improve && (!backward_can_improve || forward_min_dist <= backward_min_dist);

        if (do_forward) { //Search on forward graph
            
            auto [d,u] = pqf.top(); // d = node distance,  u = node index
            pqf.pop();  //Remove element from the queue

            if (d > dist_f[u]) continue; // Skip if already settled with better distance

            // Stall-on-demand: check if better path via lower-ranked neighbor exists
            if (stall_forward(u, dist_f, preproc_graph)) {
                // Check for meeting point settled in both directions
                if (dist_b[u] < INF) {
                    double candidate_distance = dist_f[u] + dist_b[u];
                    if (candidate_distance < best_dist) { best_dist = candidate_distance; meeting_node = u; }
                }
                continue;
            }

            // Expand outgoing upward arcs
            for (int e = preproc_graph.forward_first_out[u]; e < preproc_graph.forward_first_out[u + 1]; ++e) {
                const CHArc &arc = preproc_graph.forward_arcs[e];
                int v = arc.to;
                double new_distance = d + arc.weight; 
                if (new_distance < dist_f[v]) {
                    dist_f[v] = new_distance;
                    prev_f[v] = u;
                    pqf.push(QItem(new_distance, v));
                }
            }
            

            if (dist_b[u] < INF) {
                double candidate_distance = dist_f[u] + dist_b[u];
                if (candidate_distance < best_dist) { best_dist = candidate_distance; meeting_node = u; }
            }

        } else { //Search on reversed graph
            
            auto [d,u] = pqb.top();  // d = node distance,  u = node index
            pqb.pop(); //Remove element from the  queue
            if (d > dist_b[u]) continue;

            // Stall-on-demand on backward search
            if (stall_backward(u, dist_b, preproc_graph)) {
                if (dist_f[u] < INF) {
                    double candidate_distance = dist_f[u] + dist_b[u];
                    if (candidate_distance < best_dist) { best_dist = candidate_distance; meeting_node = u; }
                }
                continue;
            }

            // Expand outgoing arcs in backwards search
            for (int e = preproc_graph.backward_first_out[u]; e < preproc_graph.backward_first_out[u + 1]; ++e) {
                const CHArc &arc = preproc_graph.backward_arcs[e];
                int v = arc.to; 
                double new_distance = d + arc.weight;
                if (new_distance < dist_b[v]) {
                    dist_b[v] = new_distance;
                    prev_b[v] = u;
                    pqb.push(QItem(new_distance, v));
                }
            }

            // Check meeting point
            if (dist_f[u] < INF) {
                double candidate_distance = dist_f[u] + dist_b[u];
                if (candidate_distance < best_dist) { best_dist = candidate_distance; meeting_node = u; }
            }
        }
    }

    route.total_weight = best_dist;
}

