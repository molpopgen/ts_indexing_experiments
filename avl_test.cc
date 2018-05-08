#include "avl.h"
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>
#include <cstdlib>
#include "util.hpp"

using namespace std;

struct avl_data
{
    double left, right, time;
    int parent, child;
};

static int
comp_avl_data_left(const void* a_, const void* b_)
{
    const avl_data* a = static_cast<const avl_data*>(a_);
    const avl_data* b = static_cast<const avl_data*>(b_);
    int ret = (a->left > b->left) - (a->left < b->left);
    if (ret == 0)
        {
            ret = (a->time < b->time) - (a->time > b->time);
            if (ret == 0)
                {
                    ret = (a->child < b->child) - (a->child > b->child);
                }
        }
    return ret;
}

static int
comp_avl_data_right(const void* a_, const void* b_)
{
    const avl_data* a = static_cast<const avl_data*>(a_);
    const avl_data* b = static_cast<const avl_data*>(b_);
    int ret = (a->right > b->right) - (a->right < b->right);
    if (ret == 0)
        {
            ret = (a->time < b->time) - (a->time > b->time);
            if (ret == 0)
                {
                    ret = (a->child < b->child) - (a->child > b->child);
                }
        }
    return ret;
}

static void
free_avl_data(void* d)
{
    delete (static_cast<avl_data*>(d));
}

static inline avl_data
get_edge(const avl_node_t* avl_node)
{
    return *((avl_data*)avl_node->item);
}

static inline double
edge_left(const avl_node_t* avl_node)
{
    return ((avl_data*)avl_node->item)->left;
}

static inline double
edge_right(const avl_node_t* avl_node)
{
    return ((avl_data*)avl_node->item)->right;
}

static void
algT(avl_tree_t* left_index, avl_tree_t* right_index, int max_node)
{
    vector<int> parent(max_node, -1);
    double left, right, pos;
    avl_node_t *avl_node, *remove_start;
    avl_node_t* in = left_index->head;
    avl_node_t* out = right_index->head;
    avl_data edge;
    int j;

    /* Load the tree for start */
    left = 0;
    pos = 0;
    right = 1;
    /* TODO there's probably quite a big gain to made here by seeking
     * directly to the tree that we're interested in. */
    while (in != NULL && out != NULL)
        {
            while (out != NULL && (edge = get_edge(out)).right == pos)
                {
                    //printf("setting %d to -1\n", edge.child);
                    parent[edge.child] = -1;
                    out = out->next;
                }
            while (in != NULL && (edge = get_edge(in)).left == pos)
                {
                    parent[edge.child] = edge.parent;
                    in = in->next;
                }
            left = pos;
            right = 1.0;
            if (in != NULL)
                {
                    right = std::min(right, edge_left(in));
                }
            if (out != NULL)
                {
                    right = std::min(right, edge_right(out));
                }
			//for(auto p : parent){std::cout <<p<<'\n';}
			//std::exit(0);
            //printf("VISITING %f %f\n", left, right);
            //for (j = 0; j < max_node; j++)
            //    {
            //        printf("%d ", parent[j]);
            //    }
            //printf("\n");
            pos = right;
        }
}

int
main(int argc, char** argv)
{
    size_t last_size = 0;

    int N = atoi(argv[1]);
    int ngens = atoi(argv[2]);
    int next_node = 2 * N;
    mt19937 mt(42);
    uniform_real_distribution<> pos(0., 1.);
    avl_data* e;
    avl_node_t* insert_loc;
    avl_tree_t* input_left = avl_alloc_tree(comp_avl_data_left, free_avl_data);
    avl_tree_t* output_right
        = avl_alloc_tree(comp_avl_data_right, free_avl_data);
    for (int gen = 0; gen < ngens; ++gen)
        {
            uniform_int_distribution<> u(next_node - 2 * N, next_node);
            for (int ind = 0; ind < N; ++ind)
                {
                    auto p1 = u(mt);
                    auto p2 = u(mt);
                    auto brk = pos(mt);
                    e = new avl_data{ 0., brk, -double(gen), p1, next_node };
                    insert_loc = avl_insert(input_left, e);
                    e = new avl_data{ brk, 1., -double(gen), p2, next_node };
                    insert_loc = avl_insert(input_left, e);

                    e = new avl_data{ 0., brk, double(gen), p1, next_node };
                    insert_loc = avl_insert(output_right, e);
                    e = new avl_data{ brk, 1., double(gen), p2, next_node };
                    insert_loc = avl_insert(output_right, e);

                    //input_left.emplace(
                    //    std::make_tuple(0., -double(gen), p1, next_node),
                    //    edge{ p1, next_node, 0., brk });
                    //input_left.emplace(
                    //    std::make_tuple(brk, -double(gen), p2, next_node),
                    //    edge{ p2, next_node, brk, 1. });
                    //output_right.emplace(
                    //    std::make_tuple(brk, double(gen), p1, next_node),
                    //    edge{ p1, next_node, 0., brk });
                    //output_right.emplace(
                    //    std::make_tuple(1., double(gen), p2, next_node),
                    //    edge{ p2, next_node, brk, 1. });
                    ++next_node;
                    e = new avl_data{ brk, 1., double(gen), p1, next_node };
                    insert_loc = avl_insert(input_left, e);
                    e = new avl_data{ 0., brk, -double(gen), p2, next_node };
                    insert_loc = avl_insert(input_left, e);

                    e = new avl_data{ brk, 1., double(gen), p1, next_node };
                    insert_loc = avl_insert(output_right, e);
                    e = new avl_data{ 0., brk, double(gen), p2, next_node };
                    insert_loc = avl_insert(output_right, e);
                    //input_left.emplace(
                    //    std::make_tuple(0., -double(gen), p2, next_node),
                    //    edge{ p2, next_node, 0., brk });
                    //input_left.emplace(
                    //    std::make_tuple(brk, -double(gen), p1, next_node),
                    //    edge{ p1, next_node, brk, 1. });
                    //output_right.emplace(
                    //    std::make_tuple(brk, double(gen), p2, next_node),
                    //    edge{ p2, next_node, 0., brk });
                    //output_right.emplace(
                    //    std::make_tuple(1., double(gen), p1, next_node),
                    //    edge{ p1, next_node, brk, 1. });
                    ++next_node;
                }
            algT(input_left, output_right, next_node);
            //algT(input_left, output_right, next_node);
        }
    avl_free_tree(input_left);
    avl_free_tree(output_right);
}
