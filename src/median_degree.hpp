// MEDIAN DEGREE project

#include <iostream>
#include <unordered_map>
#include <queue>
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

// this will be the key for the median updates tree. It is ordered lexicographically.
using DegName = std::pair<int, std::string>;
class JsonDateCompare {
    bool operator()(const nlohmann::json &lhs, const nlohmann::json& rhs) {
        return lhs["Date"] < rhs["Date"];
    }
};

class MedianDegreeStruct {
public:
    // Will branch to make a version that uses dates as priorities, eliminating this
    using TransactionList = std::priority_queue<nlohmann::json, std::vector<nlohmann::json>, JsonDateCompare>;
    using DegreeMap = std::unordered_map<std::string,int>;
    // actually, the values are not used. Some tweaking, perhaps, for a "Set Treap"
    // Treap by the strict definition: using a randomly generated priority
    // Will branch to make a version that uses dates as priorities, eliminating TransactionList
    using MedianMap = Treap<DegName,int,Random>;
    
    void insert(nlohmann::json j);
    double getMedianDegree() const;
private:
    TransactionList transactions;
    DegreeMap degMap;
    MedianMap medMap;
};
