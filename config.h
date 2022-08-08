#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "logger.h"

enum IndexOrder {
    BY_INPUT_ORDER,
    RANDOM,
    BY_DEGREE,
    PATH_COVER,
    INDEX_ORDER_NUMBER,
};

struct Config {
    std::string dataset = "test";
    std::string method_name = "nmc";
    int sample_n = 10000;
    GraphProbability prob_model = WEIGHTED_CASCADE;
    int query_number = 1000;
    IndexOrder order = BY_DEGREE;
    double threshold_r = 0;
    int threshold_hop = 2e9;
    bool close_struct_opt = false;
    int tree_bound = 2e9;
    int rss_r = 50;
};

std::ostream& operator << (std::ostream& os, const Config& config);

#endif