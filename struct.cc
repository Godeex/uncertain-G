#include "struct.h"

bool Graph::valid(Vertex v) {return v.v_pos >= 0;}
bool Graph::valid(Edge e) {return e.e_pos >= 0;}

int Graph::vertex_number() const {
    return v_storage.size;
}

int Graph::edge_number() const {
    return e_storage.size;
}

Graph::Graph(int v_num, bool count_degree, bool doubly_linked_edges, bool use_vertex_map): count_degree(count_degree),
    doubly_linked_edges(doubly_linked_edges), use_vertex_map(use_vertex_map) {
    if (!use_vertex_map) {
        // printf("resize %d\n", v_num);
        v_storage.resize(v_num);
        // printf("%d\n", v_storage.capacity);
        vertex_load_storage.resize(v_num);
        has_vertex = new bool[v_num];
    }
    // v_storage.resize(v_num);
}
Graph::~Graph() {}

void Graph::save(FILE *fp) {}
void Graph::load(const char *filename) {}

Edge Graph::first_in(Vertex v) const {
    return {v_storage.get(v.v_pos)->first_in};
}

Edge Graph::last_in(Vertex v) const {
    return {v_storage.get(v.v_pos)->last_in};
}

Edge Graph::last_in(Edge e) const {
    return {backward_link_storage.get(e.e_pos)->last_in};
}

Edge Graph::last_out(Vertex v) const {
    return {v_storage.get(v.v_pos)->last_out};
}

Edge Graph::last_out(Edge e) const {
    return {backward_link_storage.get(e.e_pos)->last_out};
}

Edge Graph::next_in(Edge e) const {
    return {e_storage.get(e.e_pos)->next_in};
}

Edge Graph::first_out(Vertex v) const {
    return {v_storage.get(v.v_pos)->first_out};
}

Edge Graph::next_out(Edge e) const {
    return {e_storage.get(e.e_pos)->next_out};
}

Vertex Graph::get_target(Edge e) const {
    return {e_storage.get(e.e_pos)->vertex};
}

Vertex Graph::get_source(Edge e) const {
    return {e_storage.get(e.e_pos)->source};
}

Vertex Graph::get_vertex_by_id(int v_id) const {
    if (use_vertex_map) {
        auto it = vertex_map.find(v_id);
        if (it == vertex_map.end()) {
            return {-1};
        }
        return it->second;
    }
    if (!has_vertex[v_id]) return {-1};
    return {v_id};
}

EdgeLoad Graph::get_edge_load(Edge e) const {
    return *edge_load_storage.get(e.e_pos);
}

VertexLoad Graph::get_vertex_load(Vertex v) const {
    // logger_info << "get load of " << v.v_pos << std::endl;
    return *vertex_load_storage.get(v.v_pos);
}

int Graph::id(Vertex v) const {
    return get_vertex_load(v).id;
}

Vertex Graph::new_vertex(int v_id) {
    if (use_vertex_map) {
        int v_pos = v_storage.newElement({-1, -1, -1, -1});
        vertex_load_storage.newElement({0, 0, v_id});
        vertex_map.insert({v_id, {v_pos}});
        return {v_pos};
    }
    if (!has_vertex[v_id]) {
        v_storage.set(v_id, {-1, -1, -1, -1});
        vertex_load_storage.set(v_id, {0, 0, v_id});
        has_vertex[v_id] = true;
        if (v_storage.size < v_id + 1) {
            v_storage.size = v_id + 1;
            vertex_load_storage.size = v_id + 1;
        }
    }
    return {v_id};
}

void Graph::add_edge(int a_id, int b_id, const EdgeLoad &p) {
    // get vertex by id, create if not exist
    int a_pos = get_vertex_by_id(a_id).v_pos;
    int b_pos = get_vertex_by_id(b_id).v_pos;
    if (a_pos == -1) a_pos = new_vertex(a_id).v_pos;
    if (b_pos == -1) b_pos = new_vertex(b_id).v_pos;
    // new edge
    int e_pos = e_storage.newElement({});
    edge_load_storage.newElement(p);
    // update edge linked list
    // printf("%d %d\n", a_pos, b_pos);
    auto base_va = v_storage._get(a_pos);
    auto base_vb = v_storage._get(b_pos);
    // printf("%d %d\n", base_vb->first_in, base_va->first_out/);
    if (doubly_linked_edges) {
        // for vertex a: e.last_out=a.last_out, a.last_out.next_out=e, a.last_out=e, e.next_out=-1, update a.first_out if a.first_out=-1
        // for vertex b: e.last_in=b.last_in, b.last_in.next_in=e, b.last_in=e, e.next_in=-1, update b.first_in if b.first_in=-1
        e_storage.set(e_pos, {a_pos, b_pos, -1, -1});
        backward_link_storage.newElement({base_vb->last_in, base_va->last_out});
        if (base_va->last_out != -1) {
            e_storage._get(base_va->last_out)->next_out = e_pos;
        }
        if (base_vb->last_in != -1) {
            e_storage._get(base_vb->last_in)->next_in = e_pos;
        }
        if (base_va->first_out == -1) {
            base_va->first_out = e_pos;
        }
        if (base_vb->first_in == -1) {
            base_vb->first_in = e_pos;
        }
        base_va->last_out = e_pos;
        base_vb->last_in = e_pos;
    } else {
        e_storage.set(e_pos, {a_pos, b_pos, base_vb->first_in, base_va->first_out});
        v_storage.set(a_pos, {base_va->first_in, e_pos});
        v_storage.set(b_pos, {e_pos, base_vb->first_out});
    }
    // update vertex degree
    if (count_degree) {
        VertexLoad a_load = *vertex_load_storage.get(a_pos);
        vertex_load_storage.set(a_pos, {a_load.in_degree, a_load.out_degree + 1, a_load.id});
        VertexLoad b_load = *vertex_load_storage.get(b_pos);
        vertex_load_storage.set(b_pos, {b_load.in_degree + 1, b_load.out_degree, b_load.id});
    }
    // logger_info << "add edge "  << a_id << "->" << b_id << std::endl;
    // print();
}

void Graph::update_edge(Edge e, const EdgeLoad &edge_load) {
    edge_load_storage.set(e.e_pos, edge_load);
}

void Graph::delete_edge(Edge e) {
    // TODO: delete edge faster
    // TODO: if doubly_linked_edges
    Vertex s = get_source(e);
    Vertex t = get_target(e);
    // logger_info << "deleting eid "<< e.e_pos<<" " << s.v_pos << " " << t.v_pos << std::endl;
    // print();
    auto ed=first_out(s);
    if (ed.e_pos == e.e_pos) {
        v_storage._get(s.v_pos)->first_out = next_out(ed).e_pos;
    } else {
        for (auto nxt=next_out(ed); valid(nxt); ed=nxt, nxt=next_out(nxt)) {
            if (nxt.e_pos == e.e_pos) {
                // delete nxt
                e_storage.set(ed.e_pos, {s.v_pos, get_target(ed).v_pos, next_in(e).e_pos, next_out(nxt).e_pos});
                break;
            }
        }
    }
    ed=first_in(t);
    if (ed.e_pos == e.e_pos) {
        v_storage._get(t.v_pos)->first_in = next_in(ed).e_pos;
    } else {
        logger_info << ed.e_pos << std::endl;
        for (auto nxt=next_in(ed); valid(nxt); ed=nxt, nxt=next_in(nxt)) {
            if (nxt.e_pos == e.e_pos) {
                e_storage.set(ed.e_pos, {get_source(ed).v_pos, t.v_pos, next_in(nxt).e_pos, next_out(e).e_pos});
                break;
            }
        }
    }
    // TODO: count degree
    // logger_info << "after deleting eid "<< e.e_pos<<" " << s.v_pos << " " << t.v_pos << std::endl;
    // print();
}

void Graph::print() {
    for (int i = 0; i < e_storage.size; ++i) {
        logger_info << "edge " << i << " " << id({e_storage.get(i)->source}) << "->" <<id({e_storage.get(i)->vertex}) << " next_in " <<  e_storage.get(i) -> next_in << " next_out " << e_storage.get(i)->next_out;
        if (doubly_linked_edges) {
            logger_info << " last_in " << backward_link_storage.get(i)->last_in << " last_out " << backward_link_storage.get(i)->last_out;
        }
        logger_info << " prob "<< *edge_load_storage.get(i);
        logger_info << std::endl;
    }
    for (int i = 0; i < v_storage.size; ++i) {
        logger_info << "vertex " << i << " f_in " << v_storage.get(i)->first_in << " f_out " <<v_storage.get(i)->first_out;
        if (doubly_linked_edges) {
            logger_info << " last_in " << v_storage.get(i)->last_in << " last_out " << v_storage.get(i)->last_out;
        }
        logger_info << std::endl;
    }
}

const char *to_string(GraphProbability model) {
    switch (model) {
        case WEIGHTED_CASCADE: return "Weighted Cascade";
        case TRIVALENCY: return "Trivalency";
        case DBLP_D20: return "dblp0.05";
        case DBLP_D5: return "dblp0.2";
    }
    return "";
}

Distance::Distance() {}
Distance::Distance(Vertex node, Vertex parent, double dis, int hop): node(node), parent(parent), distance(dis), hop(hop) {}
bool Distance::operator < (const Distance &that) {
    return distance > that.distance;
}
