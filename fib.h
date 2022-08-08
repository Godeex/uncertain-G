#ifndef FIB_H
#define FIB_H

#include <string.h>
#include <vector>
#include <algorithm>
#include "struct.h"
#include "logger.h"

template<typename T>
struct Fib {
    static const int LGN = 32;
    struct Node {
        int l, r, p, ch;
        int marked, degree;
    };
    Storage<Node> nodes;
    Storage<T> values;

    inline int &l(int x) {
        return nodes._get(x)->l;
    }
    inline int &r(int x) {
        return nodes._get(x)->r;
    }
    inline int &p(int x) {
        return nodes._get(x)->p;
    }
    inline int &ch(int x) {
        return nodes._get(x)->ch;
    }
    inline int &degree(int x) {
        return nodes._get(x)->degree;
    }
    inline int &marked(int x) {
        return nodes._get(x)->marked;
    }

    int root;
    T root_value() const {
        return *values.get(root);
    }

    int link(int x, int y) {
        if (x == -1) return y;
        if (y == -1) return x;
        int r_temp = r(x);
        r(x) = y, l(y) = x;
        l(r_temp) = y, r(y) = r_temp;
        return x;
    }

    inline T& value(int x) {
        return *values._get(x);
    }

    int insert(const T &v) {
        // TODO: contiguous memory
        int id = values.newElement(v);
        nodes.newElement({id, id, -1, -1, 0, 0});

        if (root == -1) {
            root = id;
        } else {
            link(root, id);
            if (value(id) < value(root)) {
                root = {id};
            }
        }
        return {id};
    }

    static int merge_list[LGN];
    void merge(int x) {
        int y;
        while ((y = merge_list[degree(x)]) != -1) {
            if (value(y) < value(x)) {
                std::swap(y, x);
            }
            r(y) = l(y) = y;
            ch(x) = link(ch(x), y);
            p(y) = x;
            merge_list[degree(x)] = -1;
            degree(x)++;
        }
        merge_list[degree(x)] = x;
    }

    void delete_min() {
        if (root == -1) {
            return;
        }
        for (int i = 0; i < sizeof(merge_list)/sizeof(merge_list[0]); ++i) {
            merge_list[i] = -1;
        }
        if (ch(root) != -1) {
            r(l(ch(root))) = -1;
            for (int i = ch(root), nxt; i != -1; i = nxt) {
                nxt = r(i);
                p(i) = -1;
                merge(i);
            }
        }
        r(l(root)) = -1;
        for (int i = r(root), nxt; i != -1; i = nxt) {
            if (i != -1) nxt = r(i);
            merge(i);
        }

        int last = -1, first = -1;
        root = -1;
        for (int i = 0; i < LGN; ++i) {
            int x = merge_list[i];
            if (x != -1) {
                l(x) = last;
                if (last != -1) {
                    r(last) = x;
                }
                // update last
                last = x;
                // update first
                if (first == -1) {
                    first = x;
                }
                // update root
                if (root == -1) {
                    root = x;
                } else {
                    if (value(x) < value(root)) {
                        root = x;
                    }
                }
            }
        }
        if (last != -1) r(last) = first;
        if (first != -1) l(first) = last;
    }

    void cutAndLinkRoot(int fa, int x) {
        if (fa != -1) {
            if (r(x) == x) {
                ch(fa) = -1;
            } else {
                ch(fa) = r(x);
                l(ch(fa)) = l(x);
                r(l(x)) = ch(fa);
            }
            p(x) = -1;
            degree(fa)--;
            link(root, x);
            marked(x) = 0;
        }
    }

    void decreaseKey(int x, T newValue) {
        if (x==-1 || -1==root || value(x) < newValue) {
            return; // error
        }
        value(x) = newValue;
        // cut x from x->p
        int y = p(x);
        cutAndLinkRoot(y, x);
        // cut ancestors of x
        while (y!=-1 && marked(y)) {
            int f = p(y);
            cutAndLinkRoot(f, y);
            y = f;
        }
        if (y!=-1 && p(y) != -1) marked(y) = 1;
        if (newValue < value(root)) {
            root = x;
        }
    }

    int empty() const {
        return root == -1;
    }

    // void print_tree(Node *x) {
    //     if (x) {
    //         if (x->ch) {
    //             x->ch->l->r = NULL;
    //             Logger::Get(logDEBUG2) << x->value.node << "-" << x->ch->value.node << std::endl;
    //             print_tree(x->ch);
    //             x->ch->l->r = x->ch;
    //         }
    //         print_tree(x->r);
    //     }
    // }
    // void print() {
    //     if (root) {
    //         root->l->r = NULL;
    //         print_tree(root);
    //         root->l->r = root;
    //     }
    // }

    // std::vector<Node *> garbages;
    // void collect(Node *x) {
        // garbages.push_back(x);
    // }
    // void clean() {
    //     for (auto x: garbages) {
    //         delete x;
    //     }
    //     garbages.clear();
    // }

    // void delete_tree(Node *x) {
    //     if (x) {
    //         auto y = x->r;
    //         if (x->ch) {
    //             x->ch->l->r = NULL;
    //             delete_tree(x->ch);
    //         }
    //         delete x;
    //         delete_tree(y);
    //     }
    // }

    // void delete_all() {
        // if (root) {
        //     root->l->r = NULL;
        //     delete_tree(root);
        // }
        // clean();
    // }

    void reset() {
        // delete_all();
        nodes.size = 0;
        values.size = 0;
        root = -1;
    }

    Fib():root(-1){}
    ~Fib() {
        // delete_all();
    }
};

template<typename T> int Fib<T>::merge_list[Fib<T>::LGN];

#endif