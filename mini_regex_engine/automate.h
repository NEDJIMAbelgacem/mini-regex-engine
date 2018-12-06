#pragma once
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <algorithm>
#include <tuple>
#include <string>

#include "regex_instance.h"
#include "abstract_syntax_tree.h"

using namespace std;
// (alphabet, states, start_state, final_state, transitions table)
using ndfa_t = tuple<set<char>, set<long long>, long long, set<long long>, map<long long, map<char, long long>>>;

constexpr char CAT_OP = '\0';

// general finite state automate
class automate
{
public:
	set<char> alphabet;
	set<long long> states;
	long long start_state;
	set<long long> final_states;
	map<tuple<long long, char>, set<long long>> trans_table;
public:
	automate(long long& index);
	automate(abstract_syntax_tree* ast, long long& index);
	~automate();
	automate(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<tuple<long long, char>, set<long long>> trans_table_t);
	void set_start_state(long long start_state);
	void add_final_state(long long final_state);
	void add_transition(long long s1, char c, long long s2);
	void delete_0_length_circuits();
	void star_automaton(long long& next_state_index);
	void optional_automaton(long long& next_state_index);
	void plus_automaton(long long& next_next_state_index);
	void concat_automaton(const automate& a, long long& next_state_index);
	void union_automation(const automate& a, long long& next_state_index);
	string convert_to_dot_language();
	void render_automaton(string automaton_name);
	void print();
};