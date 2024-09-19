#include "apriori.h"
#include <iostream>
#include <ctime>
#include <set>
#include <vector>

using namespace std;

void printFrequentItemsets(const vector<vector<set<int>>>& frequentItemsetsPerLevel) {
    for (int i = 0; i < frequentItemsetsPerLevel.size(); ++i) {
        cout << "Level " << i + 1 << ": " << frequentItemsetsPerLevel[i].size() << " frequent itemsets" << endl;

        for (const auto& itemset : frequentItemsetsPerLevel[i]) {
           cout << "{ ";
            for (int item : itemset) {
                cout << item << " ";
     }
            cout << "} ";
        }
        cout << endl;
    }
}

int main() {
    string filename = "data4.txt";
    vector<vector<int>> dataset = readDataset(filename);
    int min_sup = 5;
    double min_conf = 0.5;

    vector<vector<set<int>>> frequentItemsetsPerLevel;
    clock_t start = clock();
    apriori(dataset, min_sup, frequentItemsetsPerLevel);
    clock_t end = clock();

    cout << "Execution time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    printFrequentItemsets(frequentItemsetsPerLevel);

    generateAssociationRules(frequentItemsetsPerLevel, min_conf);

    return 0;
}

