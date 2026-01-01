#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;
void clearScreen();
vector<string> split(const string &line);
vector<vector<string>> readCSV(const string &file);
void writeCSV(const string &file, const vector<vector<string>> &data);

#endif
