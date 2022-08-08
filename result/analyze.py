dataset = "test"
std_file = dataset + "_nmc_1000000.res"
cmp_files = [dataset + "_dijk_10000.res", dataset + "_nmc_10000.res", dataset + "_spi_d_10000.res", dataset + "_pll_d_10000.res"
, dataset+ "_lps_struct_opt_10000.res", dataset + "_rss2_struct_opt_10000.res", dataset+"_recursive_10000.res"]

for r in [0.2, 0.1, 0.05, 0.01, 0.001, 0.0001]:
    cmp_files.append("test_spi_d_r%0.6f_struct_opt_10000.res" % r)
    cmp_files.append("test_spi_d_r%0.6f_10000.res" % r)

for k in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]:
    cmp_files.append("test_spi_d_k%d_struct_opt_10000.res" % k)
    cmp_files.append("test_spi_d_k%d_10000.res" % k)


# test files
std_file = "test_Weighted Cascade_truth_10000.res"
cmp_files = ["test_Weighted Cascade_recursive_10000.res",
    "test_Weighted Cascade_spi_d_struct_opt_10000.res",
    "test_Weighted Cascade_nmc_10000.res",
    "test_Weighted Cascade_lps_10000.res",
    "test_Weighted Cascade_rss2_10000.res",
    "test_Weighted Cascade_pll_d_10000.res",
    "test_Weighted Cascade_dijk_10000.res",]

# lastfm files
sample_n = "2000"
suffix = sample_n + ".res"
std_file = "lastfm_Weighted Cascade_nmc_1000000.res"
cmp_files = ["lastfm_Weighted Cascade_recursive_" + suffix,
    "lastfm_Weighted Cascade_spi_d_struct_opt_" + suffix,
    "lastfm_Weighted Cascade_spi_d_" + suffix,
    "lastfm_Weighted Cascade_nmc_" + suffix,
    "lastfm_Weighted Cascade_lps_" + suffix,
    "lastfm_Weighted Cascade_rss2_" + suffix,
    # "lastfm_Weighted Cascade_pll_d_" + suffix,
    "lastfm_Weighted Cascade_dijk_10000.res",]
for r in [0.3, 0.2, 0.1, 0.05, 0.01]:
    # cmp_files.append("lastfm_Weighted Cascade_spi_d_r%0.6f_struct_opt_10000.res" % r)
    cmp_files.append(("lastfm_Weighted Cascade_spi_d_r%0.6f_" + suffix) % r)
for k in [1, 2, 3, 4]:
    # cmp_files.append("lastfm_Weighted Cascade_spi_d_k%d_struct_opt_10000.res" % k)
    cmp_files.append(("lastfm_Weighted Cascade_spi_d_k%d_" + suffix) % k)
for b in [100, 200, 300, 400, 500, 1000, 2000, 5000]:
    cmp_files.append(("lastfm_Weighted Cascade_spi_d_b%d_" + suffix) % b)

# single file
# cmp_files = ["lastfm_Weighted Cascade_spi_d_2000.res"]

# dblp
sample_n = "1000"
suffix = sample_n + ".res"
std_file = "dblp_dblp0.05_nmc_10000.res"
cmp_files = ["dblp_dblp0.05_recursive_" + suffix,
    # "lastfm_Weighted Cascade_spi_d_struct_opt_" + suffix,
    # "lastfm_Weighted Cascade_spi_d_" + suffix,
    "dblp_dblp0.05_nmc_" + suffix,
    "dblp_dblp0.05_lps_" + suffix,
    "dblp_dblp0.05_rss2_" + suffix,
    "dblp_dblp0.05_dijk_" + suffix]
    # "lastfm_Weighted Cascade_pll_d_" + suffix,

def parse_answer_line(line):
    tokens = line.split()
    result = float(tokens[-1])
    s = int(tokens[2])
    t = int(tokens[4])
    return s, t, result

def key(s, t):
    return (s << 25) | t

ans = {}
with open(std_file) as file:
    for line in file:
        if line.startswith('answer query'):
            s, t, result = parse_answer_line(line)
            ans[key(s, t)] = result

import math
eps = 1e-6
for cmp_file in cmp_files:
    abs_error = .0
    rel_error = .0
    count = 0
    time = 0
    with open(cmp_file) as file:
        flag = 0
        for line in file:
            if flag == 0:
                if line.startswith('answer query'):
                    s, t, result = parse_answer_line(line)
                    if key(s, t) in ans:
                        # print(s, t, "ans", ans[key(s, t)], "result", result)
                        abs_error += abs(ans[key(s, t)] - result)
                        if result > eps:
                            rel_error += abs(ans[key(s, t)] - result) / result
                        count += 1
                    flag = 1
            elif flag == 1:
                time += int(line.split()[2])
                flag = 0
    print()
    print("Result File", cmp_file)
    print("count %d avg abs_error %f avg rel_error %f" % (count, abs_error / count, rel_error / count))
    print("avg time: %f ms" % (time / float(count)))