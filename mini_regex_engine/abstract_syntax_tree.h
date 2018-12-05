#pragma once
#include <string>
#include <fstream>
#include <ostream>
#include <iostream>

using namespace std;

class abstract_syntax_tree
{
public:
	friend class automate;
	enum Type {Concat_OP, Union_OP, Star_OP, Optional_OP, Leaf};
	Type type;
	char c;
private:
	abstract_syntax_tree(char c);
	abstract_syntax_tree* left;
	abstract_syntax_tree* right;
public:
	static abstract_syntax_tree* last_instruction(string& s);
	static abstract_syntax_tree* construct_ast(string& s);
	void convert_to_dot_language(string& dot);
	void render_ast(string tree_name);
	~abstract_syntax_tree();
};
