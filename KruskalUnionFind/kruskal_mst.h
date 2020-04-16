#ifndef KRUSKAL_MST_H
#define KRUSKAL_MST_H

#include <functional> // std::less
#include <vector>     // std::vector

#include "AdjListGraph.h"
#include "DisjointSet.h"

template <typename Label, typename Weight>
auto kruskal_mst(AdjListGraph<Label, Weight>&& adj_list_graph) noexcept -> std::vector<Edge<Label, Weight>> {
	// this will store the Minimum Spanning Tree
	std::vector<Edge<Label, Weight>> mst;

	// sort edges in non-decreasing order of weight
	auto edges = adj_list_graph.get_sorted_edges(std::less<>{});
	
    auto vertexes = adj_list_graph.get_vertexes();
    const size_t n = vertexes.size();
    const size_t n_stop = n - 1;

	mst.reserve(n_stop);

    /**
     * Create a new Disjoint-Set data structure to store the vertexes.
     * Initially, every vertex is in a separate set.
     * vertexes is no longer accessible after the process.
     */
    DisjointSet<Label> disjoint_set(std::move(vertexes));

	// a Minimum Spanning Tree can have (n - 1) edges at maximum.
	for (auto it = edges.cbegin(); !(it == edges.cend() || mst.size() == n_stop); ++it) {
		const auto& edge = *it;

		const auto& v = edge.get_from();
		const auto& w = edge.get_to();

		if (!disjoint_set.are_connected(v, w)) {
			mst.push_back(edge);
			disjoint_set.unite(v, w);
		}
	}

    return mst;
}

#endif  // KRUSKAL_MST_H
