#include "apriori.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

vector<vector<int>> readDataset(const string& filename) {
    ifstream file(filename);
    string line;
    vector<vector<int>> dataset;

    if (!file.is_open()) {
        cerr << "Unable to open file" << endl;
        return dataset;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string label;
        vector<int> transaction;
        int item;

        // Read and ignore the transaction label
        ss >> label;

        // Read the items
        while (ss >> item) {
            transaction.push_back(item);
        }
        dataset.push_back(transaction);
    }

    file.close();
    return dataset;
}


vector<set<int>> generateCandidates1(const vector<vector<int>>& dataset) {
    set<int> allItems;
    for (const auto& transaction : dataset) {
        for (int item : transaction) {
            allItems.insert(item);
        }
    }

    vector<set<int>> candidates;
    for (int item : allItems) {
        candidates.push_back({ item });
    }

    return candidates;
}

vector<set<int>> generateCandidatesNext(const vector<set<int>>& frequentItemsets) {
    vector<set<int>> candidates;

    int size = frequentItemsets.size();
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            set<int> candidate = frequentItemsets[i];
            candidate.insert(frequentItemsets[j].begin(), frequentItemsets[j].end());
            if (candidate.size() == frequentItemsets[0].size() + 1) {
                candidates.push_back(candidate);
            }
        }
    }

    return candidates;
}

map<set<int>, int> countSupport(const vector<set<int>>& candidates, const vector<vector<int>>& dataset) {
    map<set<int>, int> supportCount;

    for (const auto& transaction : dataset) {
        set<int> transactionSet(transaction.begin(), transaction.end());
        for (const auto& candidate : candidates) {
            if (includes(transactionSet.begin(), transactionSet.end(), candidate.begin(), candidate.end())) {
                supportCount[candidate]++;
            }
        }
    }

    return supportCount;
}

vector<set<int>> pruneCandidates(const map<set<int>, int>& supportCount, int min_sup, int numTransactions) {
    vector<set<int>> frequentItemsets;

    for (const auto& pair : supportCount) {
        if (pair.second >= min_sup) {
            frequentItemsets.push_back(pair.first);
        }
    }

    return frequentItemsets;
}

void apriori(const vector<vector<int>>& dataset, int min_sup, vector<vector<set<int>>>& frequentItemsetsPerLevel) {
    vector<set<int>> candidates1 = generateCandidates1(dataset);
    map<set<int>, int> supportCount1 = countSupport(candidates1, dataset);
    vector<set<int>> frequentItemsets1 = pruneCandidates(supportCount1, min_sup, dataset.size());
    frequentItemsetsPerLevel.push_back(frequentItemsets1);

    int k = 1;
    while (!frequentItemsetsPerLevel[k - 1].empty()) {
        vector<set<int>> candidatesNext = generateCandidatesNext(frequentItemsetsPerLevel[k - 1]);
        map<set<int>, int> supportCountNext = countSupport(candidatesNext, dataset);
        vector<set<int>> frequentItemsetsNext = pruneCandidates(supportCountNext, min_sup, dataset.size());
        frequentItemsetsPerLevel.push_back(frequentItemsetsNext);
        k++;
    }
}

double calculateSupport(const vector<set<int>>& transactions, const set<int>& itemset) {
    int count = 0;
    for (const auto& transaction : transactions) {
        bool contains = true;
        for (int item : itemset) {
            if (transaction.find(item) == transaction.end()) {
                contains = false;
                break;
            }
        }
        if (contains) {
            ++count;
        }
    }
    return static_cast<double>(count) / transactions.size();
}

// Function to generate association rules
void generateAssociationRules(const vector<vector<set<int>>>& frequentItemsetsPerLevel, double min_conf) {
    cout << "Generating Association Rules..." << endl;

    // Iterate over all levels except the first (no rules from single items)
    for (size_t i = 1; i < frequentItemsetsPerLevel.size(); ++i) {
        for (const auto& itemset : frequentItemsetsPerLevel[i]) {
            // Generate all non-empty subsets of the itemset
            int n = itemset.size();
            vector<int> items(itemset.begin(), itemset.end());
            for (int mask = 1; mask < (1 << n); ++mask) {
                set<int> antecedent;
                set<int> consequent = itemset;

                for (int j = 0; j < n; ++j) {
                    if (mask & (1 << j)) {
                        antecedent.insert(items[j]);
                        consequent.erase(items[j]);
                    }
                }

                if (!antecedent.empty() && !consequent.empty()) {
                    double antecedent_support = calculateSupport(frequentItemsetsPerLevel[0], antecedent);
                    double rule_support = calculateSupport(frequentItemsetsPerLevel[0], itemset);
                    double confidence = rule_support / antecedent_support;

                    if (confidence >= min_conf) {
                        cout << "Rule: {";
                        for (int item : antecedent) {
                            cout << item << " ";
                        }
                        cout << "} -> {";
                        for (int item : consequent) {
                            cout << item << " ";
                        }
                        cout << "} with confidence: " << confidence << endl;
                    }
                }
            }
        }
    }
}

