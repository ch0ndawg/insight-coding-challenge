#include "median_degree.hpp"
#include "treap.hpp"
#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>

inline double dateDiff(const MedianDegreeStruct::Transaction& lhs, const MedianDegreeStruct::Transaction& rhs)
{
    return difftime(lhs.first,rhs.first);
}

void MedianDegreeStruct::insert(const nlohmann::json &j)
{
    // get date and time of transaction as a UNIX time
    const char *dateFmt = "%Y-%m-%dT%H:%M:%SZ";
    std::string dateString = j["created_time"];
    tm transactionTimeStruct;
    strptime(dateString.c_str(), dateFmt, &transactionTimeStruct);
    
    time_t transactionTime = timegm(&transactionTimeStruct);
    
    // get actor; throw exception if invalid
    std::string actor = j["actor"];
    if (actor.empty()) throw EmptyActorException();
    
    // get target (could do exception handling here, too)
    std::string target = j["target"];
    
    // Canonization procedure: to assist in non-directedness of the graph, always make
    //   the lexicographically first name the "actor", i.e., whether the actor is Bob or Alice.
    //   and the target is Alice or Bob, the pair is always going to be (Alice,Bob).
    
    if (actor > target) std::swap(actor,target);
    
    Transaction t = std::make_pair(transactionTime, std::make_pair(actor,target));
    
    if (!transactions.empty()) { // latest element is at the very end
        auto latest = transactions.rbegin();
        if (dateDiff(*latest,t) >= 60.0) return; // reject it
        evictOldTransactions(t);
    }
        
    
    // check to see if the edge is already in the graph
    auto graphEntry = graph.find(t.second);
    // if it's not, then update the degrees
    if (graphEntry == graph.end()) {
        // increment their degrees
        int da = degMap[actor]++;
        int dt = degMap[target]++;
        
        // remove them from the median tree
        medMap.remove(std::make_pair(da,actor));
        medMap.remove(std::make_pair(dt,target));
        
        // insert new values into the median tree
        medMap.insert(std::make_pair(da+1,actor),0);
        medMap.insert(std::make_pair(dt+1,target),0);
    } else { // if so, update the timestamps
        graph[t.second] = t.first; // update the time
        // erase the old time
        transactions.erase(std::make_pair(graphEntry->second,graphEntry->first));
    }
    
    // insert the new entry into the transactions
    transactions.insert(t);
}

// evict old transactions
void MedianDegreeStruct::evictOldTransactions(const Transaction &t)
{
    // remove everything outside the window
    auto earliest = transactions.begin();
    for ( ; earliest != transactions.end() && dateDiff(t,*earliest) >= 60.0; earliest++) {
        std::string actor = earliest->second.first;
        std::string target = earliest->second.second;
        
        // remove it from the graph
        graph.erase(earliest->second);
        
        // decrement their degrees
        int da = degMap[actor]--;
        int dt = degMap[target]--;
        
        // remove them from the median tree
        medMap.remove(std::make_pair(da,actor));
        medMap.remove(std::make_pair(dt,target));
        
        // insert new values into the median tree [disallow zero]
        
        if (da-1>0) medMap.insert(std::make_pair(da-1,actor), /*unused*/ 0);
        else degMap.erase(actor); // remove it from the degree map entirely
                
        if (dt-1>0) medMap.insert(std::make_pair(dt-1,target), /*unused*/ 0);
        else degMap.erase(target); // remove it from the degree map entirely
    }
    // actually remove all the earliest entries (earliest)
    transactions.erase(transactions.begin(),earliest);
}


double MedianDegreeStruct::getMedianDegree() const
{
    int n = medMap.size();
    if (n <= 0) return 0.0/0.0; // NaN
    if (n % 2) { // odd size
        return medMap.orderStatistic(n/2).first.first; // truncates
    } else {
        return 0.5 * (medMap.orderStatistic(n/2 - 1).first.first + medMap.orderStatistic(n/2).first.first);
    }
}