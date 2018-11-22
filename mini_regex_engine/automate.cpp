#include "pch.h"
#include "automate.h"
#include <iostream>

void automate::add_transition(long long s1, char c, long long s2) {
	this->trans_table[s1][c].insert(s2);
	this->states.insert(s1);
	this->states.insert(s2);
	this->alphabet.insert(c);
}

void automate::set_start_state(long long start_state) {
	this->start_state = start_state;
}

void automate::add_final_state(long long final_state) {
	this->final_states.insert(final_state);
}

void automate::print_automate() {
	cout << "alphabet : ";
	for (char c : this->alphabet) cout << c << " ";
	cout << endl;
	cout << "states : ";
	for (long long s : this->states) cout << s << " ";
	cout << endl;
	cout << "start state : " << this->start_state << endl;
	cout << "final states : ";
	for (long long s : this->final_states) cout << s << " ";
	cout << endl;
	cout << "transitions : " << endl;
	for (pair<long long, map<char, set<long long>>> p1 : this->trans_table) {
		long long s1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long s2 : p2.second) {
				cout << "(" << s1 << ", " << c << ", " << s2 << ")" << endl;
			}
		}
	}
}

automate::automate(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<long long, map<char, set<long long>>> trans_table_i) 
	: alphabet(alphabet_i), states(states_i), start_state(start_state_i), final_states(final_states_i), trans_table(trans_table_i)
{
	
}

automate::automate() {  }
automate::~automate()
{
}
