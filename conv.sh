DATASET=hept
RUN_FLAG="d=${DATASET} pm=tr conv"
make run

set -x
# ./run m=truth $RUN_FLAG
./run m=nmc $RUN_FLAG
./run m=recursive $RUN_FLAG
./run m=lps $RUN_FLAG
./run m=rss2 $RUN_FLAG
