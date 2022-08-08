#ifndef MY_SEARCH
#define MY_SEARCH

#include <string.h>
#include <vector>
#include "struct.h"
#include "fib.h"
struct BreadthFirstSearch {
    Graph &graph;
    Vertex *q;
    int *visited;
    int visit_flag, op, cl, cursor;
    int direction;

    const static int MAX_FLAG = 2e9;

    BreadthFirstSearch(Graph &graph);
    ~BreadthFirstSearch();

    void reset(const std::vector<Vertex> &initial_set, int d=0); // 0 FORWARD, otherwise BACKWRAD

    Vertex next();
};


struct DijkstraBreadthFirstSearch {
    Graph &graph;
    int *heap_nodes;
    Fib<Distance> heap;
    int direction;
    int *visited;
    int visit_flag;

    const static int MAX_FLAG = 2e9;

    DijkstraBreadthFirstSearch(Graph &graph);
    ~DijkstraBreadthFirstSearch();
    void reset(const std::vector<Vertex> &initial_set, int d=0);

    Distance next(bool close = false); // close = true: do not expand from current node
    Distance front() const;
};
#endif