#pragma once
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <algorithm>
#include "regex_matcher.h"
using namespace std;

constexpr char CAT_OP = '\0';

// general finite state automate
class automate
{
public:
	set<char> alphabet;
	set<long long> states;
	long long start_state;
	set<long long> final_states;
	map<long long, map<char, set<long long>>> trans_table;
public:
	automate(long long& index);
	~automate();
	automate(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<long long, map<char, set<long long>>> trans_table_i);
	void set_start_state(long long start_state);
	void add_final_state(long long final_state);
	void add_transition(long long s1, char c, long long s2);
	void delete_0_length_circuits();
	void star_automaton(long long& next_state_index);
	void concat_automaton(const automate& a, long long& next_state_index);
	void union_automation(const automate& a, long long& next_state_index);
	void print();
};