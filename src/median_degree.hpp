// MEDIAN DEGREE project

#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <chrono>
#include <utility>
#include "json.hpp"
#include "treap.hpp"

class Random {
    int val;
public:
    Random() : val(std::rand()) {}
    bool operator<(Random rhs) { return val < rhs.val;}
};

class EmptyActorException {};

// this will be the key for the median updates tree. It is ordered lexicographically.
// this will be converted to a native format as an optimization,
// but for now, we keep it here for proof of concept
using DegName = std::pair<int, std::string>;
struct JsonDateCompare {
    bool operator()(const nlohmann::json &lhs, const nlohmann::json& rhs) {
        return lhs["created_time"] < rhs["created_time"];
    }
};

class MedianDegreeStruct {
public:
    // Will branch to make a version that uses dates as priorities, eliminating this
    using TransactionList = std::multiset<nlohmann::json,JsonDateCompare>;
    using DegreeMap = std::unordered_map<std::string,int>;
    // actually, the values are not used. Some tweaking, perhaps, for a "Set Treap"
    // Treap by the strict definition: using a randomly generated priority
    // Will branch to make a version that uses dates as priorities, eliminating TransactionList
    using MedianMap = Treap<DegName,int,Random>;
    
    void insert(const nlohmann::json& j);
    double getMedianDegree() const;
private:
    TransactionList transactions;
    DegreeMap degMap;
    MedianMap medMap;
    
};
