#ifndef ADJACENCY_MAP_GRAPH_H
#define ADJACENCY_MAP_GRAPH_H

#include <algorithm>      // std::transform, std::sort
#include <unordered_map>  // std::unordered_map
#include <unordered_set>  // std::unordered_set
#include <vector>         // std::vector

/**
 * Edge represents a weighted edge between two vertexes.
 */
template <typename Label, typename Weight>
struct Edge {
    Label from;
    Label to;
    Weight weight;

    /**
     * Default constructor used to represent "null" values when the MST is initialized
     * in Prim's algorithm.
     */
    Edge() noexcept = default;

    explicit Edge(const Label& from, const Label& to, const Weight& weight) noexcept :
        from(from), to(to), weight(weight) {
    }

    /**
     * Default assignment operator, needed to sort a container of Edges in place
     */
    Edge<Label, Weight>& operator=(const Edge<Label, Weight>& rhs) = default;

    /**
     * Default copy and move constructors, needed because in Prim's algorithm we need
     * to override edges
     */
    Edge(const Edge<Label, Weight>& rhs) = default;
    Edge(Edge<Label, Weight>&& rhs) = default;

    /**
     * Equality operator, used by std::unordered_set.
     * (from, to, w1) and (to, from, w2) are considered the same edge, even if
     * the weights are different.
     */
    bool operator==(const Edge& e) const {
        return (from == e.from && to == e.to) || (to == e.from && from == e.to);
    }
};

/**
 * Hash functors for custom types
 */
namespace custom_hash {
    // commutative hash functor for Edge<Label, Weight>
    struct edge_hash {
        template <class Label, class Weight>
        std::size_t operator()(const Edge<Label, Weight>& edge) const {
            constexpr auto hash_max = std::numeric_limits<Label>::max();
            const auto& [i, j, _] = edge;
            return (i * j + (i * i) * (j * j) + (i * i * i) * (j * j * j)) % hash_max;
        }
    };
}  // namespace custom_hash

/**
 * Adjacency Map class for undirected weighted graphs.
 * The given nodes must be labeled as unsigned numbers x.
 *
 * Label is the type of a node label, which must be an unsigned integer.
 * Weight is the type of an edge weight, which must be a signed integer.
 */
template <typename Label, typename Weight>
class AdjacencyMapGraph {
    using vertex_weight_map_t = std::unordered_map<Label, Weight>;
    using adj_map_t = std::unordered_map<Label, vertex_weight_map_t>;
    using edge_set_t = std::unordered_set<Edge<Label, Weight>, custom_hash::edge_hash>;

    /**
     * Make DFSCycleDetection a friend class for performance reasons, so it can iterate over the
     * vertexes accessing the private member adj_map directly, instead of calling the get_vertexes()
     * method (O(1) vs O(n)).
     */
    template <typename L, typename W>
    friend class DFSCycleDetection;

    /**
     * Map that stores the graph vertexes as keys and the (vertex, weight) pair connected to each
     * key as values.
     */
    adj_map_t adj_map;

    /**
     * Set used for O(1) edges retrieval
     */
    edge_set_t edge_set;

    /**
     * Initializes the graph starting from a vector of edges. The given number of vertexes is used
     * to efficiently preallocate memory and void exprensive and multiple rehashes. Time: O(n + m)
     * Space: O(n + m)
     */
    void init(const std::vector<Edge<Label, Weight>>& edge_list, const size_t n_vertex);

public:
    /**
     * Creates a graph representation starting from a vector of edges.
     * Time: O(n + m)
     * Space: O(n + m)
     */
    explicit AdjacencyMapGraph(std::vector<Edge<Label, Weight>>&& edge_list,
                               const size_t n_vertex = 0) noexcept {
        init(edge_list, n_vertex);
    }

    /**
     * Return the number of vertexes stored.
     * Time:  O(1)
     * Space: O(1)
     */
    [[nodiscard]] size_t vertexes_size() const;

    /**
     * Return the list of vertexes.
     * Time:  O(n)
     * Space: O(n)
     */
    [[nodiscard]] std::vector<Label> get_vertexes() const;

    /**
     * Return the set of edges.
     * Time:  O(1)
     * Space: O(1)
     */
    [[nodiscard]] const edge_set_t& get_edges() const;

    /**
     * Return the set of edges sorted by weight according to the given comparator.
     * Time:  O(mlogm)
     * Space: O(m)
     */
    template <class Comparator>
    [[nodiscard]] std::vector<Edge<Label, Weight>> get_sorted_edges(Comparator&& comparator) const;

    /**
     * Return the set of vertexes adjacent to the given vertex.
     * Time:  O(1)
     * Space: O(1)
     */
    [[nodiscard]] const vertex_weight_map_t& adjacent_vertexes(const Label& vertex) const;

    /**
     * Add a new non-directed edge.
     * Time:  O(1) amortized
     * Space: O(1)
     */
    void add_edge(const Edge<Label, Weight>& edge);

    /**
     * Remove edge from the storage.
     * Time:  O(1) amortized
     * Space: O(1)
     */
    void remove_edge(const Edge<Label, Weight>& edge);
};

template <typename Label, typename Weight>
inline void AdjacencyMapGraph<Label, Weight>::init(
    const std::vector<Edge<Label, Weight>>& edge_list, const size_t n_vertex) {
    // preallocate memory
    adj_map.reserve(n_vertex);
    edge_set.reserve(edge_list.size());

    for (const auto& edge : edge_list) {
        add_edge(edge);
    }
}

template <typename Label, typename Weight>
inline size_t AdjacencyMapGraph<Label, Weight>::vertexes_size() const {
    return adj_map.size();
}

template <typename Label, typename Weight>
inline std::vector<Label> AdjacencyMapGraph<Label, Weight>::get_vertexes() const {
    std::vector<Label> vertexes;
    vertexes.reserve(adj_map.size());

    // copy adj_map keys into the vertexes vector
    std::transform(adj_map.cbegin(), adj_map.cend(), std::back_inserter(vertexes),
                   [](const auto& map_entry) {
                       return map_entry.first;
                   });

    return vertexes;
}

template <typename Label, typename Weight>
inline const std::unordered_set<Edge<Label, Weight>, custom_hash::edge_hash>&
AdjacencyMapGraph<Label, Weight>::get_edges() const {
    return edge_set;
}

template <typename Label, typename Weight>
template <class Comparator>
inline std::vector<Edge<Label, Weight>> AdjacencyMapGraph<Label, Weight>::get_sorted_edges(
    Comparator&& comparator) const {
    const auto edges = get_edges();
    std::vector<Edge<Label, Weight>> sorted_edges;
    sorted_edges.reserve(edges.size());

    // copy the content of edges into sorted_edges
    sorted_edges.insert(sorted_edges.end(), edges.cbegin(), edges.cend());

    // sort the values in sorted_edges applying the comparator to the internal weights
    std::sort(sorted_edges.begin(), sorted_edges.end(),
              [comp{std::move(comparator)}](const auto& l, const auto& r) {
                  return comp(l.weight, r.weight);
              });

    return sorted_edges;
}

template <typename Label, typename Weight>
inline const std::unordered_map<Label, Weight>& AdjacencyMapGraph<Label, Weight>::adjacent_vertexes(
    const Label& vertex) const {
    return adj_map.at(vertex);
}

template <typename Label, typename Weight>
void AdjacencyMapGraph<Label, Weight>::add_edge(const Edge<Label, Weight>& edge) {
    using vertex_weight_map_t = AdjacencyMapGraph<Label, Weight>::vertex_weight_map_t;
    const auto& [from, to, weight] = edge;

    vertex_weight_map_t& adj_map_from = adj_map[from];
    vertex_weight_map_t& adj_map_to = adj_map[to];

    // If the edge is already present, check the weight.
    if (adj_map_from.count(to)) {
        if (adj_map_from[to] > weight) {
            adj_map_from[to] = weight;
            adj_map_to[from] = weight;

            // removes the old edge by keys from and to and
            // re-add the new edge with the new weight
            edge_set.erase(edge);
            edge_set.insert(edge);
        }
    }
    // else, add the edge to the non-directed graph
    else {
        adj_map_from[to] = weight;
        adj_map_to[from] = weight;
        edge_set.insert(edge);
    }
}

template <typename Label, typename Weight>
inline void AdjacencyMapGraph<Label, Weight>::remove_edge(const Edge<Label, Weight>& edge) {
    const auto& [from, to, _] = edge;

    adj_map[from].erase(to);
    adj_map[to].erase(from);
    edge_set.erase(edge);
}

#endif  // ADJACENCY_MAP_GRAPH_H
