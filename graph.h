#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "logger.h"
#include "config.h"

Graph *get_dblp_graph();
Graph *construct_graph_from_dataset(const Config&config, const char* filename, int &max_id, bool directed, bool count_edge, bool use_vertex_map);
void  generate_edge_probability(Graph &graph, GraphProbability model);

#endif