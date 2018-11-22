// mini_regex_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "regex.h"

using namespace std;

int main()
{
	string regex;
	cout << "input regular expression : ";
	cin >> regex;
	regex_matcher rm = regex::parse_expression(regex);
	rm.print();
	cout << "input string to be processed or :q to exit" << endl;
	string s;
	while (cin >> s, s != ":q") {
		cout << "result : " << (rm.match(s) ? "matched" : "unmatched") << endl;
	}
	return 0;
}