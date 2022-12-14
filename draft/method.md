## 索引构建
从每个点出发构建索引。先用最短路构建骨架网络，然后只在遍历到的点中考虑索引。  
有两个具体方式:  
1. 跑完最短路，砍掉超出阈值的点，把边全部加进去得到子图，子图中跑花销稍大的准确的可达概率估计。
2. 上一个方式的问题在于取子图的依据，直接用最短路作为可达概率的估计。可以一边跑最短路，一边用现有可达概率的估计，

另外，也可以考虑不砍点，因为 pruned landmark labeling 本身就是砍点的，可以根据实际的 size 来考虑。这一点对概率估计方法的要求是，一个点到所有点的可达性在一次 BFS 采样中就可以求出，不然每次查询根到所有点的花销估计非常大。

### 可达概率估计方法的选取
### 索引构建的节点顺序
最原始的：先假设任意点对之间最短路只有一条，每个点一颗 SPT, 排序依据是每个点在所有 SPT 里的子孙数量和。因为这个值就是这个点覆盖最短路的数量。
### 单节点索引构建
设索引构建顺序为 $v_1, v_2, ..., v_n$, 每个点维护两个索引 $L_f(v), L_b(v)$. 设当前正在为 $v_i(1 \le i \le n)$ 构建索引，从 $v_i$ 开始正向 Dijkstra, 设当前 BFS 到的点为 $u$, 距离为 $\delta_u$, 对当前索引进行 $v_i\rightarrow u$ 的查询得到距离 $q$, 若 $q\le delta_u$(按理说只能等于，不可能小于)，不再 BFS $u$ 的后继，否则把 $(v_i, \delta_u)$ 加入 $L_b(u)$, 或者 $(u, \delta_u)$ 加入 $L_f(v_i)$. 同理反向 Dijkstra 处理一下 $L_f(u)$ 或 $L_b(v_i)$.
### 可能没有用的细节优化
 * $|L(v)| \le \alpha n$ 的时候用 hash map, 否则直接开个数组。这样可以把查询加速到 $O(\min(L_f(s), L_b(t)))$
 * 在所有 $|L(v)| \ge \beta n$ 的点之间计算最短路并 cache.或者干脆作为索引的一部分。

## 查询优化
1. 记可达概率 f(u,v)，是否有 $f(u,v)\ge f(u,w) * f(w,v)$，没有这个查询优化就是扯蛋。
2. 如果 u,v 的索引里找到了多个重合点，能否更精确估计 $f(u,v)$. 比如互不相交的直接相加，有重合的图结构就取最大。

对第二点的进一步阐述，设 $S(u,v)$ 是所有 uv 路径上的边集合，当 $S(u,w_1)+S(w_1,v)$ 与 $S(u,w_2)+S(w_2,v)$ 无重合时相加，有重合时取最大。如果这个关系是个等价关系，就很好弄了。

## 证明
记事件 A: s 能到达 u, 事件 B: u 能达到 t。求证 $p(A)*p(B)\le p(A\cap B)$.  
证明：$p(A)*p(B)\le p(A\cap B)$  
$\Leftrightarrow p(A\cap B)\ge (p(A\cap B)+p(A\cap \neg B))*(B)$  
$\Leftrightarrow p(A\cap B)*p(\neg B)\ge p(A\cap \neg B)*p(B)$  
$\Leftrightarrow p(A|B)\ge p(A| \neg B)$  
为什么不直接 $p(A|B)\ge p(A)$ 最后这步还是没理由啊。