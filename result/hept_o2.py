def get_method(m_string):
    return m_string.split('_')[2]
# TODO: mix
def get(m_string, b):
    for s in m_string.split('_'):
        if s.startswith(b):
            return s[1:]
    return -1
def get_r(m_string):
    return float(get(m_string, 'r'))
def get_k(m_string):
    return int(get(m_string, 'k'))
def get_b(m_string):
    return int(get(m_string, 'b'))

# hept files
sample_n = "1000"
suffix = sample_n + ".res"
std_file = "hept_Trivalency_nmc_1000000.res"
cmp_files = ["hept_Trivalency_recursive_O2_" + suffix,
    # "hept_Trivalency_spi_d_struct_opt_" + suffix,
    "hept_Trivalency_spi_O2_d_" + suffix,
    "hept_Trivalency_nmc_O2_" + suffix,
    "hept_Trivalency_lps_O2_" + suffix,
    "hept_Trivalency_rss2_O2_r50_" + suffix,
    "hept_Trivalency_pll_O2_d_" + suffix,
    "hept_Trivalency_dijk_O2_"  + suffix,
    ]

r_list = [0.02, 0.03, 0.04, 0.001, 0.002, 0.005, 0.05, 0.01, 0.3, 0.2, 0.1]
r_list.sort()
for r in r_list:
    # cmp_files.append("hept_Trivalency_spi_d_r%0.6f_struct_opt_10000.res" % r)
    cmp_files.append(("hept_Trivalency_spi_O2_d_r%0.6f_" + suffix) % r)
for k in [1, 2, 3, 4]:
    # cmp_files.append("hept_Trivalency_spi_d_k%d_struct_opt_10000.res" % k)
    cmp_files.append(("hept_Trivalency_spi_O2_d_k%d_" + suffix) % k)

b_list = [100, 200, 300, 400, 500, 10, 50, 600, 700, 800, 900]
b_list.sort()
for b in b_list:
    cmp_files.append(("hept_Trivalency_spi_d_b%d_" + suffix) % b)

print(cmp_files)
# single file
# cmp_files = ["hept_Trivalency_spi_d_2000.res"]

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
eps = 1e-3
method_results = {}

err_by_r = {}
err_by_k = {}
err_by_b = {}

rel_by_r = {}
rel_by_k = {}
rel_by_b = {}

time_by_r = {}
time_by_k = {}
time_by_b = {}

index_size_by_r = {}
index_size_by_b = {}
index_size_by_k = {}

index_time_by_r = {}
index_time_by_b = {}
index_time_by_k = {}


method_res = {}
method_time = {}
method_rel_err = {}

for cmp_file in cmp_files:
    abs_error = .0
    rel_error = .0
    count = 0
    time = 0
    method = get_method(cmp_file)
    print()
    print("Result File", cmp_file)
    with open(cmp_file) as file:
        # print(cmp_file)
        flag = 0
        for line in file:
            if flag == 0:
                if line.startswith('answer query'):
                    s, t, result = parse_answer_line(line)
                    if key(s, t) in ans:
                        # print(s, t, "ans", ans[key(s, t)], "result", result)
                        abs_error += abs(ans[key(s, t)] - result)
                        if ans[key(s, t)] > eps:
                            print(ans[key(s, t)])
                            rel_error += abs(ans[key(s, t)] - result) / ans[key(s, t)]
                        count += 1
                    flag = 1
            elif flag == 1:
                time += int(line.split()[2])
                flag = 0
    
    index_size = 0
    index_time = 0
    if method == "spi":
        index_file = cmp_file
        if 'struct_opt' not in index_file:
            index_file = (index_file[:-4] + ".i").split('_')
            index_file.insert(-1, "struct_opt")
            index_file = '_'.join(index_file)
        with open(index_file) as file:
            for line in file:
                if line.startswith('total size'):
                    index_size = int(line.split()[2])
                if line.startswith('Index Time'):
                    index_time = int(line.split()[3])

    abs_error /= count
    rel_error / count

    if method == "spi":
        b = get_b(cmp_file)
        k = get_k(cmp_file)
        r = get_r(cmp_file)
        if b != -1:
            err_by_b[b] = abs_error
            rel_by_b[b] = rel_error
            time_by_b[b] = time
            index_size_by_b[b] = index_size
            index_time_by_b[b] = index_time
            continue 
        if k != -1:
            err_by_k[k] = abs_error
            rel_by_k[k] = rel_error
            time_by_k[k] = time
            index_size_by_k[k] = index_size
            index_time_by_k[k] = index_time
            continue 
        if r != -1:
            err_by_r[r] = abs_error
            rel_by_r[r] = rel_error
            time_by_r[r] = time
            index_size_by_r[r] = index_size
            index_time_by_r[r] = index_time
            continue
    method_res[method] = abs_error
    method_rel_err[method] = rel_error
    method_time[method] = time

    print("count %d avg abs_error %f avg rel_error %f" % (count, abs_error, rel_error))
    print("avg time: %f ms" % (time / float(count)))

from matplotlib.pyplot import cm
import numpy as np
import matplotlib.pyplot as plt

dataset = "hept"
def hlines(name, metrics, header):
    if header == 'abs_err':
        color = cm.rainbow(np.linspace(0, 1, len(method_res)))
        for col, method in zip(color, method_res):
            plt.axhline(y=method_res[method], linestyle='--', label=method, color=col)

    if header == 'time':
        color = cm.rainbow(np.linspace(0, 1, len(method_time)))
        for col, method in zip(color, method_time):
            # if method != 'dijk':
            plt.axhline(y=method_time[method], linestyle='--', label=method, color=col)
    
    if header == 'rel_err':
        color = cm.rainbow(np.linspace(0, 1, len(method_rel_err)))
        for col, method in zip(color, method_rel_err):
            plt.axhline(y=method_rel_err[method], linestyle='--', label=method, color=col)
    # plt.axhline(y=conv, linestyle='--', label=method)

def draw(name, metrics, header):
    plt.figure(name+header)
    plt.plot(metrics.keys(), metrics.values(), label = method)
    hlines(name, metrics, header)
    plt.title(header + "_" + dataset)
    plt.xlabel(name)
    plt.ylabel(header)
    plt.legend()
    plt.savefig(dataset + '_' + header + '_' + name + '_O2.png', dpi=600)

draw("b", err_by_b, 'abs_err')
draw("k", err_by_k, 'abs_err')
draw("r", err_by_r, 'abs_err')

draw("b", rel_by_b, 'rel_err')
draw("k", rel_by_k, 'rel_err')
draw("r", rel_by_r, 'rel_err')

draw("b", time_by_b, 'time')
draw("k", time_by_k, 'time')
draw("r", time_by_r, 'time')

draw("b", index_time_by_b, 'index_time')
draw("k", index_time_by_k, 'index_time')
draw("r", index_time_by_r, 'index_time')

draw("b", index_size_by_b, 'index_size')
draw("k", index_size_by_k, 'index_size')
draw("r", index_size_by_r, 'index_size')

