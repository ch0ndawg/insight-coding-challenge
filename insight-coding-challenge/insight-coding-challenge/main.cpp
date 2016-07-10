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
#include "treap.hpp"


int main(int argc, const char * argv[]) {
    // insert code here...
    Treap<int,int,Random> t;
    
    for (int i=1;i <= 20; i++) {
        int j=std::rand();
        t.insert(j,j);
    }
    
    std::cout << t << std::endl;
    std::cout << t.orderStatistic(0).second << ' ' << t.orderStatistic(10).second << std::endl;
    
    Treap<std::string, int, Random> tree2;
    
    for (int i=0; i < 3; i++) {
        std::string s;
        int j;
        std::cout << "Enter name: ";
        getline(std::cin, s);
        std::cout << "Enter number of points: ";
        std::cin >> j;
        std::cin.ignore(); // discard the newline
        tree2.insert(s,j);
    }
    
    std::string s2;
    std::cout << "What would you like to delete? ";
    getline(std::cin,s2);
    
    tree2.remove(s2);
    
    for (int i=0; i< tree2.size(); i++) {
        auto curr = tree2.orderStatistic(i);
        std::cout << curr.first << ": " << curr.second << std::endl;
    }
    std::pair<int, std::string> k1 = std::make_pair(3, "A");
    std::pair<int, std::string> k2 = std::make_pair(3, "B");
    
    if (k1 < k2) std::cout << "A is less";
    else std::cout << "B is less";
    return 0;
}
