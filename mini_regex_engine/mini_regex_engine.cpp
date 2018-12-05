// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <regex>
#include "mini_regex.h"

using namespace std;

string generate_random_regex(int length) {
	const char p_chars[] = { 'a', 'b', '*', '|' };
	regex_instance expression_matcher = mini_regex::parse_expression("a(a|ba|ca|bca)*");
	long long state = expression_matcher.get_start();
	set<long long> states_set;
	set<long long> final_states = expression_matcher.get_final_states();
	int nb_parenthesis = 0;
	string s = "";
	for (int i = 0; i < length; ++i) {		
		// randomise character untill you find non blocking one
		// the character is not '(' nor ')'
		bool last_char_is_par = s.size() > 0 && s[s.size() - 1] == '(';
		char c = p_chars[rand() % 4];
		char c2 = c == '*' ? 'b' : c == '|' ? 'c' : 'a';
		long long next_state = expression_matcher.next_state(state, c2);
		while (next_state == 0 || ((c == '|' || c == '*') && last_char_is_par) || (s.size() > 0 && s[s.size() - 1] == '|' && c == '*')) {
			c = p_chars[rand() % 4];
			c2 = c == '*' ? 'b' : c == '|' ? 'c' : 'a';
			next_state = expression_matcher.next_state(state, c2);
		}
		
		vector<char> par_v;
		if (nb_parenthesis > 0 && s[s.size() - 1] != '|' && s[s.size() - 1] != '(') par_v.push_back(')');
		if (s.size() != 0 && s[s.size() - 1] != '|') par_v.push_back('(');
		int r = rand() % (par_v.size() + 3);
		switch (par_v.size()) {
		case 0:
			// generate valid character
			s.push_back(c);
			state = next_state;
			break;
		case 1:
			if (r == 0) {
				if (par_v[0] == '(') {
					// open parentheses
					nb_parenthesis += 1;
					s.push_back('(');
					state = expression_matcher.get_start();
				}
				else {
					// close parenthese
					nb_parenthesis -= 1;
					s.push_back(')');
					state = expression_matcher.get_start();
					state = expression_matcher.next_state(state, 'a');
				}
			}
			else {
				// generate valid character
				s.push_back(c);
				state = next_state;
			}
			break;
		case 2:
			if (r == 0) {
				// close parenthese
				nb_parenthesis -= 1;
				s.push_back(')');
			}
			else if (r == 1) {
				// open parentheses
				nb_parenthesis += 1;
				s.push_back('(');
			} else {
				// choose a valid character
				s.push_back(c);
				state = next_state;
			}
			break;
		}
	}
	// delete all invalid characters at the end
	while (s.size() != 0 && s[s.size() - 1] == '|') s.pop_back();
	while (s.size() != 0 && s[s.size() - 1] == '(') {
		s.pop_back();
		nb_parenthesis--;
	}
	// close all remaining parentheses
	for (int i = 0; i < nb_parenthesis; ++i) s.push_back(')');
	return s;
}

string randomise_string(string chars, int size) {
	string res = "";
	if (chars.size() == 0) return "";
	for (int i = 0; i < size; ++i) res.push_back(chars[rand() % chars.size()]);
	return res;
}

void test_regex(regex_instance& r1, const regex& r2, string chars, int tries_number = 1000, int tries_sz_l = 3, int tries_sz_r = 50) {
	for (int i = 0; i < tries_number; ++i) {
		string txt = randomise_string(chars, tries_sz_l + rand() % (tries_sz_r - tries_sz_l));
		bool b1 = r1.match(txt);
		bool b2 = regex_match(txt, r2);
		if (b1 != b2) {
			cout << txt << " " << b1 << b2 << endl;
		}
	}
}

// this function will loop indefinitely if the regex implementation is correct
// and stops if not
void test_matches() {
	bool b = true;
	srand(1);
	while (b) {
		const char arr[] = { 'a', 'b' };
		int seed = rand();
		cout << "seed   : " << seed << endl;
		int length = 5 + rand() % 30;
		string s = generate_random_regex(length);
		cout << "regex  : " << s << endl;
		regex_instance r2 = mini_regex::parse_expression(s);
		regex r1(s, regex_constants::ECMAScript);
		try {
			long double avg_time = 0.;
			for (int i = 0; i < 500; ++i) {
				string s2 = randomise_string(string(arr), 5 + rand() % 50);
				bool b1 = regex_match(s2, r1);
				bool b2 = r2.match(s2);
				if (b1 != b2) {
					cout << s2 << " : " << b1 << " " << b2 << endl;
					b = false;
				}
			}
		}
		catch (regex_error err) {
			cout << err.what() << endl;
			for (int i = 0; i < 50; ++i) cout << "-";
			continue;
		}
		if (b) cout << "result : correct :)" << endl;
		for (int i = 0; i < 50; ++i) cout << "-";
		cout << endl;
	}
}

int main()
{
	string r = "(a|ba)*";
	regex_instance ri = mini_regex::parse_expression(r);
	ri.render_to_png("g");
	return 0;
	cout << "Infinite loop for testing : " << endl;
	test_matches();
	return 0;
}