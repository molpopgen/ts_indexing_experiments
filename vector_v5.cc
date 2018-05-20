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

struct index_recorder
{
    simpler_index_type left_temp, right_temp;
    inline void
    operator()(indexes_t& indexes, const vector<double>& node_times,
               const vector<edge>& edges, const size_t last_et_size)
    {
        vector<size_t> L(edges.size() - last_et_size);
        iota(L.begin(), L.end(), last_et_size);
        auto R(L);
        std::sort(
            L.begin(), L.end(), [&edges, &node_times](size_t a, size_t b) {
                auto t = -node_times[edges[a].p], tb = -node_times[edges[b].p];
                return tie(edges[a].l, t) < tie(edges[b].l, tb);
            });
        std::sort(R.begin(), R.end(),
                  [&edges, &node_times](size_t a, size_t b) {
                      return tie(edges[a].r, node_times[edges[a].p])
                             < tie(edges[b].r, node_times[edges[b].p]);
                  });
        left_temp.clear();
        right_temp.clear();
        auto beg = indexes.first.begin();
        for (auto& l : L)
            {
                simpler_key k{ edges[l].l, -node_times[edges[l].p] };
                auto itr = upper_bound(
                    beg, indexes.first.end(), k,
                    [](const simpler_key& key,
                       const simpler_index_type::value_type& v) {
                        return tie(key.first, key.second)
                               < tie(v.first.first, v.first.second);
                        //return key < v.first;
                    });
                if (itr < indexes.first.end())
                    {
                        assert(itr->first > k);
                    }
                left_temp.insert(left_temp.end(), beg, itr);
                left_temp.emplace_back(k, make_pair(edges[l].p, edges[l].c));
                beg = itr;
            }
        left_temp.insert(left_temp.end(), beg, indexes.first.end());

        beg = indexes.second.begin();
        for (auto& r : R)
            {
                simpler_key k{ edges[r].r, node_times[edges[r].p] };
                auto itr = upper_bound(
                    beg, indexes.second.end(), k,
                    [](const simpler_key& key,
                       const simpler_index_type::value_type& v) {
                        return tie(key.first, key.second)
                               < tie(v.first.first, v.first.second);
                        //    return key < v.first;
                    });
                right_temp.insert(right_temp.end(), beg, itr);
                right_temp.emplace_back(k, make_pair(edges[r].p, edges[r].c));
                beg = itr;
            }
        right_temp.insert(right_temp.end(), beg, indexes.second.end());
        assert(left_temp.size() == edges.size());
        assert(right_temp.size() == edges.size());
        indexes.first.swap(left_temp);
        indexes.second.swap(right_temp);
    }
};

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
    bool output = false;
    if (argc > 3)
        {
            output = atoi(argv[1]);
        }
    using pair_type = pair<key_type, edge>;
    size_t last_size = 0;

    int N = atoi(argv[1]);
    int ngens = atoi(argv[2]);
    int next_node = 2 * N;
    vector<edge> edges;
    vector<double> node_times(2 * N, 0);
    mt19937 mt(42);
    uniform_real_distribution<> pos(0., 1.);
    std::size_t last_et_size = 0;
    indexes_t IO;
    index_recorder recorder;
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
            recorder(IO, node_times, edges, last_et_size);
            last_et_size = edges.size();
            algorithmT(edges, IO.first, IO.second, node_times.size(), 1.0);
        }
    if (output)
        {
            ofstream out("v5out_left.txt");
            for (auto& i : IO.first)
                {
                    out << i.first.first << ' ' << i.first.second << ' '
                        << i.second.first << ' ' << i.second.second << '\n';
                }
            out.close();
            out.open("v5out_right.txt");
            for (auto& i : IO.second)
                {
                    out << i.first.first << ' ' << i.first.second << ' '
                        << i.second.first << ' ' << i.second.second << '\n';
                }
        }
}
