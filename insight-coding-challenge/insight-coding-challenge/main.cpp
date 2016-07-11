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
#include <sstream>
#include "treap.hpp"
#include "median_degree.hpp"


int main(int argc, const char * argv[]) {
    
    MedianDegreeStruct m;
    // argv[1] will be the input file
    // argv[2] will be the output file
    // if none provided, output file will be to stdout
    
    
    std::fstream infile0;
    if (argc > 0) infile0.open(argv[1],std::fstream::in);
    std::fstream outfile0;
    if (argc > 1) outfile0.open(argv[2],std::fstream::out | std::fstream::app);
    
    std::istream& infile = infile0? infile0 : std::cin;
    std::ostream& outfile = outfile0? outfile0 : std::cout; // to stdout
    
    while (!infile.eof()) {
        nlohmann::json j;
        std::string s;
        std::getline(infile,s);
        // if it is a blank line, keep going
        if (s.empty()) continue;
        
        // process the string as a single line
        std::stringstream ss(s);
        ss >> j;
        m.insert(j);
        outfile << std::fixed << std::setprecision(2) << m.getMedianDegree() << '\n'; // use NaN when empty
    }
}
