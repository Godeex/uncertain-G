# dataset, pm = 'lastfm', 'Weighted Cascade'
dataset, pm = 'dblp', 'dblp0.05'
# dataset, pm = 'hept', 'Trivalency'
# dataset, pm = 'test', 'Weighted Cascade'

max_len = 8

def parse_answer_line(line):
    tokens = line.split()
    result = float(tokens[-1])
    s = int(tokens[2])
    t = int(tokens[4])
    return s, t, result

methods = ['lps', 'nmc', 'rss2', 'recursive']
# methods = ['nmc']
res = {method: [] for method in methods}
x = []
for i in range(max_len):
    row = []
    sample_n = (i+1)*250
    for method in methods:
        f_name = dataset + '_' + pm + '_' + method + '_O2_' + ('r50_' if method == 'rss2' else '') + str(sample_n) + '.res'
        avg = 0
        count = 0
        print(sample_n, method)
        try:
            with open(f_name) as file:
                for line in file:
                    if line.startswith('answer query'):
                        s, t, result = parse_answer_line(line)
                        avg += result
                        count += 1
        except (FileNotFoundError):
            print(f_name, " Not found")
            continue
        if count == 0:
            continue
        avg /= count
        row.append(avg)
    if len(row) == len(methods):
        for r, method in zip(row, methods):
            res[method].append(r)
        x.append(sample_n)
    print(method)
    print(res)

import matplotlib.pyplot as plt
for method in res:
    r = res[method]
    plt.plot(x, r, label = method)

# conv_f_name = 'lastfm_Weighted Cascade_nmc_1000000.res'
# conv_f_name = 'test_Weighted Cascade_truth_10000.res'
conv_f_name = 'dblp_dblp0.05_nmc_O2_10000.res'
count = 0
conv_r = 0
try:
    with open(conv_f_name) as file:
        for line in file:
            if line.startswith('answer query'):
                s, t, result = parse_answer_line(line)
                conv_r += result
                count += 1
except (FileNotFoundError):
    pass
if count > 0:
    conv_r /= count
    plt.axhline(y=conv_r, linestyle='--')
# plt.axhline(y=0.205, linestyle='--')

# plt.yticks([i*0.02+0.12 for i in range(5)])
plt.title('convergence-step-by-step-' + dataset)
plt.xlabel('sample_n')
plt.ylabel('avg reliability')
plt.legend()
plt.savefig(dataset + '_conv_sbs.png', dpi=600)
