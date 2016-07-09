// MEDIAN DEGREE project

#include <iostream>
#include <unordered_map>
#include <priority_queue>
#include <string>
#include <chrono>
#include "json.hpp"

class GraphVertex; // Graph vertex: contains a priority queue of transactions

class MedianDegreeApp {
public:
  using Graph = std::unordered_map<std::string, nlohmann::json>;
private:

  Graph vGraph;
};
