// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <regex>
#include "mini_regex.h"

using namespace std;

string generate_random_regex(int length) {
	const int tokens_count = 8;
	const char p_chars[] = { 'a', 'b', 'c', '*', '?', '+', '|', '(', ')' };
	int nb_parenthesis = 0;
	string s = "";
	for (int i = 0; i < length; ++i) {		
		set<char> valid_tokens;
		for (int j = 0; j < tokens_count; ++j) valid_tokens.insert(p_chars[j]);
		if (s.size() == 0) {
			valid_tokens.erase('*');
			valid_tokens.erase('?');
			valid_tokens.erase('+');
			valid_tokens.erase('|');
			valid_tokens.erase(')');
		}
		if (s.size() != 0) {
			switch (s.back()) {
			case '(':
			case '|':
				valid_tokens.erase(')');
				valid_tokens.erase('|');
			case '*':
			case '?':
			case '+':
				valid_tokens.erase('*');
				valid_tokens.erase('?');
				valid_tokens.erase('+');
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
	bool popped_char = true;
	while (popped_char) {
		popped_char = false;
		while (s.size() != 0 && s.back() == '|') {
			s.pop_back();
			popped_char = true;
		}
		while (s.size() != 0 && s[s.size() - 1] == '(') {
			s.pop_back();
			popped_char = true;
			nb_parenthesis--;
		}
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

bool test_regex(regex_instance& r1, const regex& r2, string chars, int tries_number = 1000, int tries_sz_l = 3, int tries_sz_r = 50) {
	bool correct = true;
	for (int i = 0; i < tries_number; ++i) {
		string txt = randomise_string(chars, tries_sz_l + rand() % (tries_sz_r - tries_sz_l));
		bool b1 = r1.match(txt);
		bool b2 = regex_match(txt, r2);
		if (b1 != b2) {
			cout << txt << " " << b1 << b2 << endl;
			correct = false;
		}
	}
	return correct;
}

bool test_exp(string exp, string chars, int tries_number = 1000, int tries_sz_l = 3, int tries_sz_r = 50) {
	regex_instance r1 = mini_regex::parse_expression(exp);
	regex r2(exp, regex_constants::ECMAScript);
	return test_regex(r1, r2, chars, tries_number, tries_sz_l, tries_sz_r);
}

// test according to a certain seed
bool test_seed(int seed) {
	for (int i = 0; i < 50; ++i) cout << "-";
	cout << endl;
	cout << "seed   : " << seed << endl;
	srand(seed);

	const char arr[] = { 'a', 'b', 'c' };
	int length = 5 + rand() % 30;
	string s = generate_random_regex(length);
	cout << "regex  : " << s << endl;

	regex_instance r1 = mini_regex::parse_expression(s);
	bool b = true;
	try {
		regex r2(s, regex_constants::ECMAScript);
		b = test_regex(r1, r2, string(arr));
	}
	catch (regex_error err) {
		cout << err.what() << endl;
	}
	if (b) cout << "result : correct :)" << endl;
	return b;
}

set<int> seeds_set;

// this function will loop indefinitely if the regex implementation is correct
// and stops if not
void test_matches() {
	bool b = true;
	while (b) {
		int seed = rand();
		if (seeds_set.find(seed) != seeds_set.end()) {
			while (seeds_set.find(seed) != seeds_set.end()) seed = rand();
		}
		seeds_set.insert(seed);
		b = test_seed(seed);
		if (!b) break;
	}
}

bool test_expression_with_seed(string exp, int seed) {
	for (int i = 0; i < 50; ++i) cout << "-";
	cout << endl;
	cout << "seed   : " << seed << endl;
	srand(seed);

	const char arr[] = { 'a', 'b', 'c' };
	int length = 5 + rand() % 30;
	string s = generate_random_regex(length);
	cout << "regex  : " << s << endl;

	regex_instance r1 = mini_regex::parse_expression(exp);
	regex r2(exp, regex_constants::ECMAScript);
	bool b = true;
	try {
		b = test_regex(r1, r2, string(arr), 100000);
	}
	catch (regex_error err) {
		cout << err.what() << endl;
	}
	if (b) cout << "result : correct :)" << endl;
	else cout << "result : incorrect :(" << endl;
	return b;
}

int main()
{
	cout << "Infinite loop for testing : " << endl;
	test_matches();
	return 0;
}