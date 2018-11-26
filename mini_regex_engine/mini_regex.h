#pragma once
#include <string>
#include <tuple>
#include <deque>
#include <stack>
#include "automate.h"
#include "regex_matcher.h"

#include <iostream>

//constexpr char CAT_OP = '\0';

namespace mini_regex
{
	using namespace std;

	automate union_regex_automates(const automate& a1, const automate& a2, long long& next_state_index);
	automate concat_regex_automates(const automate& a1, const automate& a2);
	string postfix_transform(string s);
	automate parse_postfix_format(string& s, long long& next_state_index);
	regex_matcher parse_expression(string s);
	regex_matcher generate_regex_matcher(automate a);
};

