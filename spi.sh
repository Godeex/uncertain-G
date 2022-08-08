DATASET=lastfm
SAMPLE_N=1000
RUN_FLAG="d=${DATASET} s=${SAMPLE_N} pm=wc"

# for r in 0.3 0.2 0.1 0.05 0.01
# for r in 0.02 0.03 0.04 0.001 0.002 0.005
for r in 0.3 0.2 0.1 0.05 0.01 0.02 0.03 0.04 0.001 0.002 0.005
do
    echo "r=$r"
    ./run m=spi i tr=$r $RUN_FLAG
    ./run m=spi nso tr=$r $RUN_FLAG
    # ./run m=spi tr=$r $RUN_FLAG
done

set -x
for k in 1 2 3 4
do
    echo "k=$k"
    ./run m=spi i tk=$k $RUN_FLAG
    ./run m=spi nso tk=$k $RUN_FLAG
    # ./run m=spi tk=$k $RUN_FLAG
done


# for b in 100 200 300 400 500 1000 2000 5000
# for b in 10 50 600 700 800 900
for b in 10 50 600 700 800 900 100 200 300 400 500 1000 2000 5000
do
    echo "b=$b"
    ./run m=spi i tb=$b $RUN_FLAG
    ./run m=spi nso tb=$b $RUN_FLAG
    # ./run m=spi tb=$b $RUN_FLAG
done


# ./run m=spi i tr=0.2 $RUN_FLAG
# ./run m=spi i tr=0.1 $RUN_FLAG
# ./run m=spi i tr=0.05 $RUN_FLAG
# ./run m=spi i tr=0.01 $RUN_FLAG
# ./run m=spi i tr=0.001 $RUN_FLAG
# ./run m=spi i tr=0.0001 $RUN_FLAG
# ./run m=spi i tk=1 $RUN_FLAG
# ./run m=spi i tk=2 $RUN_FLAG
# ./run m=spi i tk=3 $RUN_FLAG
# ./run m=spi i tk=4 $RUN_FLAG
# ./run m=spi i tk=5 $RUN_FLAG
# ./run m=spi i tk=6 $RUN_FLAG
# ./run m=spi i tk=7 $RUN_FLAG
# ./run m=spi i tk=8 $RUN_FLAG
# ./run m=spi i tk=9 $RUN_FLAG
# ./run m=spi i tk=10 $RUN_FLAG


# ./run m=spi tr=0.2 nso $RUN_FLAG
# ./run m=spi tr=0.1 nso $RUN_FLAG
# ./run m=spi tr=0.05 nso $RUN_FLAG
# ./run m=spi tr=0.01 nso $RUN_FLAG
# ./run m=spi tr=0.001 nso $RUN_FLAG
# ./run m=spi tr=0.0001 nso $RUN_FLAG
# ./run m=spi tk=1 nso $RUN_FLAG
# ./run m=spi tk=2 nso $RUN_FLAG
# ./run m=spi tk=3 nso $RUN_FLAG
# ./run m=spi tk=4 nso $RUN_FLAG
# ./run m=spi tk=5 nso $RUN_FLAG
# ./run m=spi tk=6 nso $RUN_FLAG
# ./run m=spi tk=7 nso $RUN_FLAG
# ./run m=spi tk=8 nso $RUN_FLAG
# ./run m=spi tk=9 nso $RUN_FLAG
# ./run m=spi tk=10 nso $RUN_FLAG

# ./run m=spi tr=0.2
# ./run m=spi tr=0.1
# ./run m=spi tr=0.05
# ./run m=spi tr=0.01
# ./run m=spi tr=0.001
# ./run m=spi tr=0.0001
# ./run m=spi tk=1
# ./run m=spi tk=2
# ./run m=spi tk=3
# ./run m=spi tk=4
# ./run m=spi tk=5
# ./run m=spi tk=6
# ./run m=spi tk=7
# ./run m=spi tk=8
# ./run m=spi tk=9
# ./run m=spi tk=10
