
#include <stdio.h>
#include "avl.h"
#include "block_allocator.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_math.h>

typedef struct {
    double left;
    double right;
    int parent;
    int child;
    int time;
} edge_t;

block_allocator_t allocator;
avl_tree_t left_index;
avl_tree_t right_index;

static int
cmp_edge_left_increasing_time(const void *a, const void *b) {
    const edge_t *ca = (const edge_t *) a;
    const edge_t *cb = (const edge_t *) b;
    int ret = (ca->left > cb->left) - (ca->left < cb->left);
    if (ret == 0) {
        ret = (ca->time > cb->time) - (ca->time < cb->time);
        if (ret == 0) {
            ret = (ca->child > cb->child) - (ca->child < cb->child);
        }
    }
    return ret;
}

static int
cmp_edge_right_decreasing_time(const void *a, const void *b) {
    const edge_t *ca = (const edge_t *) a;
    const edge_t *cb = (const edge_t *) b;
    int ret = (ca->right > cb->right) - (ca->right < cb->right);
    if (ret == 0) {
        ret = (ca->time < cb->time) - (ca->time > cb->time);
        if (ret == 0) {
            ret = (ca->child > cb->child) - (ca->child < cb->child);
        }
    }
    return ret;
}

static inline edge_t
get_edge(const avl_node_t *restrict avl_node)
{
    return *((edge_t *) avl_node->item);
}

static inline double
edge_left(const avl_node_t *restrict avl_node)
{
    return ((edge_t *) avl_node->item)->left;
}

static inline double
edge_right(const avl_node_t *restrict avl_node)
{
    return ((edge_t *) avl_node->item)->right;
}

static void
algT(int *parent, int max_node)
{
    double left, right, pos;
    avl_node_t *avl_node, *remove_start;
    avl_node_t *restrict in = left_index.head;
    avl_node_t *restrict out = right_index.head;
    edge_t edge;
    int j;

    /* Load the tree for start */
    left = 0;
    pos = 0;
    right = 1;
    /* TODO there's probably quite a big gain to made here by seeking
     * directly to the tree that we're interested in. */
    while (in != NULL && out != NULL) {
        while (out != NULL && (edge = get_edge(out)).right == pos) {
            printf("setting %d to -1\n", edge.child);
            parent[edge.child] = -1;
            out = out->next;
        }
        while (in != NULL && (edge = get_edge(in)).left == pos) {
            parent[edge.child] = edge.parent;
            in = in->next;
        }
        left = pos;
        right = 1.0;
        if (in != NULL) {
            right = GSL_MIN(right, edge_left(in));
        }
        if (out != NULL) {
            right = GSL_MIN(right, edge_right(out));
        }
        printf("VISITING %f %f\n", left, right);
        for (j = 0; j < max_node; j++) {
            printf("%d ", parent[j]);
        }
        printf("\n");
        pos = right;
    }
}


static void
add_edge(double left, double right, int time, int parent, int child)
{

    avl_node_t *avl_node;
    edge_t *edge = block_allocator_get(&allocator, sizeof(edge_t));

    edge->left = left;
    edge->right = right;
    edge->parent = parent;
    edge->child = child;
    edge->time = time;

    avl_node = block_allocator_get(&allocator, sizeof(*avl_node));
    avl_init_node(avl_node, edge);
    avl_node = avl_insert_node(&left_index, avl_node);
    assert(avl_node != NULL);

    avl_node = block_allocator_get(&allocator, sizeof(*avl_node));
    avl_init_node(avl_node, edge);
    avl_node = avl_insert_node(&right_index, avl_node);
    assert(avl_node != NULL);
    printf("Add %f %f %d %d %d\n", left, right, time, parent, child);

}

int
main(int argc, char **argv)
{
    gsl_rng *rng = gsl_rng_alloc(gsl_rng_default);
    int N = atoi(argv[1]);
    int ngens = atoi(argv[2]);
    int max_node = N * ngens * 2;
    int next_node = 2 * N;
    int gen, ind, p1, p2;
    double pos;
    int *parent = malloc(max_node * sizeof(*parent));

    memset(parent, 0xff, max_node * sizeof(*parent));

    block_allocator_alloc(&allocator, 8192);
    avl_init_tree(&left_index, cmp_edge_left_increasing_time, NULL);
    avl_init_tree(&right_index, cmp_edge_right_decreasing_time, NULL);

    for (gen = 0; gen < ngens; gen++) {
        for (ind = 0; ind < N; ind++) {
            p1 = next_node - gsl_rng_uniform_int(rng, 2 * N);
            p2 = next_node - gsl_rng_uniform_int(rng, 2 * N);
            pos = gsl_rng_uniform(rng);
            add_edge(0, pos, -gen, p1, next_node);
            add_edge(pos, 1, -gen, p2, next_node);
            next_node++;
            add_edge(0, pos, -gen, p2, next_node);
            add_edge(pos, 1, -gen, p1, next_node);
            next_node++;
        }
        algT(parent, max_node);
    }


    return 0;
}
