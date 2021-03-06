#include "pch.h"
#include "automate.h"

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

void automate::optional_automaton(long long& next_state_index) {
	for (long long fs : this->final_states) {
		this->add_transition(this->start_state, CAT_OP, next_state_index);
		this->add_transition(next_state_index, CAT_OP, fs);
	}
	next_state_index++;
}

void automate::plus_automaton(long long& next_next_state_index) {
	map<tuple<long long, char>, set<long long>> trans_table2;
	// make a copy of the current transitions table layout with different states indexing
	for (pair<tuple<long long, char>, set<long long>> p1 : this->trans_table) {
		long long& s1 = get<0>(p1.first);
		char& c = get<1>(p1.first);
		for (long long s2 : p1.second)
			trans_table2[make_pair(s1 + next_next_state_index, c)].insert(s2 + next_next_state_index);
	}
	// add the transition to the this automate transition table
	for (pair<tuple<long long, char>, set<long long>> p : trans_table2) {
		long long& s1 = get<0>(p.first);
		char& c = get<1>(p.first);
		for (long long s2 : p.second) 
			this->add_transition(s1, c, s2);
	}
	// define a looping point
	long long looping_point = this->start_state + next_next_state_index;
	// join the final states of the previous final states with the looping point
	for (long long s : this->final_states) {
		this->add_transition(s, CAT_OP, looping_point);
	}
	// do a partial iteration over the second part of the automaton
	long long indexing_bias = next_next_state_index;
	next_next_state_index *= 2;
	for (long long fs : this->final_states) {
		this->add_transition(fs + indexing_bias, CAT_OP, next_next_state_index);
		this->add_transition(next_next_state_index, CAT_OP, looping_point);
		next_next_state_index++;
	}
	this->final_states.clear();
	this->final_states.insert(looping_point);
}

automate::automate(long long& index) { 
	this->set_start_state(index);
	this->states.insert(index);
	index++;
}

automate::automate(abstract_syntax_tree* ast, long long& index) {
	if (ast == nullptr) return;

	automate left_a(ast->left, index);
	automate right_a(ast->right, index);

	switch (ast->type) {
	case abstract_syntax_tree::Star_OP:
		left_a.star_automaton(index);
		break;
	case abstract_syntax_tree::Optional_OP:
		left_a.optional_automaton(index);
		break;
	case abstract_syntax_tree::Plus_OP:
		left_a.plus_automaton(index);
		break;
	case abstract_syntax_tree::Concat_OP:
		left_a.concat_automaton(right_a, index);
		break;
	case abstract_syntax_tree::Union_OP:
		left_a.union_automation(right_a, index);
		break;
	case abstract_syntax_tree::Leaf:
		this->add_transition(index, ast->c, index + 1);
		this->set_start_state(index);
		this->add_final_state(index + 1);
		index += 2;
		break;
	}
	if (ast->type != abstract_syntax_tree::Leaf) {
		this->alphabet = left_a.alphabet;
		this->states = left_a.states;
		this->final_states = left_a.final_states;
		this->start_state = left_a.start_state;
		this->trans_table = left_a.trans_table;
	}
}

string automate::convert_to_dot_language() {
	string dot = "digraph {\nrankdir=LR;\n";
	map<long long, string> states_naming;
	map<long long, string> states_shapes;
	for (long long s : this->states) states_naming[s] = to_string(s);
	states_naming[this->start_state] = "start";
	for (long long s : this->states) states_shapes[s] = "circle";
	for (long long fs : this->final_states) states_shapes[fs] = "doublecircle";
	for (long long s : this->states) dot += "\t" + states_naming[s] + "[shape=" + states_shapes[s] + "];\n";
	for (pair<tuple<long long, char>, set<long long>> p : this->trans_table) {
		long long& s1 = get<0>(p.first);
		string c = "";
		if (get<1>(p.first) == '\0') c = "ep";
		else c = get<1>(p.first);
		for (long long s2 : p.second) {
			dot += "\t" + states_naming[s1] + " -> " + states_naming[s2] + "[label=\"" + c + "\"];\n";
		}
	}
	dot += "}";
	return dot;
}

void automate::render_automaton(string automaton_name) {
	string dot = this->convert_to_dot_language();
	filebuf fb;
	fb.open(automaton_name, std::ios::out);
	ostream os(&fb);
	os << dot;
	fb.close();
	string cmd = "dot -Tpng " + automaton_name + " -o " + automaton_name + ".png";
	system(cmd.data());
	cmd = automaton_name + ".png";
	system(cmd.data());
	remove(automaton_name.data());
}

automate::~automate()
{
}
