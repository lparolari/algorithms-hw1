#include <iostream>

#include "adj_list_graph_factory.h"
#include "sum_weights.h"
#include "kruskal_mst.h"

int main() noexcept {
    typedef size_t Label; // nodes are identified by size_t type
    typedef long Weight;  // weights are of type long

    AdjListGraph<Label, Weight> adj_list_graph(adj_list_graph_factory<Label, Weight>());

    // compute Minimum Spanning Tree with Kruskal algorithm using Disjoint-Set data structure
    const auto& mst = kruskal_mst(std::move(adj_list_graph));

    // total weight of the mst found by Kruskal's algorithm
    const auto total_weight = sum_weights<Label, Weight>(mst.cbegin(), mst.cend());

    // use std::fixed to avoid displaying numbers in scientific notation
    std::cout << std::fixed << total_weight << std::endl;
}
