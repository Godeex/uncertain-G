#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <chrono>
#include <iomanip>
#include "struct.h"
#include "graph.h"
#include "query.h"
#include "recursive_sampling.h"
#include "pruned_landmark_labeling.h"
#include "lazy_propagation_sampling.h"
#include "shortest_path_dijkstra.h"
#include "modified_pll.h"
#include "spi.h"
#include "naive_monte_carlo.h"
#include "truth.h"
#include "method.h"
#include "logger.h"
#include "config.h"
#include "rss.h"
#include "rss1.h"

std::ostream& operator << (std::ostream& os, const Config& config) {
    logger_info << "Config(";
    logger_info << "dataset=" << config.dataset << ',';
    logger_info << "method=" << config.method_name << ',';
    logger_info << "prob_model=" << to_string(config.prob_model) << ',';
    logger_info << "sample_n=" << config.sample_n << ',';
    logger_info << "index_order=" << to_string(config.order) << ',';
    logger_info << "threshold_r=" << config.threshold_r << ',';
    logger_info << "threshold_hop=" << config.threshold_hop << ',';
    logger_info << "use_struct_opt=" << !config.close_struct_opt << ',';
    logger_info << "tree_bound=" << config.tree_bound << ',';
    logger_info << "rss_r=" << config.rss_r << ',';
    logger_info << ')' << std::endl;
    return os;
}

struct DataConfig {
    int max_id;
    const char *query_filename;
    const char *data_filename;
    bool directed = true;
    bool edge_count = false;
    bool use_vertex_map = true;
};
std::map<std::string, DataConfig> load_config = {{
    "dblp_distance", {
        0,
        "query/distance_query_dblp.txt",
        "datasets/dblp.txt",
        false,
        true,
        true,
    }
}, {
    "test_distance", {
        0,
        "query/test_dis_query.txt",
        "datasets/test.txt",
        true,
        false,
        true,
    }
}, {
    "hept_distance", {
        0,
        "query/distance_query_hept.txt",
        "datasets/hept.txt",
        false,
        false,
        true,
    }
}, {
    "lastfm_distance", {
        0,
        "query/distance_query_lastfm.txt",
        "datasets/lastfm.txt",
        false,
        false,
        true,
    }
}};

struct Bucket {
    int sum = 0, count = 0;
    void add(int x, int c) {
        sum += x;
        count += c;
    }
    double get() {
        return sum / (double) count;
    }
};

struct QueryBucket {
    static const int group_number = 100;
    Bucket buckets[group_number];
    int counter = 0;
    void add(int x, int c) {
        buckets[counter].add(x, c);
        counter++;
        if (counter == group_number) {
            counter = 0;
        }
    }
    void get(double &variance, double &avg) {
        variance = 0;
        avg = 0;
        double res[group_number];
        for (int i = 0; i < group_number; ++i) {
            res[i] = buckets[i].get();
            avg += res[i];
        }
        avg /= group_number;
        for (int i = 0; i < group_number; ++i) {
            variance += (res[i] - avg) * (res[i] - avg);
        }
        variance /= group_number-1;
    }
};

void generate_distance_query(DataConfig config, Graph *graph, int query_number) {
    save_query(config.query_filename, generate_distance_query(*graph, query_number));
}

// void generate_influence_query(RunningConfig config, Graph *graph) {
//     save_query(config.query_filename, generate_influence_query(*graph));
// }

void log_influence_result(std::vector<double> v) {
    for (int i = 0; i < v.size(); ++i) {
        logger_info << "(" << i << "," << v[i] << ") ";
    }
    logger_info << std::endl;
}

// inline std::string index_file(const std::string &method, IndexOrder &order, const std::string &config_name, GraphProbability prob_model) {
    // return std::string("index/")+(method == "pll" ? method + '_' + to_string(order) : method)+"_"+config_name+"_"+to_string(prob_model);
// }

inline std::string method_string(const Config &config) {
    std::string method_string = config.method_name;
    #ifdef O2
        method_string += "_O2";
    #endif
    if (config.method_name == "pll") {
        method_string += std::string("_") + to_string(config.order);
    }
    if (config.method_name == "spi") {
        method_string += std::string("_") + to_string(config.order);
        if (config.threshold_r > 0) method_string += "_r" + std::to_string(config.threshold_r);
        if (config.threshold_hop != 2e9) method_string += "_k" + std::to_string(config.threshold_hop);
        if (config.tree_bound != 2e9) method_string += "_b" + std::to_string(config.tree_bound);
    }
    if (config.method_name == "rss2") {
        method_string += "_r" + std::to_string(config.rss_r);
    }
    return method_string;
}

inline std::string dataset_string(const Config &config) {
    return config.dataset + "_" + to_string(config.prob_model);
}

inline std::string index_file(const Config &config) {
    std::string ms = method_string(config);
    return std::string("index/")+ms+"_" + dataset_string(config) +"_"+to_string(config.prob_model)+"_"+std::to_string(config.sample_n);
}

Method * get_method(const Config &config, Graph &graph) {
    if (config.method_name == "pll") {
        return new PrunedLandmarkLabeling(graph, config.order);
    } else if (config.method_name == "lps") {
        return new LazyPropagationSampling(graph, config.sample_n);
    } else if (config.method_name == "mpll") {
        return new MPLL(graph);
    } else if (config.method_name == "dijk") {
        return new ShortestPathDijkstra(graph);
    } else if (config.method_name == "nmc") {
        return new NaiveMonteCarlo(graph, config.sample_n);
    } else if (config.method_name == "truth") {
        return new BruteForce(graph);
    } else if (config.method_name == "recursive") {
        return new RecursiveSampling(graph, config.sample_n);
    } else if (config.method_name == "rss2") {
        RecursiveStratifiedSampling *method = new RecursiveStratifiedSampling(graph, config.sample_n);
        method->set_rss_r(config.rss_r);
        return method;
    } else if (config.method_name == "spi") {
        ShortestPathIndexing *method = new ShortestPathIndexing(graph, config.sample_n, BY_DEGREE, !config.close_struct_opt, config.threshold_r, config.threshold_hop);
        method->set_tree_bound(config.tree_bound);
        return method;
    } else if (config.method_name == "rss1") {
        return new RecursiveStratifiedSamplingI(graph, config.sample_n);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // logger
    Logger::setLogLevel(logINFO);

    // random
    srand(12345);

    // config variables
    Config config;
    bool do_query = false, do_index = false;
    bool stdout = false;
    bool convergence = false;

    // parse argument and set config variables
    if (argc >= 2) {
        // TODO: argparse: choice and error
        for (int i = 1; i < argc; ++i) {
            if (strncmp("d=", argv[i], 2) == 0) {
                config.dataset = argv[i]+2;
            } else if (strncmp("m=", argv[i], 2) == 0) {
                config.method_name = argv[i]+2;
            } else if (strncmp("pm=", argv[i], 3) == 0) {
                // pm = wc or tr
                std::string prob_model_s = argv[i] + 3;
                if (prob_model_s == "wc") {
                    config.prob_model = WEIGHTED_CASCADE;
                } else if (prob_model_s == "tr") {
                    config.prob_model = TRIVALENCY;
                } else if (prob_model_s == "dblp0.05") {
                    config.prob_model = DBLP_D20;
                } else if (prob_model_s == "dblp0.2") {
                    config.prob_model = DBLP_D5;
                } else {
                    // TODO: error
                }
            } else if (strncmp("s=", argv[i], 2) == 0) {
                config.sample_n = std::stoi(argv[i] + 2);
            } else if (strncmp("q", argv[i], 1) == 0) {
                do_query = true;
                if (strncmp("q=", argv[i], 2) == 0) {
                    config.query_number = std::stoi(argv[i] + 2);
                }
            } else if (0 == strncmp("i", argv[i], 1)) {
                do_index = true;
            } else if (0 == strncmp("o=", argv[i], 2)) {
                std::string order_s = argv[i] + 2;
                for (int io = 0; io < INDEX_ORDER_NUMBER; ++io) {
                    if (to_string((IndexOrder)io) == order_s) {
                        config.order = (IndexOrder)io;
                    }
                }
            } else if (0 == strncmp("tr=", argv[i], 3)) {
                config.threshold_r = std::stod(argv[i]+3);
            } else if (0 == strncmp("tk=", argv[i], 3)) {
                config.threshold_hop = std::stoi(argv[i]+3);
            } else if (0 == strncmp("tb=", argv[i], 3)) {
                config.tree_bound = std::stoi(argv[i]+3);
            } else if (0 == strncmp("nso", argv[i], 3)) {
                config.close_struct_opt = true;
            } else if (0 == strncmp("stdout", argv[i], 6)) {
                stdout = true;
            } else if (0 == strncmp("conv", argv[i], 4)) {
                convergence = true;
            } else if (0 == strncmp("r=", argv[i], 2)) {
                config.rss_r = std::stoi(argv[i] + 2);
            }
        }
    }

    // set log file
    std::string ms = method_string(config);
    if (!config.close_struct_opt && config.method_name == "spi") ms += "_struct_opt";
    std::string log_file = "result/" + dataset_string(config) + "_" + ms + "_" + std::to_string(config.sample_n);
    if (do_index) log_file += ".i";
    if (do_query) log_file += ".q";
    if (convergence) log_file += ".conv";
    if (!do_index && !do_query && !convergence) {
        log_file += ".res";
    }
    if (!stdout) Logger::setFile(log_file);

    // log config
    logger_info << config;

    // load dataset config
    const std::string config_name = config.dataset + "_distance";
    DataConfig &data_config = load_config[config_name];

    // laod graph and generate probability
    Graph *graph = construct_graph_from_dataset(config, data_config.data_filename, data_config.max_id, data_config.directed, data_config.edge_count, data_config.use_vertex_map);
    generate_edge_probability(*graph, config.prob_model);

    // generate query
    if (do_query) {
        logger_info << "generating query file" << std::endl;
        generate_distance_query(data_config, graph, config.query_number);
        return 0;
    }

    Method *method = get_method(config, *graph);

    // generate index
    std::string index_filename = index_file(config);
    if (do_index) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        method->index();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        logger_info << "Index Time : " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " s" << std::endl;
        method->save_index(index_filename.c_str());
        return 0;
    } else {
        // load index from disk
        if (method) method->load_index(index_filename.c_str());
    }

    // open query file
    FILE *fp = fopen(data_config.query_filename, "r");
    if (!fp) {
        logger_error << "Query File " << data_config.query_filename << " Not Found." << std::endl;
        return 0;
    }

    // run method
    if (!convergence) {
        DistanceQuery* q = new DistanceQuery(*graph);
        char s[255];
        while (fgets(s, 255, fp)) {
            q->load(s);
            logger_info << "running distance query " << q->s_id << " to " << q->t_id << std::endl;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            double res = method->query(*q);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            logger_info << "answer query " << q->s_id << " -> " << q->t_id ;
            logger_info << " : " << std::fixed << std::setprecision(15) << res << std::endl;
            logger_info << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;
        }
    } else {
        const int max_query_number = 100;
        std::vector<DistanceQuery> q;
        char s[255];
        while (fgets(s, 255, fp)) {
            DistanceQuery qu(*graph);
            qu.load(s);
            q.push_back(qu);
            if (q.size() >= max_query_number) {
                break;
            }
        }

        const int minimum_sample_n = 1000;

        std::chrono::steady_clock::time_point conv_start = std::chrono::steady_clock::now();
        if (config.method_name == "nmc") {
            const int step = 250;
            QueryBucket *qb = new QueryBucket[max_query_number];
            for (int sample_n = step; ; sample_n += step) {
                // test convergence
                logger_info << "test convergence, sample_n = " << sample_n << std::endl;
                double avg_v = 0, avg_r = 0;
                for (int i = 0; i < q.size(); ++i) {
                    logger_info << "Query: " << q[i].dump() << std::endl;
                    for (int j = 0; j < QueryBucket::group_number; ++j) {
                        logger_info << "group " << j << std::endl;
                        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                        int res = naive_monte_carlo_logging_int(q[i], step, *graph, NULL, true);
                        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                        logger_info << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;
                        logger_info << "Result: " << res << std::endl;
                        qb[i].add(res, step);
                    }
                    double v, r;
                    qb[i].get(v, r);
                    avg_v += v;
                    avg_r += r;
                }
                avg_r /= q.size();
                avg_v /= q.size();
                double ro = avg_v / avg_r;
                logger_info << "avg_r=" << avg_r << ", avg_v=" << avg_v << ", avg_v/avgr=" << ro << std::endl;
                if (ro < 0.0008  && sample_n >= minimum_sample_n) {
                    break;
                }
            }
        } else {
            const int step = 250;
            for (int sample_n = step; ; sample_n += step) {
                config.sample_n = sample_n;
                double avg_v = 0, avg_r = 0;
                logger_info << "test convergence, sample_n = " << sample_n << std::endl;
                Method *method = get_method(config, *graph);
                for (int i = 0; i < q.size(); ++i) {
                    std::vector<double> results;
                    double r = 0, v = 0;
                    logger_info << "Query: " << q[i].dump() << std::endl;
                    for (int j = 0; j < QueryBucket::group_number; ++j) {
                        logger_info << "group " << j << std::endl;
                        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                        double res = method->query(q[i]);
                        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                        logger_info << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;
                        logger_info << "Result: " << res << std::endl;
                        results.push_back(res);
                        r += res;
                    }
                    r /= results.size();
                    for (auto x: results) {
                        v += (x - r) * (x - r);
                    }
                    v /= results.size() - 1;
                    avg_r += r, avg_v += v;
                }
                avg_r /= q.size();
                avg_v /= q.size();
                double ro = avg_v/avg_r;
                logger_info << "avg_r=" << avg_r << ", avg_v=" << avg_v << ", avg_v/avgr=" << ro << std::endl;
                if (ro < 0.0005 && sample_n >= minimum_sample_n) {
                    break;
                }
            }
        }
        std::chrono::steady_clock::time_point conv_end = std::chrono::steady_clock::now();
        logger_info << "Time to convergence : " << std::chrono::duration_cast<std::chrono::milliseconds>(conv_end - conv_start).count() << " ms" << std::endl;
    }

    // for (int prob_model = 0; prob_model < GRAPH_PROBABILITY_NUMBER; ++prob_model) {
    //     logger_info << "Prob Model: " << to_string((GraphProbability) prob_model) << std::endl;
    //     generate_edge_probability(*graph, (GraphProbability) prob_model);

    //     // Lazy Propagation
    //     LazyPropagationSampling lps(*graph);
    //     // Load Index
    //     std::string filename = index_file("pll", config_name, (GraphProbability)prob_model);
    //     pll.load_index(filename.c_str());

    //     char s[255];
    //     int cnt = 0;
        
    //     while (fgets(s, 255, fp)) {
    //         DistanceQuery* q = new DistanceQuery();
    //         q->load(s, graph);
    //         logger_info << "distance query " << q->s->id << " to " << q->t->id << std::endl;
    //         if (graph->edge_number() < 24) {
    //             logger_info << "truth: " << brute_force(*q, *graph) << std::endl;    
    //         }
    //         logger_info << "naive monte carlo " << naive_monte_carlo(*q, 10000, *graph) << std::endl;
    //         logger_info << "recursive sampling " << recursive_sampling(*graph, *q, 10000) << std::endl; 
    //         logger_info << "recursive stratified sampling " << recursive_stratified_sampling_II(*graph, *q, 10000) << std::endl;
    //         logger_info << "lazy propagation sampling " << lps.query(*q, 10000) << std::endl;
    //         logger_info << "shortest path " << shortest_path_dijk_fib(*q, *graph) << std::endl;
    //         logger_info << "shortest path by PrunedLandmarkLabeling " << pll.query(q->s->id, q->t->id) << std::endl;
    //         logger_info << std::endl;
    //     }
    //     rewind(fp);
    // }

    // if (config_name.find("influence") != std::string::npos)
    // for (int prob_model = 0; prob_model < GRAPH_PROBABILITY_NUMBER; ++prob_model) {
    //     logger_info << "Prob Model: " << to_string((GraphProbability) prob_model) << std::endl;
    //     generate_edge_probability(*graph, (GraphProbability) prob_model);
    //     char s[255];
    //     int cnt = 0;
    //     while (fgets(s, 255, fp)) {
    //         InfluenceQuery* q = new InfluenceQuery();
    //         q->load(s);
    //         logger_info << "influence query " << q->dump() << std::endl;
    //         if (graph->edge_number() < 24) {
    //             logger_info << "truth: ";
    //             log_influence_result(brute_force(*q, *graph));
    //             logger_info << "vanilla greedy ";
    //             log_influence_result(vanilla_greedy(*q, 10000, *graph));
    //         }
    //         // logger_info << "celf\n";
    //         // log_influence_result(cost_effective_lazy_forward(*q, 10000, *graph));
    //         logger_info << "mia\n";
    //         std::string filename = index_file("mia", config_name, (GraphProbability)prob_model);
    //         // mia.load_index(filename.c_str());
    //         log_influence_result(mia.run(*q));
    //         // mia.select_by_coverage(*q);
    //         // logger_info << "recursive sampling " << recursive_sampling(*graph, *q, 10000) << std::endl; 
    //         // logger_info << "recursive stratified sampling " << recursive_stratified_sampling_II(*graph, *q, 10000) << std::endl;
    //         // logger_info << "shortest path " << shortest_path_dijk_fib(*q, *graph) << std::endl;
    //         logger_info << std::endl;
    //     }
    //     rewind(fp);
    // }

    delete graph;
    fclose(fp);
    return 0;
}