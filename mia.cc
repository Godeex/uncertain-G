#include <string.h>
#include <unordered_set>
#include "struct.h"
#include "query.h"
#include "fib.h"

static int *cov;
static bool cmp(int a, int b) {
    return cov[a] > cov[b];
}

struct MaximumInfluenceArborescene {
    double p_threshold;
    Graph **tree_index;
    Graph *graph;

    MaximumInfluenceArborescene(Graph *graph, double p_threshold = 1e-3): p_threshold(p_threshold),
        graph(graph), tree_index(NULL) {}
    void delete_index() {
        if (tree_index) {
            for (int i = 0; i < graph->size(); ++i) {
                delete tree_index[i];
            }
            delete []tree_index;
            tree_index = NULL;
        }
    }
    ~MaximumInfluenceArborescene() {delete_index();}

    void save_tree(Vertex *v, FILE *fp) {
        if (v && v->edges.size() > 0) {
            fprintf(fp, "%d", v->id);
            for (auto e: v->edges) {
                fprintf(fp, " %d", e->vertex->id);
            }
            fprintf(fp, "\n");
            for (auto e: v->edges) {
                save_tree(e->vertex, fp);
            }
        }
    }

    void save_index(const char *filename) {
        FILE *fp = fopen(filename, "w");
        for (int i = 0; i < graph->size(); ++i) {
            if (tree_index[i]) {
                fprintf(fp, "root:%d\n", i);
                save_tree(tree_index[i]->get_vertex(i), fp);
            }
        }
        fclose(fp);
    }

    void index() {
        Fib<Distance>::Node **heap_nodes = new Fib<Distance>::Node*[graph->size()];
        Edge**parent_edge = new Edge*[graph->size()];

        delete_index();
        tree_index = new Graph*[graph->size()]();
        for (auto v: *graph->get_vertices()) {
            if (v) {
                memset(heap_nodes, 0, sizeof(Fib<Distance>::Node*) * graph->size());
                // shortest path tree rooted at v
                Fib<Distance> heap;
                Graph *mia = new Graph(graph->size());
                int s = v->id;
                mia->new_vertex(s);
                heap_nodes[s] = heap.insert({s, 1.0});
                // single source shortest path
                double result = 0;
                while (!heap.empty()) {
                    Distance disx = heap.root->value;
                    heap.delete_min();
                    Vertex* x = graph->get_vertex(disx.node);
                    if (x != v) {
                        Edge* edge = parent_edge[x->id];
                        // reverse edge direction
                        logger_debug << "index " << s << " add edge " << edge->vertex->id << " " << x->id << std::endl; 
                        mia->add_edge(edge->vertex->id, x->id, edge->p);
                    }
                    for (auto edge: x->in_edges) {
                        auto y = edge->source;
                        double p = disx.distance * edge->p;
                        if (p < p_threshold) {
                            continue;
                        }
                        logger_debug << "index " << s << " explore " << x->id << " " <<  y->id << std::endl; 
                        if (!heap_nodes[y->id]) {
                            heap_nodes[y->id] = heap.insert({y->id, p});
                            parent_edge[y->id] = edge;
                        } else if (heap_nodes[y->id]->value.distance < p) {
                            heap.decreaseKey(heap_nodes[y->id], {y->id, p});
                            parent_edge[y->id] = edge;
                        }
                    }
                }

                mia->compact();
                tree_index[v->id] = mia;
            }
        }

        delete []heap_nodes;
        delete []parent_edge;
    }

    void load_index(const char *filename) {
        // TODO: load from disk.
        index();
    }

    double prob(Vertex *v, const std::unordered_set<int> &seeds) {
        if (seeds.find(v->id) != seeds.end()) {
            return 1;
        }
        double result = 1;
        for (auto e: v->edges) {
            result *= 1 - e->p * prob(e->vertex, seeds);
        }
        return 1-result;
    }

    double query(const std::unordered_set<int> &seeds) {
        double result = 0;
        for (int i = 0; i < graph->size(); ++i) {
            if (tree_index[i]) {
                result += prob(tree_index[i]->get_vertex(i), seeds);
            }
        }
        return result;
    }

    std::vector<double> run(const InfluenceQuery &q) {
        // TODO: optimize like celf
        std::vector<double> results({0});
        std::unordered_set<int> seeds;
        double max_influence = 0;
        for (int round = 0; round < q.max_size; ++round) {
            int best_vertex = -1;
            for (int i = 0; i < graph->size(); ++i) {
                if (graph->get_vertex(i) && seeds.find(i) == seeds.end()) {
                    seeds.insert(i);
                    double influence = query(seeds);
                    if (influence > max_influence) {
                        best_vertex = i;
                        max_influence = influence;
                    }
                    seeds.erase(i);
                }
            }
            if (best_vertex != -1) {
                logger_info << "round " << round << " choose " << best_vertex << std::endl;
                seeds.insert(best_vertex);
            }
            results.push_back(max_influence);
        }
        return results;
    }

    void cal_cov(Vertex *v, int *cov) {
        cov[v->id]++; // or prob?
        for (auto e: v->edges) {
            cal_cov(e->vertex, cov);
        }
    }
    
    void select_by_coverage(const InfluenceQuery &q) {
        delete []cov;
        cov = new int[graph->size()]();
        int *pos = new int[graph->size()]();
        for (int i = 0; i < graph->size(); ++i) {
            pos[i] = i;
            if (tree_index[i]) {
                cal_cov(tree_index[i]->get_vertex(i), cov);
            }
        }
        std::sort(pos, pos+graph->size(), cmp);
        std::unordered_set<int> seeds;
        for (int i = 0; i < q.max_size; ++i) {
            // logger_info << pos[i] << std::endl;
            seeds.insert(pos[i]);
            double res = query(seeds);
            logger_info << "select " << pos[i] << " result " << res << std::endl;
        }
        delete []pos;
    }
};