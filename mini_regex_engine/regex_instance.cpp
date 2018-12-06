#include "pch.h"
#include "regex_instance.h"

regex_instance::regex_instance(set<char> alphabet_i, set<long long> states_i, long long start_state_i, set<long long> final_states_i, map<long long, map<char, long long>> trans_table_i)
	: alphabet(alphabet_i), states(states_i), start_state(start_state_i), final_states(final_states_i), trans_table(trans_table_i)
{
}

regex_instance::~regex_instance()
{
}

bool regex_instance::match(string s) {
	long long current_state = this->start_state;
	for (char& c : s) {
		if (this->trans_table[current_state].find(c) == this->trans_table[current_state].end()) 
			return false;
		current_state = trans_table[current_state][c];
	}
	return this->final_states.find(current_state) != this->final_states.end();
}

bool regex_instance::search(string s, string& res) {
	int i = 0;
	res = "";
	bool has_match = false;
	while (i < s.size()) {
		bool matched = false;
		int max_j = 0;
		int j = 0;
		long long state = this->start_state;
		if (this->final_states.find(state) != this->final_states.end()) {
			has_match = matched = true;
		}

		while (i + j < s.size()) {
			state = this->next_state(state, s[i + j]);
			j++;
			if (this->final_states.find(state) != this->final_states.end()) {
				has_match = matched = true;
				max_j = j;
			}
		}

		if (matched && max_j > res.size()) res = s.substr(i, max_j);
		i += 1;
	}

	return has_match;
}

void regex_instance::findall(string s, set<string>& res) {
	int i = 0;
	res.clear();
	if (this->final_states.find(this->start_state) != this->final_states.end()) res.insert("");
	while (i < s.size()) {
		bool matched = false;
		int max_j = 0;
		int j = 0;
		long long state = this->start_state;
		if (this->final_states.find(state) != this->final_states.end()) 
			matched = true;

		while (i + j < s.size()) {
			state = this->next_state(state, s[i + j]);
			j++;
			if (this->final_states.find(state) != this->final_states.end()) {
				matched = true;
				max_j = j;
			}
		}

		if (matched) res.insert(s.substr(i, max_j));
		i += 1;
	}
}

void regex_instance::print() {
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

long long regex_instance::next_state(long long state, char c) {
	if (this->states.find(state) == this->states.end() || this->alphabet.find(c) == this->alphabet.end()) {
		return -1;
	}
	return this->trans_table[state][c];
}

set<long long> regex_instance::get_Self_looping_states() {
	set<long long> st;
	for (long long state : this->states) {
		bool self_looping = true;
		for (pair<char, long long> p : this->trans_table[state]) {
			char& c = p.first;
			long long& state2 = p.second;
			if (c == CAT_OP) continue;
			if (state2 != state) {
				self_looping = false;
				break;
			}
			if (!self_looping) break;
		}
		if (self_looping) st.insert(state);
	}
	return st;
}

long long regex_instance::get_start() {
	return this->start_state;
}

set<long long> regex_instance::get_final_states() {
	return this->final_states;
}

set<long long> regex_instance::get_states() {
	return this->states;
}

long long regex_instance::trans_table_size() {
	long long n = 0;
	for (pair<long long, map<char, long long>> p : this->trans_table)
		n += p.second.size();
	return n;
}

string regex_instance::convert_to_dot_language() {
	string dot = "digraph {\n\trankdir=LR;\n";
	string start_shape = this->final_states.find(this->start_state) == this->final_states.end() ? "[shape=circle]" : "[shape=doublecircle]";
	dot += "\tstart" + start_shape + ";\n";
	for (long long s : this->states) {
		if (s == this->start_state) continue;
		string shape;
		if (this->final_states.find(s) != this->final_states.end()) shape = "[shape=doublecircle];\n";
		else shape = "[shape=circle];\n";
		dot += string("\t") + to_string(s) + shape;
	}
	for (pair<long long, map<char, long long>> p1 : this->trans_table) {
		string s1 = p1.first == this->start_state ? "start" : to_string(p1.first);
		for (pair<char, long long> p2 : p1.second) {
			char& c = p2.first;
			string s2 = p2.second == this->start_state ? "start" : to_string(p2.second);
			dot += string("\t") + s1 + " -> " + s2 + "[label=\"" + c + "\"];\n";
		}
	}
	dot += "}";
	return dot;
}

void regex_instance::render_to_png(string graph_name) {
	string dot = this->convert_to_dot_language();
	filebuf fb;
	fb.open(graph_name, std::ios::out);
	ostream os(&fb);
	os << dot;
	cout << dot << endl;
	fb.close();
	string cmd = "dot -Tpng " + graph_name + " -o " + graph_name + ".png";
	system(cmd.data());
	cmd = graph_name + ".png";
	system(cmd.data());
	remove(graph_name.data());
}
