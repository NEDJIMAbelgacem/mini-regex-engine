#include "pch.h"
#include "mini_regex.h"

regex_instance mini_regex::parse_expression(string s) {
	// add concatenation operator denoted by CAT_OP
	// s2 is the regex after the insertion of concatenation operator
	string s2;
	for (char c : s) {
		if (s2.size() != 0 && s2.back() != '(' && s2.back() != '|' && c != ')' && c != '|' && c != '*') s2.push_back(CAT_OP);
		s2.push_back(c);
	}
	abstract_syntax_tree* ast = abstract_syntax_tree::construct_ast(s2);
	long long i = 0;
	automate a(ast, i);
	regex_instance r = generate_regex_instance(a);
	delete ast;
	return r;
}

regex_instance mini_regex::generate_regex_instance(automate a) {

	// construct dirceted acyclic reduced graph of the states graph
	a.delete_0_length_circuits();

	map<tuple<long long, char>, set<long long>>& dag_transitions = a.trans_table;
	set<long long>& dag_states = a.states;
	long long& dag_start_state = a.start_state;
	set<long long>& dag_final_states = a.final_states;

	map<long long, set<long long>> reachable;
	map<long long, bool> visited;
	for (long long s : dag_states) {
		visited[s] = false;
	}

	// finds reachable components
	auto reachable_func = [&visited, &dag_transitions, &reachable](long long state, auto& f) -> void {
		visited[state] = true;
		set<long long> r{ state };
		for (long long state2 : dag_transitions[make_tuple(state, CAT_OP)]) {
			if (state2 == state) continue;
			if (!visited[state2]) f(state2, f);
			r.insert(reachable[state2].begin(), reachable[state2].end());
		}
		reachable[state] = r;
	};

	// construct reachability map
	for (long long s : dag_states) 
		if (!visited[s]) 
			reachable_func(s, reachable_func);

	// find all components that can reach a final state using 0 length transitions
	set<long long> final_components;
	for (long long s1 : dag_states) {
		bool is_final_state = false;
		set<long long> r = reachable[s1];
		for (long long s2 : dag_final_states) {
			if (r.find(s2) != r.end()) {
				is_final_state = true;
				break;
			}
		}
		if (is_final_state) final_components.insert(s1);
	}

	// final_components will be the new dag final states
	dag_final_states = final_components;

	// construct non deterministic finite state machine with sets
	map<set<long long>, map<char, set<long long>>> sets_transitions;
	set<set<long long>> sets_states;
	stack<set<long long>> sets_stack;
	set<long long> sets_start_state{ dag_start_state };

	sets_stack.push(sets_start_state);
	while (!sets_stack.empty()) {
		set<long long> s = sets_stack.top();
		sets_stack.pop();
		sets_states.insert(s);

		set<long long> in_reach;
		for (long long state : s) 
			in_reach.insert(reachable[state].begin(), reachable[state].end());

		for (char c : a.alphabet) {
			if (c == CAT_OP) continue;
			set<long long> c_reachable;
			for (long long state : in_reach) {
				set<long long> r3 = dag_transitions[make_tuple(state, c)];
				c_reachable.insert(r3.begin(), r3.end());
			}
			sets_transitions[s][c] = c_reachable;
			if (sets_states.find(c_reachable) == sets_states.end()) 
				sets_stack.push(c_reachable);
		}
	}

	// find all the final states sets
	set<set<long long>> sets_final_states;
	for (set<long long> s : sets_states) {
		bool is_final_state = false;
		for (long long dag_fstate : dag_final_states) {
			if (s.find(dag_fstate) != s.end()) {
				is_final_state = true;
				break;
			}
		}
		if (is_final_state) sets_final_states.insert(s);
	}

	// map sets into integers
	set<long long> res_states;
	set<long long> res_final_states;
	long long res_start_state;
	map<long long, map<char, long long>> res_transitions;

	map<set<long long>, long long> sets_mapping;
	int i = 0;
	for (set<long long> s : sets_states) {
		sets_mapping[s] = i;
		res_states.insert(i);
		++i;
	}
	for (set<long long> s : sets_final_states) {
		res_final_states.insert(sets_mapping[s]);
	}

	res_start_state = sets_mapping[sets_start_state];

	for (pair<set<long long>, map<char, set<long long>>> p1 : sets_transitions) {
		long long s1 = sets_mapping[p1.first];
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			long long s2 = sets_mapping[p2.second];
			res_transitions[s1][c] = s2;
		}
	}

	return regex_instance(a.alphabet, res_states, res_start_state, res_final_states, res_transitions);
}
