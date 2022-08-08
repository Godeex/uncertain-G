DATASET=dblp
RUN_FLAG="d=${DATASET} pm=dblp0.05"
make run

set -x
# ./run m=truth $RUN_FLAG
for i in 250 500 750 1000 1250 1500 1750 2000
# for i in 10000
# for i in 3000 4000 5000 6000 7000 8000 9000 10000
# for i in 250 500 750 1000 1250 1500 1750 2000 3000 4000 5000 6000 7000 8000 9000 10000
# for i in 50000 60000 90000 120000 200000
do
    ./run m=nmc $RUN_FLAG s=$i
    ./run m=recursive $RUN_FLAG s=$i
    ./run m=lps $RUN_FLAG s=$i
    ./run m=rss2 $RUN_FLAG s=$i r=50
    # ./run m=rss2 $RUN_FLAG s=$i r=5
done
# ./run m=truth $RUN_FLAG
./run m=nmc s=10000 pm=dblp0.05 d=dblp
