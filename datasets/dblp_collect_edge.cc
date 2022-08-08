#include <stdio.h>
#include <utility>
#include <map>
using namespace std;

#define p (24)
#define full (16777215)
#define key(a,b) ((long long)a<<p|b)
#define first(k) (k>>p)
#define second(k) (k&full)

map<long long, int> edge_count;

int main() {
    FILE *fp = fopen("dblp_multiple_edge.txt", "r");
    FILE *wr = fopen("dblp.txt", "w");
    int a, b;
    int max_id = 0;
    
    while (fscanf(fp, "%d%d", &a, &b) == 2) {
        if (a > b) {
            swap(a, b);
        }
        if (max_id < b) {
            max_id = b;
        }
        auto it = edge_count.find(key(a, b));
        if (it != edge_count.end()) {
            it->second++;
        } else {
            edge_count[key(a, b)] = 1;
        }
    }
    for (auto it = edge_count.begin(); it != edge_count.end(); it++) {
        long long k = it->first;
        fprintf(wr, "%d %d %d\n", first(k), second(k), it->second);
    }
    printf("%d\n", max_id);
    fclose(fp);
    fclose(wr);
}