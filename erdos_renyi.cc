#include <stdio.h>
#include <stdlib.h>
#include <time.h>
const int MAX_N = 10;
const int MAX_M = 1000;
int main() {
    srand(time(NULL));
    int n = 12;
    int p = 0.2 * RAND_MAX;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                if (rand() < p) {
                    printf("%d %d\n", i * 3, j * 3);
                }
            }
        }
    }
}
