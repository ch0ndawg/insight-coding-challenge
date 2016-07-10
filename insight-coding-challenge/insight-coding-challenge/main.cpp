//
//  main.cpp
//  insight-coding-challenge
//
//  Created by Chris on 7/9/16.
//  Copyright © 2016 Nested Tori. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include "treap.hpp"
#include "median_degree.hpp"


int main(int argc, const char * argv[]) {
    
    MedianDegreeStruct m;
    // argv[1] will be the input file
    // argv[2] will be the output file
    // if none provided, output file will be to stdout
    
    std::fstream infile(argv[1],std::fstream::in);
    std::fstream outfile0(argv[2],std::fstream::out | std::fstream::app);
    
    std::ostream& outfile = outfile0? outfile0 : std::cout; // to stdout
    outfile.width(10);
    outfile.precision(2);
    nlohmann::json j;
    while (infile >> j) {
        m.insert(j);
        outfile << m.getMedianDegree() << '\n'; // use NaN when empty
    }
}
