#ifndef DATA_STRUCT_FOR_IC_MODEL
#define DATA_STRUCT_FOR_IC_MODEL
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>

#include "logger.h"

// TODO: it is the same as vector
template<typename T>
struct Storage {
    static const unsigned int expand_factor = 2, max_size = 1 << 31, initial_size = 1 << 4;
    unsigned int size, capacity;
    T *storage;
    Storage(): size(0), capacity(initial_size), storage(new T[initial_size]()) {}
    ~Storage() {
        delete []storage;
    }
    int newElement(const T &e) { // return position/id of element
        if (size == max_size) {
            // TODO: throw error;
        } else if (size == capacity) {
            resize(capacity << 1);
        }
        set(size, e);
        return size++;
    }
    const T *get(int pos) const {
        return pos < size ? storage+pos : NULL;
        // return pos < size;
    }
    T *_get(int pos) {
        return pos < size ? storage+pos : NULL;
    }
    void set(int pos, const T&e) {
        memcpy(storage + pos, &e, sizeof(T));
    }
    void resize(int s) {
        if (s < max_size) {
            int new_capacity = capacity;
            while (new_capacity < s) {
                new_capacity <<= 1;
            }
            T *new_storage = new T[new_capacity];
            memcpy(new_storage, storage, sizeof(T) * (capacity));
            delete []storage;
            capacity = new_capacity;
            storage = new_storage;
        }
    }
};

struct BaseVertex {
    int first_in, first_out;
    int last_in, last_out;
};

struct BaseEdge {
    int source;
    int vertex;
    int next_in, next_out;
};

struct BackwardLink {
    int last_in, last_out;
};

typedef double EdgeLoad; // TODO: template

struct Edge {
    int e_pos;
};

struct VertexLoad { // TODO: inheritable
    int in_degree, out_degree;
    int id;
};

struct Vertex {
    int v_pos;
};

struct Graph {
    bool count_degree;
    bool doubly_linked_edges;
    bool use_vertex_map;
    bool *has_vertex;

    Storage<BaseVertex> v_storage;
    Storage<BaseEdge> e_storage;
    Storage<EdgeLoad> edge_load_storage;
    Storage<VertexLoad> vertex_load_storage;
    Storage<BackwardLink> backward_link_storage;

    int vertex_number() const;
    int edge_number() const;
    void save(FILE *fp);
    void load(const char *filename);

    Graph(int v_num=0, bool count_degree=false, bool doubly_linked_edges=false, bool 
    use_vertex_map=true); // use_vertex_map=false: vertex id should be consecutive, 0~max_id should all appear in edges 
    ~Graph();

    Edge first_in(Vertex) const;
    Edge last_in(Vertex) const;
    Edge next_in(Edge) const;
    Edge last_in(Edge) const;
    Edge first_out(Vertex) const;
    Edge last_out(Vertex) const;
    Edge next_out(Edge) const;
    Edge last_out(Edge) const;

    Vertex get_target(Edge) const;
    Vertex get_source(Edge) const;
    Vertex get_vertex_by_id(int v_id) const;
    EdgeLoad get_edge_load(Edge) const;
    VertexLoad get_vertex_load(Vertex) const;
    int id(Vertex) const;
    static bool valid(Edge);
    static bool valid(Vertex);

    // std::vector<Vertex *> *get_vertices();

    Vertex new_vertex(int v_id); // TODO: v_load
    void add_edge(int source_id, int target_id, const EdgeLoad &load);
    // void add_edge(Vertex source, Vertex target, const EdgeLoad &load);
    void update_edge(Edge, const EdgeLoad &edge_load);
    void delete_edge(Edge);

    void print();

protected:
    std::unordered_map<int, Vertex> vertex_map; // <v_id, Vertex>
    // std::vector<int> *vertex_positions; // vertex_positions[v_id] = v_pos
    // static std::vector<int> empty_edges;
    // int get_vertex_pos(int v_id) const;
    // Vertex* get_vertex(int v_id) const;
};

struct Distance {
    Vertex node, parent;
    double distance;
    int hop;
    Distance();
    Distance(Vertex node, Vertex parent, double dis, int hop);
    bool operator < (const Distance &that);
};

enum GraphProbability {
    WEIGHTED_CASCADE,
    TRIVALENCY,
    DBLP_D5,
    DBLP_D20,
    GRAPH_PROBABILITY_NUMBER,
};
const char *to_string(GraphProbability model);

#endif