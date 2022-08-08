#include <stdio.h>
#include <string>
#include <vector>
#include "struct.h"
#include "graph.h"
#include "query.h"

Query::Query(QueryType query_type): query_type(query_type) {}
DistanceQuery::DistanceQuery(Graph &graph): Query(DISTANCE), graph(graph) {}
DistanceQuery::DistanceQuery(Vertex s, Vertex t, Graph &graph): s(s), t(t), Query(DISTANCE), graph(graph) {}
// InfluenceQuery::InfluenceQuery(): Query(INFLUENCE) {}
// InfluenceQuery::InfluenceQuery(int max_size): max_size(max_size), Query(INFLUENCE) {}

const char *DistanceQuery::dump() {
    s_id = graph.get_vertex_load(s).id;
    t_id = graph.get_vertex_load(t).id;
    return (new std::string(std::to_string(s_id) + " " + std::to_string(t_id) + "\n"))->c_str();
}
void DistanceQuery::load(const char *s) {
    sscanf(s, "%d%d", &s_id, &t_id);
    this->s = graph.get_vertex_by_id(s_id);
    this->t = graph.get_vertex_by_id(t_id);
    // logger_info << "load s->t " << this->s_id << "->" << this->t_id << std::endl;
}

// const char *InfluenceQuery::dump() {
//     return (new std::string(std::to_string(max_size)))->c_str();
// }

// void InfluenceQuery::load(const char *s) {
//     sscanf(s, "%d", &max_size);
// }

void save_query(const char *filename, std::vector<Query*> *queries) {
    FILE *fp = fopen(filename, "w");
    for (auto query: *queries) {
        fputs(query->dump(), fp);
    }
    fclose(fp);
}

Vertex random_neighbor(Vertex x, Graph &graph) {
    // return x->edges.size() ? x->edges[rand() % x->edges.size()]->vertex : x;
    if (!graph.valid(x)) return {-1};
    int e_n = graph.get_vertex_load(x).out_degree;
    if (!graph.valid(x) || !e_n) return {-1};
    int counter = rand() % e_n;
    for (auto e = graph.first_out(x); graph.valid(e); e = graph.next_out(e)) {
        if (counter-- == 0) {
            return graph.get_target(e);
        }
    }
    return {-1};
}

bool has_edge(Vertex a, Vertex b, Graph &graph) {
    for (auto e=graph.first_out(a); graph.valid(e); e=graph.next_out(e)) {
        if (graph.get_target(e).v_pos == b.v_pos) {
            return true;
        }
    }
    return false;
}

// // used to produce small result, for convergence test
// std::vector<Query*> *generate_distance_query(Graph &graph, int number) {
//     auto queries = new std::vector<Query*>();
//     int half = number / 2;
//     int rest = number - half;
//     while (half--) {
//         int a, b, c, d;
//         while (true) {
//             do {
//                 a = rand() % graph.vertex_number();
//             } while (!graph.valid((Vertex){a}));
//             b = random_neighbor((Vertex){a}, graph).v_pos;
//             c = random_neighbor((Vertex){b}, graph).v_pos;
//             d = random_neighbor((Vertex){c}, graph).v_pos;
//             if (has_edge({a}, {c}, graph)) continue;
//             if (has_edge({a}, {d}, graph)) continue;
//             if (has_edge({b}, {d}, graph)) continue;
//             if (graph.valid((Vertex){b}) && a != b) break;
//         };
//         queries->push_back(new DistanceQuery({a}, {b}, graph));
//     }
//     while (rest--) {
//         int a, b;
//         do {
//             a = rand() % graph.vertex_number();
//             b = rand() % graph.vertex_number();
//         } while (a == b);
//         queries->push_back(new DistanceQuery({a}, {b}, graph));
//     }
//     return queries;
// }

// normal queries
std::vector<Query*> *generate_distance_query(Graph &graph, int number) {
    auto queries = new std::vector<Query*>();
    int half = number / 2;
    int rest = number - half;
    while (half--) {
        int a, b;
        while (true) {
            do {
                a = rand() % graph.vertex_number();
            } while (!graph.valid((Vertex){a}));
            b = random_neighbor(random_neighbor((Vertex){a}, graph), graph).v_pos;
            if (has_edge({a}, {b}, graph)) continue;
            if (graph.valid((Vertex){b}) && a != b) break;
        };
        queries->push_back(new DistanceQuery({a}, {b}, graph));
    }
    while (rest--) {
        int a, b;
        do {
            a = rand() % graph.vertex_number();
            b = rand() % graph.vertex_number();
        } while (a == b);
        queries->push_back(new DistanceQuery({a}, {b}, graph));
    }
    return queries;
}

// std::vector<Query*> *generate_influence_query(const Graph &graph) {
//     auto queries = new std::vector<Query*>();
//     int n = 50;
//     if (graph.max_vertex_id() <= 2) {
//         n = 1;
//     } else if (graph.max_vertex_id() <= 5) {
//         n = 2;
//     } else if (graph.max_vertex_id() <= 100) {
//         n = 5;
//     } else if (graph.max_vertex_id() <= 1000) {
//         n = 10;
//     } else if (graph.max_vertex_id() <= 5000) {
//         n = 20;
//     }
//     queries->push_back(new InfluenceQuery(n));
//     return queries;
// }
