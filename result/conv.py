# dataset = 'lastfm'
# dataset = 'test'
dataset = 'hept'
pm = 'Trivalency'
ro_series = {}
max_len = 0

for method in ['lps', 'nmc', 'rss2_r50', 'recursive']:
    f_name = dataset + '_' + pm + '_' + method + '_10000.conv'
    res = []
    ro = []
    with open(f_name) as f:
        for line in f:
            if 'avg_v/avgr' in line:
                by_eq = line.split('=')
                ro.append(float(by_eq[-1]))
                res.append(float(by_eq[1].split(',')[0]))
    print(method, res, ro)
    if len(ro) > max_len:
        max_len = len(ro)
    ro_series[method] = ro

import matplotlib.pyplot as plt
x = [(i+1) * 250 for i in range(max_len)]
for method in ro_series:
    ro = ro_series[method]
    while len(ro) < max_len:
        ro.append(None)
    plt.plot(x, ro, label = method)

plt.axhline(y=0.001, linestyle='--')

plt.title('convergence-' + dataset)
plt.xlabel('sample_n')
plt.ylabel('ro')
plt.legend()
plt.savefig(dataset + '_conv.png', dpi=600)
