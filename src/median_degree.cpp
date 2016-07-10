#include "median_degree.hpp"
#include "treap.hpp"

double dateDiff(const nlohmann::json& lhs, const nlohmann::json& rhs);
void MedianDegreeStruct::insert(const nlohmann::json &j)
{
    transactions.push(j);
    
    while (dateDiff(j,transactions.top()) >= 60.0) {
        
    }
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