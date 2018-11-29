#include "pch.h"
#include "automate.h"
#include <iostream>

void automate::add_transition(long long s1, char c, long long s2) {
	this->trans_table[make_tuple(s1, c)].insert(s2);
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

void automate::print() {
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
	for (pair<tuple<long long, char>, set<long long>> p1 : this->trans_table) {
		long long s1 = get<0>(p1.first);
		char c = get<1>(p1.first);
		for (long long s2 : p1.second) {
			cout << "(" << s1 << ", " << c << ", " << s2 << ")" << endl;
		}
	}
}

automate::automate(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<tuple<long long, char>, set<long long>> trans_table_i)
	: alphabet(alphabet_i), states(states_i), start_state(start_state_i), final_states(final_states_i), trans_table(trans_table_i)
{
	
}

void automate::delete_0_length_circuits() {
	map<long long, long long> visit_index;
	map<long long, long long> min_neighbour_index;
	stack<long long> dfs_stack;
	map<long long, bool> in_stack;
	map<long long, long long> components_mapping;
	map<tuple<long long, char>, set<long long>>& trans_table = this->trans_table;
	long long index = 0;
	for (long long s : this->states) {
		visit_index[s] = -1;
		min_neighbour_index[s] = -1;
		in_stack[s] = false;
		components_mapping[s] = s;
	}
	auto scc = [&visit_index, &min_neighbour_index, &dfs_stack, &in_stack, &components_mapping, &trans_table](long long state, long long& index, auto& scc) -> void {
		visit_index[state] = index;
		min_neighbour_index[state] = index;
		index++;
		dfs_stack.push(state);
		in_stack[state] = true;

		for (long long state2 : trans_table[make_tuple(state, CAT_OP)]) {
			if (visit_index[state2] == -1) {
				scc(state2, index, scc);
				if (min_neighbour_index[state2] < min_neighbour_index[state])
					min_neighbour_index[state] = min_neighbour_index[state2];
			}
			else if (in_stack[state2]) {
				if (visit_index[state2] < min_neighbour_index[state])
					min_neighbour_index[state] = visit_index[state2];
			}
		}
		if (min_neighbour_index[state] == visit_index[state]) {
			while (dfs_stack.top() != state) {
				long long i = dfs_stack.top();
				in_stack[i] = false;
				dfs_stack.pop();
				components_mapping[i] = components_mapping[state];
			}

			long long i = dfs_stack.top();
			in_stack[i] = false;
			dfs_stack.pop();
		}
	};

	for (long long s : this->states) {
		if (visit_index[s] == -1) {
			scc(s, index, scc);
		}
	}
	set<long long> new_States;
	set<long long> new_final_states;
	for (long long s : this->states) new_States.insert(components_mapping[s]);
	for (long long s : this->final_states) new_final_states.insert(components_mapping[s]);
	this->states = new_States;
	this->final_states = new_final_states;
	this->start_state = components_mapping[this->start_state];
	map<tuple<long long, char>, set<long long>> new_trans_table;
	for (pair<tuple<long long, char>, set<long long>> p1 : this->trans_table) {
		long long s1 = components_mapping[get<0>(p1.first)];
		char c = get<1>(p1.first);
		for (long long p2 : p1.second) {
			long long s2 = components_mapping[p2];
			if (c == CAT_OP && s1 == s2) continue;
			new_trans_table[make_tuple(s1, c)].insert(s2);
		}
	}
	this->trans_table = new_trans_table;
}

void automate::star_automaton(long long& next_state_index) {
	for (long long s : this->final_states) {
		this->add_transition(s, CAT_OP, next_state_index);
	}
	this->add_transition(next_state_index, CAT_OP, this->start_state);
	this->set_start_state(next_state_index);
	this->final_states.clear();
	this->final_states.insert(next_state_index);
	next_state_index++;
}

// warning : the 2 automata should have different states indexing
void automate::concat_automaton(const automate& a, long long& next_state_index) {
	for (pair<tuple<long long, char>, set<long long>> p1 : a.trans_table) {
		for (long long p2 : p1.second) {
			this->add_transition(get<0>(p1.first), get<1>(p1.first), p2);
		}
	}
	for (long long s : this->final_states)
		this->add_transition(s, CAT_OP, a.start_state);
	this->final_states = a.final_states;
}

void automate::union_automation(const automate& a, long long& next_state_index) {
	for (pair<tuple<long long, char>, set<long long>> p1 : a.trans_table) {
		for (long long p2 : p1.second) {
			this->add_transition(get<0>(p1.first), get<1>(p1.first), p2);
		}
	}
	this->final_states.insert(a.final_states.begin(), a.final_states.end());
	this->states.insert(next_state_index);
	this->add_transition(next_state_index, CAT_OP, this->start_state);
	this->add_transition(next_state_index, CAT_OP, a.start_state);
	this->set_start_state(next_state_index);
	next_state_index++;
}

bool automate::is_looping_over_itself(long long s) {
	bool is_looping = true;
	for (char c : this->alphabet) {
		tuple<long long, char> t = make_tuple(s, c);
		for (long long s2 : this->trans_table[t]) {
			if (s2 != s) {
				is_looping = false;
				break;
			}
		}
		if (!is_looping) break;
	}
	return is_looping;
}

void automate::convert_to_searcher() {
	for (long long s : this->states) {
		if (is_looping_over_itself(s)) continue;
		this->add_transition(s, CAT_OP, this->start_state);
	}
}

automate::automate(long long& index) { 
	/*this->set_start_state(index);
	this->states.insert(index);
	index++;*/
}

automate::~automate()
{
}
