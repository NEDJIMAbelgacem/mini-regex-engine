#include "pch.h"
#include "mini_regex.h"
#include <chrono>

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
	while (pos != s.size()) {
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

automate mini_regex::parse_postfix_format(string& s, long long& next_state_index) {

	char c = s.back();
	s.pop_back();
	automate a1(next_state_index), a2(next_state_index);
	switch (c) {
	case '*':
		a1 = parse_postfix_format(s, next_state_index);
		a1.star_automaton(next_state_index);
		return a1;
	case CAT_OP:
		a1 = parse_postfix_format(s, next_state_index);
		a2 = parse_postfix_format(s, next_state_index);
		a2.concat_automaton(a1, next_state_index);
		return a2;
	case '|':
		a1 = parse_postfix_format(s, next_state_index);
		a2 = parse_postfix_format(s, next_state_index);
		a1.union_automation(a2, next_state_index);
		return a1;
	case ')':
		a1 = parse_postfix_format(s, next_state_index);
		s.pop_back();
		return a1;
	default:
		automate a(next_state_index);
		a.set_start_state(next_state_index);
		a.add_final_state(next_state_index + 1);
		a.add_transition(next_state_index, c, next_state_index + 1);
		next_state_index += 2;
		return a;
	}
	return automate(next_state_index);
}

regex_matcher mini_regex::parse_expression(string s) {
	long long m1 = chrono::duration_cast<chrono::microseconds>(
		chrono::system_clock::now().time_since_epoch()
		).count();
	string postfixed = postfix_transform(s);
	long long m2 = chrono::duration_cast<chrono::microseconds>(
		chrono::system_clock::now().time_since_epoch()
		).count();
	cout << "postfixed format generation time : " << (m2 - m1) << endl;
	//cout << "postfix : " << postfixed << endl;
	long long i = 0;
	automate a = parse_postfix_format(postfixed, i);
	long long m3 = chrono::duration_cast<chrono::microseconds>(
		chrono::system_clock::now().time_since_epoch()
		).count();
	cout << "automate generation time : " << (m3 - m2) << endl;
	long long m4 = chrono::duration_cast<chrono::microseconds>(
		chrono::system_clock::now().time_since_epoch()
		).count();
	//a.print_automate();
	regex_matcher r = generate_regex_matcher(a);
	cout << "automate transformation time : " << (m4 - m3) << endl;
	return r;
}

regex_matcher mini_regex::generate_regex_matcher(automate a) {
	map<long long, map<char, set<long long>>>& trans_table = a.trans_table;

	// construct dirceted acyclic reduced graph of the states graph
	a.delete_0_length_circuits();
	map<long long, map<char, set<long long>>>& dag_transitions = a.trans_table;
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

	return regex_matcher(a.alphabet, res_states, res_start_state, res_final_states, res_transitions);
}
