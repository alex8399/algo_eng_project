#include "ch_graph.hpp"

#include <vector>
#include <queue>
#include <limits>
#include <functional>
#include <utility>
#include <cstddef>
#include <algorithm>


void CHGraph::preproc_graph_bottom_up(const CHGraph::Graph &graph, CHGraph::PreprocGraph &preproc_graph)
{
}

static std::vector<int>
rank_importance(const std::vector<std::vector<int>> &in_deg,
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
    const int n = graph.num_nodes;

    // reset output
    preproc_graph = CHGraph::PreprocGraph{};
    preproc_graph.num_nodes = n;
    preproc_graph.ranks.assign(n, 0);

    struct OverlayEdge
    {
        int to;
        double weight;
        int mid; // -1 for original edges, otherwise contracted middle node
    };

    std::vector<std::vector<OverlayEdge>> out_edges(n);
    std::vector<std::vector<OverlayEdge>> in_edges(n);

    // build overlay from CSR input.
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
                    in_edges[v].push_back(OverlayEdge{u, w, -1}); // store 'from' in .to field
            }
        }
    }

    // add edge or decrease its weight; keep reverse adjacency consistent.
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

    // compute heuristic-based ranking
    preproc_graph.ranks = rank_importance(in_deg, out_deg);


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

    typedef std::pair<double, int> QItem; // (dist, node)
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    for (int v = 0; v < n; ++v)
    {
        if (contracted[v])
            continue;

        // incoming neighbors U: edges u -> v (stored in in_edges[v] as {u,...})
        std::vector<std::pair<int, double>> incoming; // (u, w_uv)
        incoming.reserve(in_edges[v].size());
        for (std::size_t i = 0; i < in_edges[v].size(); ++i)
        {
            const int u = in_edges[v][i].to; // 'from'
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

        // outgoing neighbors W: edges v -> w
        std::vector<std::pair<int, double>> outgoing; // (w, w_vw)
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

        // run witness search and add missing shortcuts u->w (mid_node = v).
        for (std::size_t iu = 0; iu < incoming.size(); ++iu)
        {
            const int u = incoming[iu].first;
            const double w_uv = incoming[iu].second;

            // targets W with via-cost P_w = w(u,v)+w(v,w)
            std::vector<std::pair<int, double>> targets; // (w, P_w)
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

            // witness dijkstra from u on overlay excluding v and contracted nodes; stop at Pmax.
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

            // add shortcuts where witness path is worse than going via v.
            for (std::size_t it = 0; it < targets.size(); ++it)
            {
                const int w = targets[it].first;
                const double Pw = targets[it].second;
                if (dist[w] > Pw)
                    add_or_decrease(u, w, Pw, v); // middle node approach
            }
        }

        contracted[v] = 1;
    }

    // build the forward/ downward graphs
    std::vector<std::vector<CHGraph::CHArc>> f_adj(n), b_adj(n);

    for (int u = 0; u < n; ++u)
    {
        for (std::size_t i = 0; i < out_edges[u].size(); ++i)
        {
            const int v = out_edges[u][i].to;
            if (v < 0 || v >= n || u == v)
                continue;

            if (preproc_graph.ranks[u] < preproc_graph.ranks[v])
            {
                f_adj[u].push_back(CHGraph::CHArc{u, v, out_edges[u][i].weight, out_edges[u][i].mid});
                b_adj[v].push_back(CHGraph::CHArc{v, u, out_edges[u][i].weight, out_edges[u][i].mid});
            }
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
    
}