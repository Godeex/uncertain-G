#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include "struct.h"

enum QueryType {
    NONE,
    DISTANCE,
    DISTANCE_RANDOM,
    INFLUENCE,
};

struct Query {
    QueryType query_type;
    Query();
    Query(QueryType query_type);
    virtual const char *dump() { return ""; };
    virtual void load() {};
};

struct DistanceQuery: Query {
    Graph &graph;
    Vertex s, t;
    int s_id, t_id;
    DistanceQuery(Graph &graph);
    DistanceQuery(Vertex s, Vertex t, Graph &graph);
    const char *dump();
    void load(const char *s);
};

// struct InfluenceQuery: Query {
//     int max_size;
//     InfluenceQuery();
//     InfluenceQuery(int max_size);
//     const char *dump();
//     void load(const char *s);
// };

void save_query(const char *filename, std::vector<Query*> *queries);
std::vector<Query*> *generate_distance_query(Graph &graph, int number);
// std::vector<Query*> *generate_influence_query(const Graph &graph);

#endif