#ifndef ADJ_LIST_GRAPH_H
#define ADJ_LIST_GRAPH_H

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

/**
 * Edge is a class that represents a weighted edge from a node to another,
 * with a particular weight.
 */
template <typename Label, typename Weight>
class Edge {
	Label from;
	Label to;
	Weight weight;

public:
	Edge() noexcept : from(Label()),
	                  to(Label()),
	                  weight(Weight()) {}

	explicit Edge(const Label& from, const Label& to, const Weight& weight) noexcept : from(from),
	                                                                                   to(to),
	                                                                                   weight(weight) {}

	// // default copy and move constructors
	Edge(const Edge<Label, Weight>& rhs) = default;
	Edge(Edge<Label, Weight>&& rhs) = default;

	// default destructor
	~Edge() = default;

	/**
	 * The assignment operator is necessary because when edges are sorted,
	 * that happens in place.
	 */
	Edge<Label, Weight>& operator=(const Edge<Label, Weight>& rhs) = default;

	[[nodiscard]] Label get_from() const { return from; }

	[[nodiscard]] Label get_to() const { return to; }

	[[nodiscard]] Weight get_weight() const { return weight; }

	// friend function to print an instance of Node to the standard output.
	template <typename L, typename W>
	friend std::ostream& operator<<(std::ostream& os, const Edge<L, W>& edge);
};

template <typename Label, typename Weight>
std::ostream& operator<<(std::ostream& os, const Edge<Label, Weight>& edge) {
	os << edge.from << " <-> " << edge.to << " (" << edge.weight << ")" << std::endl;
	return os;
}

/**
 * Adjacent List class for undirected weighted graphs.
 * The given nodes must be labeled as unsigned numbers x, where 0 <= x <= n.
 *
 * Label is the type of a node label, which must be an unsigned integer.
 * Weight is the type of an edge weight, which must be a signed integer.
 */
template <typename Label, typename Weight>
class AdjListGraph {
public:
	/**
	 * WeightedEdgeLink is a struct that represents the end of an edge and its weight from
	 * a source vertex. It's used for the implementation of AdjListGraph.
	 */
	struct WeightedEdgeLink {
		const Label vertex;
		Weight weight;

		explicit WeightedEdgeLink(const Label vertex, const Weight weight) noexcept : vertex(vertex),
		                                                                              weight(weight) {}
	};

private:
	// vector of vectors that represents an adjacency list
	std::unordered_map<Label, std::vector<WeightedEdgeLink>> adj_map_list;

	// function invoked by the constructors
	void init(const std::vector<Edge<Label, Weight>>& edge_list, const size_t n_vertex) {
		/**
		 * Reserve exactly as many buckets as the number of keys in the map
		 * to avoid multiple rehashes that the increases in container size
		 * could have produced.
		 */
		adj_map_list.reserve(n_vertex);

		/**
		 * Assume that the vertexes are identified by a value x where Label(1) <= x <= Label(n_vertex).
		 */
		for (Label x = 0; x < n_vertex; x++) {
			/**
			 * using operator[](x) sets a new key if it doesn't exist yet.
			 * Vectors are automatically initialized to their 0-value,
			 * i.e. an empty (but valid) std::vector.
			 * If we don't initialize the map in this way, we risk to lose some vertex along
			 * the way (i.e. in the case where some vertexes don't point to any other vertexes.
			 */
			adj_map_list[x];
		}

		for (const auto& edge : edge_list) { add_edge(edge); }

		// TODO: remove duplicate arcs of non-minimum weight
	}

public:
	/**
	 * edge_list: vector of edges to insert into the graph
	 * n_vertex: total number of vertexes in the graph
	 * init_value: initial label for the nodes
	 */
	explicit AdjListGraph(const std::vector<Edge<Label, Weight>>& edge_list = {}, const size_t n_vertex = 0) noexcept {
		init(edge_list, n_vertex);
	}

	explicit AdjListGraph(std::vector<Edge<Label, Weight>>&& edge_list, const size_t n_vertex = 0) noexcept {
		init(edge_list, n_vertex);
	}

	// disable copy constructor
	AdjListGraph& operator==(const AdjListGraph<Label, Weight>& rhs) = delete;

	// default move constructor
	AdjListGraph(AdjListGraph<Label, Weight>&& rhs) noexcept = default;

	// default assignment operator
	AdjListGraph(const AdjListGraph<Label, Weight>& rhs) = default;

	// default destructor, since no free-pointer is used.
	~AdjListGraph() = default;

	// friend function to print an instance of AdjListGraph to the standard output.
	template <typename L, typename W>
	friend std::ostream& operator<<(std::ostream& os, const AdjListGraph<L, W>& adj_list_graph);

	// return the number of vertexes in the Adjacency List
	[[nodiscard]] size_t vertexes_size() const { return adj_map_list.size(); }

	// return the list of vertexes in the Adjacency List. There's no ordering guarantee.
	[[nodiscard]] std::vector<Label> get_vertexes() const {
		std::vector<Label> vertexes;
		vertexes.reserve(adj_map_list.size());

		std::transform(adj_map_list.cbegin(), adj_map_list.cend(), std::back_inserter(vertexes),
		               [](const auto& map_entry) { return map_entry.first; });
		return vertexes;
	}

	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};


	// return the list of edges in the Adjacency List. There's no ordering guarantee.
	[[nodiscard]] std::vector<Edge<Label, Weight>> get_edges(bool directed = true) const {
		std::vector<Edge<Label, Weight>> edges;

		if (directed) {
			for (const auto& map_entry : adj_map_list) {
				const auto& from = map_entry.first;
				const auto& w_edge_link_list = map_entry.second;

				for (const auto& w_edge_link : w_edge_link_list) {
					const auto& to = w_edge_link.vertex;
					const auto& weight = w_edge_link.weight;
					edges.emplace_back(from, to, weight);
				}
			}
		} else {
			std::unordered_map<std::pair<Label, Label>, Weight, pair_hash> edge_map;
			for (const auto& map_entry : adj_map_list) {
				const auto& from = map_entry.first;
				const auto& w_edge_link_list = map_entry.second;

				for (const auto& w_edge_link : w_edge_link_list) {
					const auto& to = w_edge_link.vertex;
					const auto& weight = w_edge_link.weight;

					auto new_edge = std::make_pair(std::min(from, to), std::max(from, to));
					edge_map[new_edge] = weight;
				}
			}

			for (const auto& edge : edge_map) {
				const auto from = edge.first.first;
				const auto to = edge.first.second;
				const auto weight = edge.second;
				edges.emplace_back(from, to, weight);
			}
		}


		return edges;
	}

	// return the list of the nodes adjacent to the given vertex. The given vertex must exist in
	// the Adjacency List. Each returned node also contains the weight cost from the given vertex
	// to that node.
	[[nodiscard]] std::vector<WeightedEdgeLink> get_adjacent_vertexes(const Label& vertex) const {
		return adj_map_list.at(vertex);
	}

	// adds a new edge to the adjacency list
	void add_edge(const Edge<Label, Weight>& edge) {
		const auto from = edge.get_from();
		const auto to = edge.get_to();
		const auto weight = edge.get_weight();

		// check if there is already a edge in the graph
		auto& adj_from = adj_map_list[from];
		for (size_t i = 0; i < adj_from.size(); i++) {
			if (adj_from[i].vertex == to) {
				if (adj_from[i].weight > weight) {
					adj_from[i].weight = weight;

					//also find and extract the duplicates in other vertex
					auto& adj_to = adj_map_list[to];
					for (size_t j = 0; j < adj_to.size(); j++) {
						if (adj_to[j].vertex == from) {
							adj_to[j].weight = weight;

							//update completed
							return;
						}
					}
				} else {
					//skip the vertex if the weight is more than the actual
					return;
				}
			}
		}

		//add if not already present
		// we're dealing with undirected graphs, so the edges should go in both ways
		adj_map_list[from].emplace_back(to, weight);
		adj_map_list[to].emplace_back(from, weight);
	}

	// removes the last WeightedEdgeLink from the vector relative to the given edge
	void remove_last_from_edge(Edge<Label, Weight>& edge) {
		adj_map_list[edge.get_from()].pop_back();
		adj_map_list[edge.get_to()].pop_back();
	}
};

template <typename Label, typename Weight>
std::ostream& operator<<(std::ostream& os, const AdjListGraph<Label, Weight>& adj_list_graph) {
	for (const auto& map_entry : adj_list_graph.adj_map_list) {
		const auto& key = map_entry.first;
		const auto& w_edge_link_list = map_entry.second;

		for (const auto& w_edge_link : w_edge_link_list) {
			os << key << " <-> " << w_edge_link.vertex << " (" << w_edge_link.weight << ")" << std::endl;
		}
	}

	os << std::endl;
	return os;
}

#endif  // ADJ_LIST_GRAPH_H
