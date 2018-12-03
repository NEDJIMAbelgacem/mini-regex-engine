// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include "mini_regex.h"
#include <regex>
#include <chrono>

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

int main()
{
	string re = "a*|b*";
	regex_instance reg1 = mini_regex::parse_expression(re);
	reg1.show_picture();
	return 0;
	// testing loop
	cout << "Infinite loop for testing : " << endl;
	bool b = true;
	srand(1);
	while (b) {
		const char arr[] = { 'a', 'b' };
		int seed = rand();
		cout << "seed : " << seed << endl;
		int length = 5 + rand() % 30;
		string s = generate_random_regex(length);
		cout << "regex : " << s << endl << endl;
		long long m1 = chrono::duration_cast<chrono::microseconds>(
			chrono::system_clock::now().time_since_epoch()
		).count();

		regex_instance r2 = mini_regex::parse_expression(s);

		long long m2 = chrono::duration_cast<chrono::microseconds>(
			chrono::system_clock::now().time_since_epoch()
		).count();

		cout << "my regex parsing time : " << (m2 - m1) << endl;

		regex r1(s, regex_constants::ECMAScript);
		long long m3 = chrono::duration_cast<chrono::microseconds>(
			chrono::system_clock::now().time_since_epoch()
		).count();
		cout << "standard regex parsing time : " << (m3 - m2) << endl;
		long long t1 = m2 - m1;
		long long t2 = m3 - m2;
		long double diff = ((long double)t1 / t2);
		cout << "mini_regex / std matcher generation : " << diff << endl;;
		try {
			long double avg_time = 0.;
			for (int i = 0; i < 500; ++i) {
				string s2 = "";
				for (int i = 0; i < rand() % (s.size() * 2); ++i) s2 += arr[rand() % 2];
				long long m1 = chrono::duration_cast<chrono::microseconds>(
					chrono::system_clock::now().time_since_epoch()
				).count();
				
				bool b1 = regex_search(s, r1);

				long long m2 = chrono::duration_cast<chrono::microseconds>(
					chrono::system_clock::now().time_since_epoch()
				).count();
				
				string res;
				bool b2 = r2.search(s, res);
				
				long long m3 = chrono::duration_cast<chrono::microseconds>(
					chrono::system_clock::now().time_since_epoch()
				).count();
				long long t1 = m2 - m1;
				long long t2 = m3 - m2;
				long double diff = ((double long) t2 / t1);
				avg_time += diff;
				//cout << "my regex match time / stardard regex match time : " << diff << endl;
				if (b1 != b2) {
					cout << s2 << " : " << b1 << " " << b2 << endl;
					b = false;
				}
			}
			avg_time /= 500.0;
			cout << "avg match time enhancement against the standard regex library : " << ((long double)avg_time / 100.0) << endl;
		}
		catch (regex_error err) {
			cout << err.what() << endl;
			continue;
		}
	}
	return 0;
}