#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "util.hpp"

using namespace std;
using simpler_key = pair<double, double>;
using simpler_value = pair<int, int>;
using simpler_index_type = vector<pair<simpler_key, simpler_value>>;
using indexes_t = pair<simpler_index_type, simpler_index_type>;

void
fill_I_O(indexes_t& indexes, const vector<double>& node_times,
         const vector<edge>& edges, const std::size_t& last_et_size)
// Fill I and O.  This is not described in Algorithm T,
// but can be sleuthed from msprime/tests/tsutil.py
{
    for (std::size_t i = last_et_size; i < edges.size(); ++i)
        {
            auto & e = edges[i];
            indexes.first.emplace_back(std::make_pair(e.l, -node_times[e.p]),
                               std::make_pair(e.p, e.c));
            indexes.second.emplace_back(std::make_pair(e.r, node_times[e.p]),
                               std::make_pair(e.p, e.c));
        }
    sort(indexes.first.begin(), indexes.first.end(),
         [](const simpler_index_type::value_type& a,
            const simpler_index_type::value_type& b) {
             return a.first < b.first;
         });
    sort(indexes.second.begin(), indexes.second.end(),
         [](const simpler_index_type::value_type& a,
            const simpler_index_type::value_type& b) {
             return a.first < b.first;
         });
    //for (auto& e : edges)
    //    {
    //        I.emplace_back(std::make_pair(e.l, -node_times[e.p]),
    //                       std::make_pair(e.p, e.c));
    //        O.emplace_back(std::make_pair(e.r, node_times[e.p]),
    //                       std::make_pair(e.p, e.c));
    //    }
    //sort(I.begin(), I.end(), [](const simpler_index_type::value_type& a,
    //                            const simpler_index_type::value_type& b) {
    //    return a.first < b.first;
    //});
    //sort(O.begin(), O.end(), [](const simpler_index_type::value_type& a,
    //                            const simpler_index_type::value_type& b) {
    //    return a.first < b.first;
    //});
}

void
algorithmT(const vector<edge>& edges, const simpler_index_type& I,
           simpler_index_type& O, const int nnodes, const double maxpos)
// Assumes tables are not empty.  Probably unsafe.
{
    vector<size_t> pi(nnodes, numeric_limits<size_t>::max());
    size_t j = 0, k = 0, M = edges.size();
    double x = 0.0;
    // TODO: replace .at with []
    while (j < M || x < maxpos)
        {
            while (k < M && O[k].first.first == x) // T4
                {
                    pi[O[k].second.second] = numeric_limits<size_t>::max();
                    ++k;
                }
            while (j < M && I[j].first.first == x) // Step T2
                {
                    // The entry for the child refers to
                    // the parent's location in the node table.
                    pi[I[j].second.second] = I[j].second.first;
                    ++j;
                }
            double right = maxpos;
            if (j < M)
                {
                    right = min(right, I[j].first.first);
                }
            if (k < M)
                {
                    right = min(right, O[k].first.first);
                }
            //for(auto & p : pi){cout << p << '\n';}
            //std::exit(0);
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
    bool output = (argc > 3) ? 1 : 0;   
    int next_node = 2 * N;
    vector<edge> edges;
    vector<double> node_times(2 * N, 0);
    mt19937 mt(42);
    uniform_real_distribution<> pos(0., 1.);
    std::size_t last_et_size = 0;
    indexes_t IO;
    IO.first.reserve(10000);
    IO.second.reserve(10000);
    for (int gen = 0; gen < ngens; ++gen)
        {
            uniform_int_distribution<> u(next_node - 2 * N, next_node);
            for (int ind = 0; ind < N; ++ind)
                {
                    auto p1 = u(mt);
                    auto p2 = u(mt);
                    auto brk = pos(mt);
                    edges.emplace_back(edge{ p1, next_node, 0, brk });
                    edges.emplace_back(edge{ p2, next_node, brk, 1.0 });
                    next_node++;
                    edges.emplace_back(edge{ p2, next_node, 0., brk });
                    edges.emplace_back(edge{ p1, next_node, brk, 1. });
                    next_node++;
                    node_times.push_back(gen);
                    node_times.push_back(gen);
                }
            fill_I_O(IO, node_times, edges, last_et_size);
            last_et_size = edges.size();
            algorithmT(edges, IO.first, IO.second, node_times.size(), 1.0);
        }
    if (output)
        {
            ofstream out("v4out_left.txt");
            for (auto& i : IO.first)
                {
                    out << i.first.first << ' ' << i.first.second << ' '
                        << i.second.first << ' ' << i.second.second << '\n';
                }
            out.close();
            out.open("v4out_right.txt");
            for (auto& i : IO.second)
                {
                    out << i.first.first << ' ' << i.first.second << ' '
                        << i.second.first << ' ' << i.second.second << '\n';
                }
        }
}
