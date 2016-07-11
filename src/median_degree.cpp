#include "median_degree.hpp"
#include "treap.hpp"
#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>

double dateDiff(const nlohmann::json& lhs, const nlohmann::json& rhs)
{
    const char *fmt = "%Y-%m-%dT%H:%M:%SZ";
    std::string s1 = lhs["created_time"];
    std::string s2 = rhs["created_time"];
    tm t1, t2;
    
    strptime(s1.c_str(), fmt, &t1);
    strptime(s2.c_str(), fmt, &t2);
    
    time_t rt1 = timegm(&t1);
    time_t rt2 = timegm(&t2);
    return difftime(rt1,rt2);
}

void MedianDegreeStruct::insert(const nlohmann::json &j)
{
    std::string actor = j["actor"];
    if (actor.empty()) throw EmptyActorException();
    
    if (!transactions.empty()) {
        // latest element is at the very end
        auto latest = transactions.rbegin();
        if (dateDiff(j,*latest) >= 60.0) return; // reject it
        
        // remove everything outside the window
        auto earliest = transactions.begin();
        for ( ; earliest != transactions.end() && dateDiff(j,*earliest) >= 60.0; earliest++) {
            std::string actor = (*earliest)["actor"];
            std::string target = (*earliest)["target"];
            
            int da = degMap[actor]--;
            int dt = degMap[target]--;
            
            // remove them from the median tree
            medMap.remove(std::make_pair(da,actor));
            medMap.remove(std::make_pair(dt,target));
            
            // insert new values into the median tree [disallow zero]

            if (da-1>0) medMap.insert(std::make_pair(da-1,actor),0);
            if (dt-1>0) medMap.insert(std::make_pair(dt-1,target),0);
        }
        // actually remove all the earliest entries (earliest)
        transactions.erase(transactions.begin(),earliest);
    }
    transactions.insert(j);
    std::string target = j["target"];
    
    int da = degMap[actor]++;
    int dt = degMap[target]++;
    
    // remove them from the median tree
    medMap.remove(std::make_pair(da,actor));
    medMap.remove(std::make_pair(dt,target));
    
    // insert new values into the median tree
    medMap.insert(std::make_pair(da+1,actor),0);
    medMap.insert(std::make_pair(dt+1,target),0);
    
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