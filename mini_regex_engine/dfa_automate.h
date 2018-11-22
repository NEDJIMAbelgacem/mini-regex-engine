#pragma once
#include <map>
#include <set>

using namespace std;

class dfa_automate
{
public:
	long long start_state;
	set<long long> final_states;
	map<long long, map<char, long long>> trans_table;
public:
	dfa_automate();
	~dfa_automate();
};

