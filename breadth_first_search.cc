#include "breadth_first_search.h"
#include "logger.h"
BreadthFirstSearch::BreadthFirstSearch(Graph &graph): graph(graph), q(new Vertex[graph.vertex_number()]), visited(new int[graph.vertex_number()]()),
    visit_flag(0) {
    reset({});
}
BreadthFirstSearch::~BreadthFirstSearch() {
    delete []q;
    delete []visited;
}

void BreadthFirstSearch::reset(const std::vector<Vertex> &initial_set, int d) {
    if (visit_flag >= MAX_FLAG) {
        memset(visited, 0, sizeof(int) * graph.vertex_number());
        visit_flag = 0;
    }
    visit_flag++;

    op = cl = cursor = 0;
    for (auto i: initial_set) {
        visited[i.v_pos] = visit_flag;
        q[cl++] = i;
    }
    direction = d;
}

Vertex BreadthFirstSearch::next() {
    while (cursor >= cl && op < cl) {
        Vertex x = q[op++];
        auto next = direction ? &Graph::next_in : &Graph::next_out;
        auto get_node = direction ? &Graph::get_source : &Graph::get_target; // TODO: move to reset
        for (auto edge = direction ? graph.first_in(x) : graph.first_out(x); Graph::valid(edge); edge=(graph.*next)(edge)) {
            // TODO: how to check removed edge
            auto y = (graph.*get_node)(edge);
            if (visited[y.v_pos] != visit_flag) {
                visited[y.v_pos] = visit_flag;
                q[cl++] = y;
            }
        }
    }
    return cursor < cl ? q[cursor++] : (Vertex){-1}; // TODO: const invalid Vertex
}

DijkstraBreadthFirstSearch::DijkstraBreadthFirstSearch(Graph &graph): graph(graph), visited(new int[graph.vertex_number()]()),
    visit_flag(0), heap_nodes(new int[graph.vertex_number()]) {
}
DijkstraBreadthFirstSearch::~DijkstraBreadthFirstSearch() {
    delete []heap_nodes;
    delete []visited;
}
void DijkstraBreadthFirstSearch::reset(const std::vector<Vertex> &initial_set, int d) {
    if (visit_flag >= MAX_FLAG) {
        memset(visited, 0, sizeof(int) * graph.vertex_number());
        visit_flag = 0;
    }
    visit_flag += 2;

    heap.reset();
    for (auto i: initial_set) {
        visited[i.v_pos] = visit_flag;
        heap_nodes[i.v_pos] = heap.insert({i, i, 1, 0});
    }
    direction = d;
}

Distance DijkstraBreadthFirstSearch::next(bool close) {
    if (!heap.empty()) {
        const Distance &disx = heap.root_value();
        heap.delete_min();
        Vertex x = disx.node;
        // logger_info << "BFS from " << disx.node << " " << x << std::endl;
        visited[x.v_pos] = visit_flag + 1;

        if (close) {
            return disx;
        }
        // explore neighbors of x
        auto next = direction ? &Graph::next_in : &Graph::next_out;
        auto get_node = direction ? &Graph::get_source : &Graph::get_target;
        for (auto edge = direction ? graph.first_in(x) : graph.first_out(x); Graph::valid(edge); edge=(graph.*next)(edge)) {
            auto y = (graph.*get_node)(edge);
            double p = graph.get_edge_load(edge);
            if (visited[y.v_pos] < visit_flag) {
                visited[y.v_pos] = visit_flag;
                // printf("%f\n", disx.distance);
                // double u = disx.distance;
                heap_nodes[y.v_pos] = heap.insert({y, x, disx.distance * p, disx.hop + 1});
            } else if (visited[y.v_pos] == visit_flag && heap.value(heap_nodes[y.v_pos]).distance < disx.distance * p) {
                heap.decreaseKey(heap_nodes[y.v_pos], {y, x, disx.distance * p, disx.hop + 1});
            }
        }
        return disx;
    }

    return {{-1}, {-1}, 0, 0};
}

Distance DijkstraBreadthFirstSearch::front() const {
    return !heap.empty() ? (Distance) (heap.root_value()) : (Distance){{-1}, {-1}, 0, 0};
}
