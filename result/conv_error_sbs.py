# dataset, pm = 'lastfm', 'Weighted Cascade'
dataset, pm = 'dblp', 'dblp0.05'
# dataset, pm = 'hept', 'Trivalency'

max_len = 40
methods = ['lps', 'nmc', 'rss2', 'recursive']

def parse_answer_line(line):
    tokens = line.split()
    result = float(tokens[-1])
    s = int(tokens[2])
    t = int(tokens[4])
    return s, t, result

conv_f_name = 'dblp_dblp0.05_nmc_O2_10000.res'
count = 0
conv_r = 0
ans = {}

def key(s, t):
    return (s << 25) | t

try:
    with open(conv_f_name) as file:
        for line in file:
            if line.startswith('answer query'):
                s, t, result = parse_answer_line(line)
                ans[key(s, t)] = result
except (FileNotFoundError):
    print("conv file not found")
    pass

abs_error = {method: [] for method in methods}
rel_error = {method: [] for method in methods}
x = []
for i in range(max_len):
    abs_row = []
    rel_row = []
    sample_n = (i+1)*250
    for method in methods:
        f_name = dataset + '_' + pm + '_' + method + '_O2_' + ('r50_' if method == 'rss2' else '') + str(sample_n) + '.res'
        abs_e = 0
        rel_e = 0
        count = 0
        print(sample_n, method)
        try:
            with open(f_name) as file:
                for line in file:
                    if line.startswith('answer query'):
                        s, t, result = parse_answer_line(line)
                        count += 1
                        abs_e += abs(ans[key(s, t)] - result)
                        if ans[key(s, t)] > 1e-8:
                            rel_e += abs(ans[key(s, t)] - result) / ans[key(s, t)]

        except (FileNotFoundError):
            continue
        if count == 0:
            continue
        rel_e /= count
        abs_e /= count
        abs_row.append(abs_e)
        rel_row.append(rel_e)
    if len(abs_row) == len(methods) and len(rel_row) == len(methods):
        for a, r, method in zip(abs_row, rel_row, methods):
            abs_error[method].append(a)
            rel_error[method].append(r)
        x.append(sample_n)
    # print(method)

import matplotlib.pyplot as plt
plot1 = plt.figure(1)
for method in rel_error:
    r = rel_error[method]
    plt.plot(x, r, label = method)
    print(x, r)

# plt.yticks([i*0.02+0.12 for i in range(5)])
plt.title('convergence-rel-error-' + dataset)
plt.xlabel('sample_n')
plt.ylabel('rel error')
plt.legend()
# plt.axhline(y=0.01, linestyle='--')
plt.savefig(dataset + '_conv_rel_error.png', dpi=600)

plot2 = plt.figure(2)
for method in abs_error:
    r = abs_error[method]
    plt.plot(x, r, label = method)

plt.title('convergence-abs-error-' + dataset)
plt.xlabel('sample_n')
plt.ylabel('abs error')
plt.legend()
plt.axhline(y=0.01, linestyle='--')
plt.savefig(dataset + '_conv_abs_error.png', dpi=600)

