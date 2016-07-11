#include "median_degree.hpp"
#include "treap.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

double dateDiff(const nlohmann::json& lhs, const nlohmann::json& rhs)
{
    const char *fmt = "%Y-%m-%dT%H:%M:%SZ";
    std::string s1 = lhs["created_time"];
    std::string s2 = rhs["created_time"];
    std::istringstream ss1(s1);
    std::istringstream ss2(s2);
    std::tm t1, t2;
    
    ss1 >> std::get_time(&t1, fmt);
    ss2 >> std::get_time(&t2, fmt);
    
    std::time_t rt1 = std::mktime(&t1);
    std::time_t rt2 = std::mktime(&t2);
    return difftime(rt1,rt2);
}

void MedianDegreeStruct::insert(const nlohmann::json &j)
{
    auto latest = transactions.rend();
    if (dateDiff(j,*latest) >= 60.0) return; // reject it
    for (auto earliest = transactions.begin(); dateDiff(j,*earliest) >= 60.0; transactions.erase(earliest)) {
        std::string actor = (*earliest)["actor"];
        std::string target = (*earliest)["target"];
        
        int da = degMap[actor]--;
        int dt = degMap[target]--;
        
        // remove them from the median tree
        medMap.remove(std::make_pair(da,actor));
        medMap.remove(std::make_pair(dt,target));
        
        // insert new values into the median tree
        medMap.insert(std::make_pair(da-1,actor),0);
        medMap.insert(std::make_pair(dt-1,target),0);
    }
    std::string actor = j["actor"];
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