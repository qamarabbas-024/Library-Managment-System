#include "utils.h"
#include <cstdlib>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

vector<string> split(const string &line) {
    vector<string> row;
    string temp;
    stringstream ss(line);
    while (getline(ss, temp, ',')) {
        row.push_back(temp);
    }
    return row;
}

vector<vector<string>> readCSV(const string &file) {
    vector<vector<string>> data;
    ifstream fin(file);
    string line;
    while (getline(fin, line)) {
        data.push_back(split(line));
    }
    fin.close();
    return data;
}

void writeCSV(const string &file, const vector<vector<string>> &data) {
    ofstream fout(file);
    for (const auto &row : data) {
        for (int i = 0; i < row.size(); i++) {
            fout << row[i];
            if (i < row.size() - 1) fout << ",";
        }
        fout << endl;
    }
    fout.close();
}
