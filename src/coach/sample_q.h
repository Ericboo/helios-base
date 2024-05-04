#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <string.h>

std::unordered_map<std::string, std::unordered_map<std::string, double>> q_table;
std::vector<std::vector<int>> generateCombinations(const std::vector<int>& unum, int hetero);
std::vector<std::vector<int>>  initialize_table();
int findIndexOfMax(const std::vector<int>& vec);