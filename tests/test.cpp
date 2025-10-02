#include <map>
#include <cassert>
#include <iostream>

#include "skip_list.hpp"


int main() {
    std::map<int, int> m;
    SkipList<std::string, int> sl(3);
    sl["kewuaa"] = 1;
    sl.insert("1", 111);
    sl.insert("9", 999);
    sl.insert("5", 555);
    sl.insert("hello", 0);
    std::cout << sl << std::endl;
    if (auto v = sl.find("5"); v) {
        std::cout << "found 5 -> " << v->second << std::endl;
    } else {
        std::cout << "5 not found" << std::endl;
    }
    sl.remove("5");
    std::cout << sl << std::endl;
    if (auto v = sl.find("5"); v) {
        std::cout << "found " << v->second << std::endl;
    } else {
        std::cout << "5 not found" << std::endl;
    }

    for (auto& [key, value] : sl) {
        std::cout << key << ": " << value << std::endl;
    }
    std::cout << sl["hello"] << std::endl;
}
