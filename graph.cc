#include <math.h>
#include "graph.h"

Graph *construct_graph_from_dataset(const Config&config, const char* filename, int &max_id, bool directed, bool has_weight, bool use_vertex_map) {
    // duplicated edges have been removed in DBLP from SNAP
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        logger_error << "Data File " << filename << " Not Exists." << std::endl;
        return NULL;
    }

    const int max_len = 255;
    char line[max_len];
    if (max_id <= 0) {
        while (fgets(line, max_len, fp)) {
            if (line[0] == '#') {
                continue;
            }
            int a, b;
            sscanf(line, "%d%d", &a, &b);
            if (a > max_id) {
                max_id = a;
            }
            if (b > max_id) {
                max_id = b;
            }
        }
        rewind(fp);
    }
    // TODO: move new Graph into methods
    bool count_degree = true, doubly_linked_edge = false;
    // if (config.prob_model == WEIGHTED_CASCADE || (config.method_name == "pll" || config.method_name == "spi") && config.order == BY_DEGREE) {
    //     count_degree = true;
        // TODO: enable doubly_linked_edges for pll path cover to fastly delete edges
    // }
    // if (config.method_name == "recursive" || config.method_name == "rss2" || config.method_name == "rss1") {
    //     doubly_linked_edge = true;
    // }
    struct Graph *graph = new Graph(max_id+1, count_degree, doubly_linked_edge, use_vertex_map);

    while (fgets(line, max_len, fp)) {
        if (line[0] == '#') {
            continue;
        }
        int a, b;
        double weight = 0;
        if (has_weight) {
            sscanf(line, "%d%d%lf", &a, &b, &weight);
        } else {
            sscanf(line, "%d%d", &a, &b);
        }
        // TODO: undirected, directed, weighted
        // printf("%f\n", weight);
        graph->add_edge(a, b, weight);
        if (!directed) graph->add_edge(b, a, weight);
    }

    fclose(fp);
    return graph;
}

void generate_edge_probability(Graph &graph, GraphProbability model) {
    double total_prob = 0;
    int vn = graph.vertex_number();
    if (model == WEIGHTED_CASCADE) {
        for (int i = 0; i < vn; ++i) {
            Vertex v = {i};
            if (graph.valid(v)) {                
                for (Edge e = graph.first_out(v); graph.valid(e); e = graph.next_out(e)) {
                    int out_degree = graph.get_vertex_load(v).out_degree;
                    double prob = out_degree ? 1.0 / out_degree : 0;
                    total_prob += prob;
                    graph.update_edge(e, {prob});
                }
            }
        }
    } else if (model == TRIVALENCY) {
        double probs[] = {0.1, 0.01, 0.001};
        int n = sizeof(probs) / sizeof(probs[0]);
        for (int i = 0; i < vn; ++i) {
            Vertex v = {i};
            if (graph.valid(v)) {
                for (Edge e = graph.first_out(v); graph.valid(e); e = graph.next_out(e)) {
                    double prob = probs[rand() % n];
                    total_prob += prob;
                    graph.update_edge(e, {prob});
                }
            }
        }
    } else if (model == DBLP_D20 || model == DBLP_D5) {
        const int mu = model == DBLP_D20 ? 20 : 5;
        for (int i = 0; i < vn; ++i) {
            Vertex v = {i};
            if (graph.valid(v)) {
                for (Edge e = graph.first_out(v); graph.valid(e); e = graph.next_out(e)) {
                    double w = graph.get_edge_load(e);
                    double prob = 1-exp(-w/mu);
                    total_prob += prob;
                    graph.update_edge(e, {prob});
                }
            }
        }
    }

    logger_info << "avg prob: " << total_prob / graph.edge_number() << std::endl;
}

