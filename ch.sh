g++ erdos_renyi.cc
./a.out > datasets/test.txt
make run && ./run q && ./run m=dijk stdout > dijk.temp && ./run m=pll i && ./run m=pll stdout > pll.temp && diff pll.temp dijk.temp
#  make run && ./run q=100 && ./run m=truth stdout > t.temp && ./run m=rss2 stdout > r.temp && diff t.temp r.temp
# make run && ./run q && ./run m=mpll i && ./run m=mpll > aaa && ./run m=dijk > bbb && diff aaa bbb
# make run && ./run q && ./run m=pll o=pc i
# make run && ./run q && ./run m=dijk > a && time ./run m=pll > b && diff a b
# make run && ./run q && time ./run m=dijk > dijk.temp && time ./run m=pll o=d > pll.temp && time ./run m=pll o=io > pll_io.temp
