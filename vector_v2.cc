#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>
#include <cstdlib>
#include "util.hpp"

using namespace std;
std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
fill_I_O(const vector<double>& node_times, const vector<edge>& edges)
// Fill I and O.  This is not described in Algorithm T,
// but can be sleuthed from msprime/tests/tsutil.py
{
    std::vector<std::size_t> I(edges.size(), 0), O(edges.size(), 0);
    using edge_iterator = std::vector<edge>::const_iterator;
    std::vector<edge_iterator> edge_pointers;
    for (auto i = std::begin(edges);
         i < std::end(edges); ++i)
        {
            edge_pointers.push_back(i);
        }
    // To fill I, sort the pointers by increasing left
    // and increasing parent time (moving from present back into past).
    // Because, unlike msprime, we are not recording node times
    // backwards into the past, our sort is simple.
    std::sort(std::begin(edge_pointers), std::end(edge_pointers),
              [&node_times](const edge_iterator i, const edge_iterator j) {
                  if (i->l == j->l)
                      {
                          return node_times[i->p] < node_times[j->p];
                      }
                  return i->l < j->l;
              });

    for (std::size_t i = 0, j = 1; i < I.size(); ++i, ++j)
        {
            I[i] = static_cast<std::size_t>(std::distance(
                std::begin(edges), edge_pointers[i]));
        }

    // To fill O, sort by right and decreasing parent time, again
    // moving into the past.  Our one trick is to sort on the
    // reversed node times.
    std::sort(std::begin(edge_pointers), std::end(edge_pointers),
              [&node_times](const edge_iterator i, const edge_iterator j) {
                  if (i->r == j->r)
                      {
                          return node_times[i->p] > node_times[j->p];
                      }
                  return i->r < j->r;
              });
    for (std::size_t i = 0; i < O.size(); ++i)
        {
            O[i] = static_cast<std::size_t>(std::distance(
                std::begin(edges), edge_pointers[i]));
        }
    return std::make_pair(std::move(I), std::move(O));
}

void
algorithmT(const vector<edge>& edges, const std::vector<std::size_t>& I,
           const std::vector<std::size_t>& O, const int nnodes,
           const double maxpos)
// Assumes tables are not empty.  Probably unsafe.
{
    std::vector<std::size_t> pi(nnodes,
                                std::numeric_limits<std::size_t>::max());
    std::size_t j = 0, k = 0, M = edges.size();
    double x = 0.0;
    // TODO: replace .at with []
    while (j < M || x < maxpos)
        {
            while (k < M && edges[O[k]].r == x) // T4
                {
                    auto& edge_ = edges[O[k]];
                    pi[edge_.c] = std::numeric_limits<std::size_t>::max();
                    ++k;
                }
            while (j < M && edges[I[j]].l == x) // Step T2
                {
                    auto& edge_ = edges[I[j]];
                    // The entry for the child refers to
                    // the parent's location in the node table.
                    pi[edge_.c] = edge_.p;
                    ++j;
                }
            double right = maxpos;
            if (j < M)
                {
                    right = std::min(right, edges[I[j]].l);
                }
            if (k < M)
                {
                    right = std::min(right, edges[O[k]].r);
                }
            x = right;
        }
}

int
main(int argc, char** argv)
{
    using pair_type = pair<key_type, edge>;
    size_t last_size = 0;

    int N = atoi(argv[1]);
    int ngens = atoi(argv[2]);
    int next_node = 2 * N;
    vector<edge> edges;
    vector<double> node_times(2*N,0);
    mt19937 mt(42);
    uniform_real_distribution<> pos(0., 1.);
    for (int gen = 0; gen < ngens; ++gen)
        {
            uniform_int_distribution<> u(next_node - 2 * N, next_node);
            for (int ind = 0; ind < N; ++ind)
                {
                    auto p1 = u(mt);
                    auto brk = pos(mt);
                    edges.emplace_back(edge{p1, next_node++, 0, brk});
                    p1 = u(mt);
                    edges.emplace_back(edge{p1, next_node++, brk, 1.0});
                    node_times.push_back(gen);
                    node_times.push_back(gen);
                }
            auto IO = fill_I_O(node_times, edges);
            algorithmT(edges, IO.first, IO.second, node_times.size(), 1.0);
        }
}
