#ifndef APRIORI_H
#define APRIORI_H

#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

vector<vector<int>> readDataset(const string& filename);
vector<set<int>> generateCandidates1(const vector<vector<int>>& dataset);
vector<set<int>> generateCandidatesNext(const vector<set<int>>& frequentItemsets);
map<set<int>, int> countSupport(const vector<set<int>>& candidates, const vector<vector<int>>& dataset);
vector<set<int>> pruneCandidates(const map<set<int>, int>& supportCount, int min_sup, int numTransactions);
void apriori(const vector<vector<int>>& dataset, int min_sup, vector<vector<set<int>>>& frequentItemsetsPerLevel);
void generateAssociationRules(const vector<vector<set<int>>>& frequentItemsetsPerLevel, double min_conf);

#endif 

