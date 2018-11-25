#include "pch.h"
#include "mini_regex.h"

automate mini_regex::union_regex_automates(automate a1, automate a2, int& next_state_index) {
	automate res;
	res.set_start_state(next_state_index);
	for (long long s : a1.final_states) res.add_final_state(s);
	for (long long s : a2.final_states) res.add_final_state(s);
	for (pair<long long, map<char, set<long long>>> p1 : a1.trans_table) {
		long long s1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long s2 : p2.second) {
				res.add_transition(s1, c, s2);
			}
		}
	}

	for (pair<long long, map<char, set<long long>>> p1 : a2.trans_table) {
		long long s1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long s2 : p2.second) {
				res.add_transition(s1, c, s2);
			}
		}
	}

	res.add_transition(next_state_index, CAT_OP, a1.start_state);
	res.add_transition(next_state_index, CAT_OP, a2.start_state);
	next_state_index++;
	return res;
}

automate mini_regex::star_regex_automate(automate a, int& next_state_index) {
	long long a_start_state = a.start_state;
	set<long long> a_final_states = a.final_states;
	for (long long s : a_final_states) {
		a.add_transition(s, CAT_OP, next_state_index);
	}
	a.add_transition(next_state_index, CAT_OP, a_start_state);
	a.set_start_state(next_state_index);
	a.final_states = set<long long>{ next_state_index };

	next_state_index++;
	return a;
}

automate mini_regex::concat_regex_automates(automate a1, automate a2) {
	automate res;
	for (pair<long long, map<char, set<long long>>> p1 : a1.trans_table) {
		long long s1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long s2 : p2.second) res.add_transition(s1, c, s2);
		}
	}
	for (pair<long long, map<char, set<long long>>> p1 : a2.trans_table) {
		long long s1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long s2 : p2.second) res.add_transition(s1, c, s2);
		}
	}
	res.set_start_state(a1.start_state);
	for (long long fs : a2.final_states) {
		res.add_final_state(fs);
	}
	for (long long fs : a1.final_states) {
		res.add_transition(fs, CAT_OP, a2.start_state);
	}
	return res;
}

// warning exits the program if the syntax is incorrect
// TODO : use exceptions
string mini_regex::postfix_transform(string s) {
	// add concatenation operator denoted by CAT_OP
	string s2;
	for (char c : s) {
		if (s2.size() != 0 && s2.back() != '(' && s2.back() != '|' && c != ')' && c != '|' && c != '*') s2.push_back(CAT_OP);
		s2.push_back(c);
	}
	//cout << "+concat operator : " << s2 << endl;
	s = s2;

	string res = "";
	int pos = 0;
	stack<pair<int, int>> operators_stack;
	// pair<int, int>::first is the CAT_OP operator count
	// pair<int, int>::second is the '|' operator count
	pair<int, int> current_operators(0, 0);
	while (pos < s.size()) {
		int i;
		string tail;
		switch (s[pos]) {
		case CAT_OP: // concatenation operator
			current_operators.first++;
			break;
		case '(':
			// allocate new operators counter
			operators_stack.push(current_operators);
			if (pos != 0) current_operators.first++;
			res.push_back('(');
			current_operators.first = 0;
			current_operators.second = 0;
			break;
		case ')':
			// complete the postfix expression and get the last stack
			for (int i = 0; i < current_operators.first; ++i) res.push_back(CAT_OP);
			for (int i = 0; i < current_operators.second; ++i) res.push_back('|');
			if (operators_stack.size() == 0) {
				cerr << "invalid character at " << pos << endl;
				exit(1);
			}
			current_operators = operators_stack.top();
			operators_stack.pop();
			res.push_back(')');
			if (current_operators.first != 0) {
				current_operators.first--;
				res.push_back(CAT_OP);
			}
			break;
		case '*':
			tail = "";
			i = res.size() - 1;
			while (i >= 0 && (res[i] == '|' || res[i] == CAT_OP)) {
				tail = res[i] + tail;
				--i;
			}
			if (i < 0) {
				// error
				cerr << "invalid character at " << pos << endl;
				exit(1);
			}
			else if (res[i] == '*') break;
			res = res.substr(0, i + 1) + '*' + tail;
			break;
		case '|':
			current_operators.second++;
			break;
		default:
			res.push_back(s[pos]);
			if (current_operators.first != 0) {
				res.push_back(CAT_OP);
				current_operators.first--;
			}
			break;
		}
		pos++;
	}
	
	for (int i = 0; i < current_operators.first; ++i) res.push_back(CAT_OP);
	for (int i = 0; i < current_operators.second; ++i) res.push_back('|');
	if (!operators_stack.empty()) {
		cerr << "\")\" expected" << endl;
		exit(1);
	}
	return res;
}

automate mini_regex::parse_postfix_format(string& s, int& next_state_index) {
	char c = s.back();
	s.pop_back();
	automate a1, a2;
	switch (c) {
	case '*':
		a1 = parse_postfix_format(s, next_state_index);
		return star_regex_automate(a1, next_state_index);
	case CAT_OP:
		a1 = parse_postfix_format(s, next_state_index);
		a2 = parse_postfix_format(s, next_state_index);
		return concat_regex_automates(a2, a1);
	case '|':
		a1 = parse_postfix_format(s, next_state_index);
		a2 = parse_postfix_format(s, next_state_index);
		return union_regex_automates(a1, a2, next_state_index);
	case ')':
		a1 = parse_postfix_format(s, next_state_index);
		s.pop_back();
		return a1;
	default:
		automate a;
		a.set_start_state(next_state_index);
		a.add_final_state(next_state_index + 1);
		a.add_transition(next_state_index, c, next_state_index + 1);
		next_state_index += 2;
		return a;
	}
	return automate();
}

regex_matcher mini_regex::parse_expression(string s) {
	string postfixed = postfix_transform(s);
	//cout << "postfix : " << postfixed << endl;
	int i = 0;
	automate a = parse_postfix_format(postfixed, i);
	//a.print_automate();
	return generate_regex_matcher(a);
}

regex_matcher mini_regex::generate_regex_matcher(automate a) {
	map<long long, map<char, set<long long>>> trans_table = a.trans_table;

	map<long long, long long> visit_index;
	map<long long, long long> min_neighbour_index;
	map<long long, long long> components_mapping;
	stack<long long> dfs_stack;
	map<long long, bool> in_stack;

	for (long long s : a.states) {
		visit_index[s] = -1;
		min_neighbour_index[s] = -1;
		components_mapping[s] = s;
		in_stack[s] = false;
	}

	// Tarjan algorithm implementation to find strongly connected states
	// i.e : states that can reach each others using 0 length transitions
	auto scc = [&visit_index, &min_neighbour_index, &dfs_stack, &in_stack, &components_mapping, &trans_table](long long state, long long& index, auto& scc) -> void {
		visit_index[state] = index;
		min_neighbour_index[state] = index;
		index++;
		dfs_stack.push(state);
		in_stack[state] = true;

		for (long long state2 : trans_table[state][CAT_OP]) {
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

	// find strongly connected components for each state
	long long i = 0;
	for (long long s : a.states) {
		if (visit_index[s] != -1) continue;
		scc(s, i, scc);
	}

	// construct dirceted acyclic reduced graph of the states graph
	map<long long, map<char, set<long long>>> dag_transitions;
	set<long long> dag_states;
	long long dag_start_state = components_mapping[a.start_state];
	for (pair<long long, long long> p : components_mapping) dag_states.insert(p.second);
	for (pair<long long, map<char, set<long long>>> p1 : trans_table) {
		long long state1 = p1.first;
		for (pair<char, set<long long>> p2 : p1.second) {
			char c = p2.first;
			for (long long state2 : p2.second) {
				if (c == CAT_OP && components_mapping[state1] == components_mapping[state2])
					continue;
				dag_transitions[components_mapping[state1]][c].insert(components_mapping[state2]);
			}
		}
	}

	map<long long, set<long long>> reachable;
	map<long long, bool> visited;
	for (long long s : dag_states) {
		visited[s] = false;
	}

	// find final state components
	set<long long> dag_final_states;
	for (long long s : a.final_states) dag_final_states.insert(components_mapping[s]);

	// finds reachable components
	auto reachable_func = [&visited, &dag_transitions, &reachable](long long state, auto& f) -> void {
		visited[state] = true;
		set<long long> r{ state };
		for (long long state2 : dag_transitions[state][CAT_OP]) {
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
				set<long long> r3 = dag_transitions[state][c];
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
	i = 0;
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

	return regex_matcher(a.alphabet, res_states, res_start_state, res_final_states, res_transitions);
}
