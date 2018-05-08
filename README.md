# Experiments with indexing Tree Sequences

Q: How do we best run Algorithm's T/L from Kelleher et al. (2016) each generation?

A: It's complicated.

## The code

This repo provides four programs that run a simplistic WF simulation where there is one mandatory recombination per
mating.  Mating is random.  No selection.

We do no simplification at all.  The goal is to see what happens as we add more and more unsimplified data to our index
vectors.

We define an edge type, (parent, child, left, right).  Time is measured from past to present.

To sort the index vectors, define a key (position, time, parent, child), where position is left or right, and time is
generation or -generation as appropriate.  Define a value, which is an edge.

The following two programs use these key/value pairs:

* map.cc Adds new index data into a `std::map` as each edge is generated.
* vector.cc Adds new index data, and then sorts all index data at the end of each generation.

Each generation, we run Alg T.  The exact same code is used for both programs (it is implemented as a template in
util.hpp to make this possible).

We have two additional programs:

* vector_v2.cc This is based on what I'm doing in [fwdpp_experimental](http://github.com/molpopgen/fwdpp_experimenal).
  I populate a `vector<edge>` and a `vector<double>`.  The latter is a minimal `NodeTable`.  Here, the input and output
  index vectors are `vector<size_t>`, and are generated from scratch each generation.
* vector_v3.cc This is a mod of vector_v2, where we use a simpler key type, (position, time), and a simpler value type,
  (parent, child).  The implementation of Alg T here had to be customized.

All 4 versions are confirmed to give the same marginal tree vectors, based on short tests.

## Performance and profiling

The profiles in pdf format in this repo are the results of running `run.sh`, which requires Google's perftools.  The
script runs N=10,000 diploids for 300 generations.

The total run times are shown in each profile.  From slowest to fastest:

* vector_v2, 1432s
* vector, 600s
* vector_v3, 345s
* map, 330s

Wait a minute--an implementation based on filling an ever-growing vector each generation does nearly as well as the map?
Comparing the profiles, the time spent in populating the container versus iterating through it in Alg T are flipped.
The map spends the majority of time in the algorithm and vector_v3 spends most of its time sorting (and Alg T is close
to free).  There are two points to make:

1. Iterating a vector and a map are both linear in container size.  But, cache coherency make the constant access per
   element very different.
2. Efficiency of filling and sorting containers depends a lot of `sizeof(key)`.  The key for the map must be larger
   because it must be unique.  We have no such uniqueness requirement for the vector implementation in vector_v3, and we
   also store a smaller value type.  The smaller types in vector_v3 change the intercept point at which we'd prefer a
   map vs a vector.

Because we want to run Alg T/L every generation, if we simplify very often, we will often run the algos on much smaller
node/edge tables, pushing us towards favoring the vector.  If we look at performance of these implementations for N=1e6 over 5 generations,
we get the following:

```sh
for i in vector vector_v2 vector_v3 map avl_test
do
/usr/bin/time -f "%e %M" ./$i 1000000 5
done
30.59 2385652
67.09 1172768
15.36 1621416
72.27 3799732
68.60 4424568
```

And over one generation:

```sh
1.55 393540
2.96 254656
0.93 371136
9.95 768612
10.05 893788
```

We see that the map takes a ton more memory, and is much less efficient when the total size of the data is smaller.
Even our relatively crappy `vector_v2` beats it over 5 generations. (And keep in mind that all vector implementations
sort each generation!!)

Thus, if we simplify relatively often, we should prefer vectors over maps.  It is likely that simplifying each
generation is worth it because simplification is itself fast, and serves to keep the inputs to the algorithms small.


