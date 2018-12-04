#pragma once
#include <string>
#include <tuple>
#include <deque>
#include <stack>
#include "automate.h"
#include "regex_instance.h"

#include <iostream>

//constexpr char CAT_OP = '\0';

namespace mini_regex
{
	using namespace std;
	regex_instance parse_expression(string s);
	regex_instance generate_regex_instance(automate a);
};

