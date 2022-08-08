# DATASET=hept
# RUN_FLAG="d=${DATASET} pm=tr s=1000"

DATASET=lastfm
RUN_FLAG="d=${DATASET} pm=wc s=1000"

make run

set -x
./run m=truth $RUN_FLAG
./run m=nmc $RUN_FLAG
./run m=spi i $RUN_FLAG
# ./run m=spi $RUN_FLAG
./run m=spi nso $RUN_FLAG
./run m=recursive $RUN_FLAG
./run m=lps $RUN_FLAG
./run m=rss2 $RUN_FLAG
./run m=pll i $RUN_FLAG
./run m=pll $RUN_FLAG
./run m=dijk $RUN_FLAG
