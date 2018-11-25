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
	long long get_start();
	set<long long> get_final_states();
	set<long long> get_states();
	long long next_state(long long state, char c);
	void print();
	~regex_matcher();
};

