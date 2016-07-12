# Median Degree Project
### Christopher Tiee

This is my submission for the [Insight Data Engineering Coding challenge](https://github.com/InsightDataScience/coding-challenge). 

## Prerequisites and Dependencies

The program is written in C++, *almost* conforming to the ISO C++14 standard. The only non-conformant part is that it uses the POSIX version of the time functions (this should be the usual `time.h` on UNIX systems), because the ISO standard versions of the time functions are still broken (at least on my system).

* NOTE: A correction was approved past the submission deadline with permission--my compiler let an error in an unused function, as well as some template deductions, to sneak past. In order to facilitate using different compilers, I have changed the makefile, which requires the following environment variables: `CPP` should be set to the C++ compiler (e.g. `CPP=g++-6`), `CFLAGS` should be set to any important options to specify a standard (e.g. for GCC and clang, `CFLAGS="-std=C++14"`), and finally `OPTS` for other options. I usually specify options for optimization, e.g., `OPTS=-O2`). The command-line syntax is still pretty much based on GCC, so use that (at least verson 5) if possible.

As for other dependencies, it uses Niels Lohmann's [JSON reader](https://github.com/nlohmann/json), but that is already included in the sources as a single header file, `json.hpp`. The treap source `treap.hpp` is also included. It is based on a structure I used for a [HackerRank exercise](https://www.hackerrank.com/challenges/array-and-simple-queries) involving balanced trees.

In summary, simply make sure the proper POSIX time functions are available and that one has an ISO C++14 conformant compiler.

## Installation and Running Instructions
First, as mentioned above, one must make sure the compiler environment variables are set: `CPP` should be set to the C++ compiler, `CFLAGS` should be set to any important options needed to specify a standard (e.g. for GCC and clang, `CFLAGS="-std=C++14"`), and finally `OPTS` for other options. I usually specify `-O2` for optimization).

Running and testing the program conforms to usage notes for testing and running as described in the README for the Insight Coding Challenge; one should either do `./run.sh` at the root of the directory or use the provided script in the testing suite. Note that `run.sh` will compile source, and as such, the testing script `run_tests.sh` will also recompile before each test. That said, compilation takes about 2 seconds (but this is useful to know if anyone is going to be timing from the initial run from start to finish.). Four test cases have been included in the `insight_testsuite` directory.

# Remarks on the Development
## General Design Overview

Generally, what governed the choices with the aim of scalability was to ensure that *all* operations would occur in logarithmic time at least in the average case. This pretty much meant that I should start looking at tree-like data structures. The JSON data was converted to a "native" data struture, a triplet--the JSON package is used mostly for its parsing.

* The main data structure is of course the graph, which is represented here by a (tree) map of edges (the key is the actor-target pair, and the value is the timestamp). We enforce non-directionality of the edges by simply lexicographically ordering the names, namely, regardless of who are the actor and target--Alice and Bob, or Bob and Alice--the resulting key is always (Alice, Bob). As payments stream in, it is added to the graph, provided, of course, that the timestamp is within the 60-second window. One possible optimization we could make is to use a hash table instead of a tree for a map--C++ requires a manual definition of hash function here, so rather than focus on that detail, I chose a tree.

* To maintain the timestamps, we use a set (a map whose keys are simultaneously its values) whose keys consist of the timestamps, and then the actor-target pair, essentially reversing the order that the edge map comes in. The ordering is by timestep first, and then by the two names (same as in the graph map). The second ordering is done to distinguish entries with the same timestamp. This structure is necessary because one needs to do be able to find both min and max (for earliest and latest entries) to maintain the window. These two data structures together can be said to comprise whole graph.

* The timestamps are stored as UNIX time (seconds since Epoch), and the conversion is provided by the time library (hence the need for POSIX time conversion functions). There is no checking for garbled times, however; so of course for real-world use, I would need to improve things to handle dirty data.

* For the median updating, we needed two more data structures. In fact, the structure pretty much mirrors for vertices what the above is for edges. First, to keep track of degrees, we use a hash table--for each name (a key), which is a vertex, we assign the degree (the value). A tree structure is not needed because we don't need to order the names here. We need this because when inserting, we have to look up the names to find the degrees. This structure can be said to capture the vertices of the graph.

* For the actual keeping track of the median we use an augmented tree. Here, the particular kind of tree we choose to augment is the treap, which is self-balancing due to use of priorities. This data structure is considerably shorter to implement than Red-Black or AVL trees. Again, there probably is some way of augmenting standard library facilities so I don't have to go "roll my own". For the key, we use another Cartesian product: the degree, and then the name of the vertex (the same pattern we used for the edges). The value is unused, i.e., it is actually a "seap"--a set with priorities. However, there is another piece of ancillary data, the size, in each treap node. This allows one to look up tree elements by numerical order, which in turn coincides with order statistics; then getting the median is calculated the usual way (except, of course, with zero-based indices).

For more documentation and design notes, see the source code comments.


## Trade-Offs Made
* The first major trade-off, mostly mentioned above already, is that the graph is not actually created in its entirety (as a traditional adjacency list structure), for the simple reason that looking up vertices would be done linear time, which is not very scalable. At the very least, one needs to supplement such a traditional structure by allowing fast lookups. Since we don't use common graph algorithms such as depth-first-search or breadth-first-search, it turns out we don't really need to store it as an adjacency list. However, of course, this doesn't come without costs (as any trade-off would): storing lots of ancillary data (in some sense, the graph is "implicit") first requires extra space, and then also requires maintaining consistency. This may create some difficulties in building fault-tolerance into the system, as well as ensuring thread-safety in a multithreaded environment, simply because various class invariants are broken during an updating.

* Some more optimizations could come in the form of better copying behavior (keys and so forth are copied as they are shuttled around), and various different forms of hashing can be employed to speed up lookups. Using such things, however, could make the code more difficult to understand and maintain.

## Conclusion
This has been a very interesting project. It raises plenty of good design questions and made me think hard about what are good data structures. Thank you for your consideration; I hope you enjoy testing the app as much as I did writing it.
