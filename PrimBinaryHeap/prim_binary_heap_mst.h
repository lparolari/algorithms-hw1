#ifndef PRIM_BINARY_HEAP_MST_H
#define PRIM_BINARY_HEAP_MST_H

#include <algorithm>  // std::transform
#include <limits>     // std::numeric_limits
#include <queue>      // std::priority_queue
#include <vector>     // std::vector

#include "AdjListGraph.h"
#include "PriorityQueue.h"

template <typename Label, typename Weight>
auto prim_binary_heap_mst(AdjListGraph<Label, Weight>&& adj_list_graph) noexcept -> std::vector<Edge<Label, Weight>> {
    auto vertexes = adj_list_graph.get_vertexes();

    const size_t n_stop = vertexes.size();
    std::vector<Edge<Label, Weight>> mst(n_stop);

    // Keys are used to pick the lightest edge in cut.
    // Initially, keys of all vertexes are set to infinity.
    std::vector<Weight> keys;
    keys.reserve(vertexes.size());
    std::transform(vertexes.cbegin(), vertexes.cend(), std::back_inserter(keys), [](const auto&) {
        return std::numeric_limits<Weight>::max();
    });

    // the source vertex can be randomly chosen. For simplicity, we choose the first vertex available.
    // the first vertex is distant 0 from itself
    keys.at(0) = Weight(0);

    // Priority Queue based on a Min Heap ordered by keys, from smaller to larger.
    // We don't need the O(N) to reorder the vertexes as a heap, since keys is already
    // a valid heap.
    constexpr bool is_heap_on_init = true;
    auto priority_queue(make_min_priority_queue<Weight, Label, is_heap_on_init>(std::move(keys), std::move(vertexes)));

    while (!priority_queue.empty()) {
        // u is the vertex with minimum key that belongs to the lightest edge of the cut
        auto u = priority_queue.top();
        priority_queue.pop();

        // traverse all vertexes which are adjacent to u
        for (const auto vw : adj_list_graph.get_adjacent_vertexes(u)) {
            const auto v = vw.vertex;
            const auto weight = vw.weight;  // w(u, v)

            // if v is not in MST and w(u, v) is smaller than the current key of v
            if (priority_queue.contains(v) && weight < priority_queue.key_at(v)) {
                // update the key associated with node v in O(logN), where N is the number
                // of elements stored in the priority queue
                priority_queue.update_key(weight, v);

                // add the edge (u, v) to the Minimum Spanning Tree
                mst.at(v) = Edge<Label, Weight>(u, v, weight);
            }
        }
    }

    return mst;
}

#endif  // PRIM_BINARY_HEAP_MST_H
