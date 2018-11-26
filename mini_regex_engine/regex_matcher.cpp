#include "pch.h"
#include "regex_matcher.h"

regex_matcher::regex_matcher(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<long long, map<char, long long>> trans_table_i)
	: alphabet(alphabet_i), states(states_i), start_state(start_state_i), final_states(final_states_i), trans_table(trans_table_i)
{
}

regex_matcher::~regex_matcher()
{
}

bool regex_matcher::match(string s) {
	long long current_state = this->start_state;
	for (char& c : s) {
		if (this->trans_table[current_state].find(c) == this->trans_table[current_state].end()) 
			return false;
		current_state = trans_table[current_state][c];
	}
	return this->final_states.find(current_state) != this->final_states.end();
}

void regex_matcher::print() {
	cout << "alphabet : ";
	for (char c : this->alphabet) cout << c << " ";
	cout << endl;
	cout << "states : ";
	for (long long s : this->states) cout << s << " ";
	cout << endl;
	cout << "start state : " << this->start_state << endl;
	cout << "final state : ";
	for (long long s : this->final_states) cout << s << " ";
	cout << endl;
	cout << "transitions : " << endl;
	for (pair<long long, map<char, long long>> p1 : this->trans_table) {
		long long s1 = p1.first;
		for (pair<char, long long> p2 : p1.second) {
			char c = p2.first;
			long long s2 = p2.second;
			cout << "(" << s1 << ", " << c << ", " << s2 << ")" << endl;
		}
	}
}

long long regex_matcher::next_state(long long state, char c) {
	if (this->states.find(state) == this->states.end() || this->alphabet.find(c) == this->alphabet.end()) {
		return -1;
	}
	return this->trans_table[state][c];
}

long long regex_matcher::get_start() {
	return this->start_state;
}

set<long long> regex_matcher::get_final_states() {
	return this->final_states;
}

set<long long> regex_matcher::get_states() {
	return this->states;
}