#include <iostream>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <chrono>
#include <utility>
#include <time.h> // POSIX version required
#include "json.hpp"
#include "treap.hpp"

class Random {
    int val;
public:
    Random() : val(std::rand()) {}
    bool operator<(Random rhs) { return val < rhs.val;}
};

class EmptyActorException {};


class MedianDegreeStruct {
public:
    // GRAPH DATA
    
    // A transaction is basically our native JSON format.
    //  The second pair will always be in increasing lexicographic order,
    //    since we do not distinguish direction of payments.
    //  The first item is the date, which is converted to system time from the JSON.
    //    REQUIRES: POSIX standard time.h; the ISO standard version is problematic
    using Transaction = std::pair<time_t,std::pair<std::string,std::string>>;
    
    // TransactionList is ordered first by time, then transaction participants
    //   The reason why we need this ordering is for efficient min and max;
    //     we want this in order to be able to maintain the window between
    //     earliest and latest transactions.
    using TransactionList = std::set<Transaction>;
    
    // This is the actual graph; it is stored as map of edges, rather than the traditional
    //   vertices with adjacency list, because for our purposes, we need to be able to find
    //   specific edges quickly. A more traditional structure favors things like DFS and BFS.
    using EdgeMap = std::map<std::pair<std::string,std::string>,time_t>;
    
    
    // MEDIAN DATA
    
    // this will be the key for the median updates tree. It is ordered lexicographically.
    using DegName = std::pair<int, std::string>;
    // this is required for us to store and look up degrees. In essence,
    //   it's how we store the vertices of the graph
    using DegreeMap = std::unordered_map<std::string,int>;
    // The actual median updates tree. It is implemented as an augmented treap, using
    //   dynamic order statistics. It is cleaner than augmenting a traditional red-black tree
    using MedianMap = Treap<DegName,int,Random>;
    
    // actual insertion function
    void insert(const nlohmann::json& j);
    double getMedianDegree() const;
private:
    // could make this public, if, say, we start to synchronize this data structure with actual
    // ticking clocks, with a live stream
    void evictOldTransactions(const Transaction &t);
    
    TransactionList transactions;
    EdgeMap graph;
    DegreeMap degMap;
    MedianMap medMap;
};
