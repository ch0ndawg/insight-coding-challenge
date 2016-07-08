// MEDIAN DEGREE project

#include <iostream>
#include <unordered_map>
#include <list>
#include <priority_queue>
#include <string>
#include "json.hpp"

class GraphNode;
class MedianDegreeApp {
public:
  using Graph = std::unordered_map<std::string, GraphNode>;
private:
  Graph vGraph;
};
