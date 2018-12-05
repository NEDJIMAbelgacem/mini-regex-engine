// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <regex>
#include "mini_regex.h"

using namespace std;

string generate_random_regex(int length) {
	const int tokens_count = 7;
	const char p_chars[] = { 'a', 'b', '*', '?', '|', '(', ')' };
	int nb_parenthesis = 0;
	string s = "";
	for (int i = 0; i < length; ++i) {		
		set<char> valid_tokens;
		for (int j = 0; j < tokens_count; ++j) valid_tokens.insert(p_chars[j]);
		if (s.size() == 0) {
			valid_tokens.erase('*');
			valid_tokens.erase('?');
			valid_tokens.erase('|');
			valid_tokens.erase(')');
		}
		if (s.size() != 0) {
			switch (s.back()) {
			case '(':
				valid_tokens.erase(')');
			case '*':
			case '?':
			case '|':
				valid_tokens.erase('*');
				valid_tokens.erase('?');
				valid_tokens.erase('|');
				break;
			}
		}
		if (nb_parenthesis <= 0) valid_tokens.erase(')');
		string valid_tokens_s = "";
		for (char c : valid_tokens) valid_tokens_s.push_back(c);
		s += valid_tokens_s[rand() % valid_tokens.size()];
		if (s.back() == '(') nb_parenthesis++;
		else if (s.back() == ')') nb_parenthesis--;
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
	//regex_instance ri = mini_regex::parse_expression(r);
	//ri.render_to_png("g");
	//return 0;
	cout << "Infinite loop for testing : " << endl;
	test_matches();
	return 0;
}