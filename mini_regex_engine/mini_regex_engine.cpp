// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "mini_regex.h"
#include <regex>

using namespace std;

string generate_random_regex(int length) {
	const char p_chars[] = { 'a', 'b', '*', '|' };
	regex_matcher expression_matcher = mini_regex::parse_expression("a(a|ba|ca|bca)*");
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
		while (next_state == 0 || ((c == '|' || c == '*') && last_char_is_par)) {
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

int main()
{
	// testing loop
	cout << "Infinite loop for testing : " << endl;
	bool b = true;
	while (b) {
		const char arr[] = { 'a', 'b' };
		int seed = rand();
		cout << "seed : " << seed << endl;
		srand(rand());
		int length = rand() % 255;
		string s = generate_random_regex(length);
		cout << "regex : " << s << endl << endl;
		regex r1(s, regex_constants::ECMAScript);
		regex_matcher r2 = mini_regex::parse_expression(s);
		for (int i = 0; i < 2000; ++i) {
			string s2 = "";
			for (int i = 0; i < rand() % (s.size() * 2); ++i) s2 += arr[rand() % 2];
			bool b1 = regex_match(s2, r1);
			bool b2 = r2.match(s2);
			if (b1 != b2) {
				cout << s2 << " : " << b1 << " " << b2 << endl;
				b = false;
			}
		}
	}
	return 0;
}