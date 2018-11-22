#include "pch.h"
#include "regex.h"

automate regex::union_regex_automates(automate a1, automate a2, int& next_state_index) {
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

automate regex::star_regex_automate(automate a, int& next_state_index) {
	long long a_start_state = a.start_state;
	set<long long> a_final_states = a.final_states;
	a.final_states.clear();
	a.final_states.insert(a_start_state);
	for (long long s2 : a_final_states) {
		a.add_transition(s2, CAT_OP, a_start_state);
	}
	next_state_index++;
	return a;
}

automate regex::concat_regex_automates(automate a1, automate a2) {
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

string regex::postfix_transform(string s) {
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
		switch (s[pos]) {
		case CAT_OP: // concatenation operator
			current_operators.first++;
			break;
		case '(':
			// allocate new operator counter
			operators_stack.push(current_operators);
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
			break;
		case '*':
			// push directly into the resulting string *maximum priority*
			if (pos == 0) {
				// error
				cerr << "invalid expression start *" << endl;
				exit(1);
			}
			else if (s[pos - 1] == ')') res += '*';
			else {
				string tail = "";
				int i = res.size() - 1;
				while (i >= 0 && res[i] != '*' && (res[i] == '|' || res[i] == CAT_OP)) {
					tail = res[i] + tail;
					--i;
				}
				if (i < 0) {
					// err
					cerr << "invalid character at " << pos << endl;
					exit(1);
				} else if (res[i] == '*') break;
				res = res.substr(0, i + 1) + '*' + tail;
			}
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

automate regex::parse_postfix_format(string& s, int& next_state_index) {
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

regex_matcher regex::parse_expression(string s) {
	string postfixed = postfix_transform(s);
	cout << "postfix : " << postfixed << endl;
	int i = 0;
	automate a = parse_postfix_format(postfixed, i);
	a.print_automate();
	return generate_regex_matcher(a);
}

regex_matcher regex::generate_regex_matcher(automate a) {

	map<long long, map<char, set<long long>>> trans_table = a.trans_table;
	map<long long, set<long long>> dfs_reachable;
	map<long long, bool> visited;
	
	auto dfs_routine = [&visited, &dfs_reachable, &trans_table](long long state, auto& dfs_routine) -> set<long long> {
		visited[state] = true;
		if (dfs_reachable.find(state) != dfs_reachable.end()) return dfs_reachable[state];
		set<long long> s{ state };
		for (long long state2 : trans_table[state][CAT_OP]) {
			set<long long> st;
			if (!visited[state2]) st = dfs_routine(state2, dfs_routine);
			else {
				st = dfs_reachable[state2];
				st.insert(state2);
			}
			s.insert(st.begin(), st.end());
		}
		dfs_reachable[state] = s;
		return s;
	};

	for (long long state : a.states) {
		if (visited[state]) continue;
		dfs_routine(state, dfs_routine);
	}

	set<long long> final_states;
	for (long long state : a.states) {
		bool is_final_state = false;
		for (long long fstate : a.final_states) {
			if (dfs_reachable[state].find(fstate) != dfs_reachable[state].end()) {
				is_final_state = true;
				break;
			}
		}
		if (is_final_state) final_states.insert(state);
	}

	for (long long state : a.states) {
		cout << state << " : ";
		for (long long s : dfs_reachable[state]) cout << s << " ";
		cout << endl;
	}

	map<set<long long>, map<char, set<long long>>> table;
	set<set<long long>> processed;
	stack<set<long long>> sets_stack;
	sets_stack.push(set<long long>{ a.start_state });
	while (!sets_stack.empty()) {
		set<long long> st = sets_stack.top();
		sets_stack.pop();
		processed.insert(st);
		set<long long> st2;
		for (long long state : st) {
			set<long long> k = dfs_reachable[state];
			st2.insert(k.begin(), k.end());
		}
		map<char, set<long long>> transitions;
		for (char c : a.alphabet) {
			if (c == CAT_OP) continue;
			set<long long> c_reachable;
			for (long long state : st2) {
				if (a.trans_table[state].find(c) != a.trans_table[state].end()) {
					c_reachable.insert(a.trans_table[state][c].begin(), a.trans_table[state][c].end());
				}
			}
			transitions[c] = c_reachable;
			if (processed.find(c_reachable) == processed.end()) sets_stack.push(c_reachable);
		}
		table[st] = transitions;
	}


	map<set<long long>, long long> sets_indexing;
	set<long long> res_states;
	long long index = 0;
	for (set<long long> s : processed) {
		sets_indexing[s] = index;
		res_states.insert(index);
		index++;
	}

	set<char> res_alphabet = a.alphabet;
	res_alphabet.erase(CAT_OP);

	long long res_start_state = sets_indexing[set<long long>{ a.start_state }];
	set<long long> res_final_states;
	for (pair<set<long long>, long long> p : sets_indexing) {
		bool is_final_state = false;
		for (long long fstate : final_states) {
			if (p.first.find(fstate) != p.first.end()) {
				is_final_state = true;
				break;
			}
		}
		if (is_final_state) res_final_states.insert(p.second);
	}

	for (pair<set<long long>, long long> p : sets_indexing) {
		for (long long s : p.first) cout << s << " ";
		cout << " : " << p.second << endl;
	}

	map<long long, map<char, long long>> res_trans_table;
	for (pair<set<long long>, map<char, set<long long>>> p1 : table) {
		long long state1 = sets_indexing[p1.first];
		for (pair<char, set<long long>> p2 : p1.second) {
			long long state2 = sets_indexing[p2.second];
			char c = p2.first;
			res_trans_table[state1][c] = state2;
		}
	}

	return regex_matcher(res_alphabet, res_states, res_start_state, res_final_states, res_trans_table);
}
