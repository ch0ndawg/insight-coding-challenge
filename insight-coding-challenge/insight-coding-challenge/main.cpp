//
//  main.cpp
//  insight-coding-challenge
//
//  Created by Chris on 7/9/16.
//  Copyright © 2016 Nested Tori. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include "treap.hpp"

class Random {
    int val;
public:
    Random() : val(std::rand()) {}
    bool operator<(Random rhs) { return val < rhs.val;}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Treap<int,int,Random> t;
    
    for (int i=1;i <= 20; i++) {
        int j=std::rand();
        t.insert(j,j);
    }
    
    std::cout << t << std::endl;
    std::cout << t.orderStatistic(0).second << ' ' << t.orderStatistic(10).second;
    return 0;
}
