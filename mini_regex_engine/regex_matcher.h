#pragma once
#include <map>
#include <set>
#include <stack>
#include <iostream>
#include "automate.h"


using namespace std;

class regex_matcher
{
	set<char> alphabet;
	set<long long> states;
	long long start_state;
	set<long long> final_states;
	map<long long, map<char, long long>> trans_table;
public:
	regex_matcher(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<long long, map<char, long long>> trans_table_i);
	bool match(string s);
	void print();
	~regex_matcher();
};

