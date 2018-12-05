#include "pch.h"
#include "abstract_syntax_tree.h"

using namespace std;

abstract_syntax_tree::abstract_syntax_tree(char c)
{
	this->c = c;
	switch (c)
	{
	case '*':
		this->type = Star_OP;
		break;
	case '?':
		this->type = Optional_OP;
		break;
	case '\0':
		this->type = Concat_OP;
		break;
	case '|':
		this->type = Union_OP;
		break;
	default:
		this->type = Leaf;
		break;
	}
	this->left = nullptr;
	this->right = nullptr;
}

abstract_syntax_tree::~abstract_syntax_tree()
{
	delete this->right;
	delete this->left;
}

abstract_syntax_tree* abstract_syntax_tree::last_instruction(string& s) {
	if (s.size() == 0) return nullptr;
	abstract_syntax_tree* root = nullptr;
	char c = s.back();
	s.pop_back();
	switch (c) {
	case '\0':
	case '|':
	case '(':
		// error
		return nullptr;
		break;
	case '*':
		root = new abstract_syntax_tree(c);
		root->left = last_instruction(s);
		break;
	case '?':
		root = new abstract_syntax_tree(c);
		root->left = last_instruction(s);
		break;
	case ')':
		root = abstract_syntax_tree::construct_ast(s);
		if (s.size() == 0 || s.back() != '(') {
			// error
			return nullptr;
		}
		s.pop_back();
		break;
	default:
		root = new abstract_syntax_tree(c);
		break;
	}
	return root;
}

abstract_syntax_tree* abstract_syntax_tree::construct_ast(string& s) {
	abstract_syntax_tree* root = nullptr;
	abstract_syntax_tree* right = last_instruction(s);
	while (s.size() != 0 && s.back() != '(' && s.back() != '|') {
		char c = s.back();
		s.pop_back();
		switch (c) {
		case '\0':
			root = new abstract_syntax_tree(c);
			root->left = last_instruction(s);
			root->right = right;
			break;
		default:
			// errors
			break;
		}
		right = root;
	}
	if (s.size() != 0 && s.back() == '|') {
		root = new abstract_syntax_tree(s.back());
		s.pop_back();
		root->right = right;
		root->left = construct_ast(s);
		right = root;
	}
	return right;
};

void abstract_syntax_tree::convert_to_dot_language(string& dot) {
	string ptr_val = to_string((long long) this);
	if (this->left != nullptr) {
		string lptr_val = to_string((long long)this->left);
		dot += string("\t") + ptr_val + " -- " + lptr_val + ";\n";
	}
	if (this->right != nullptr) {
		string rptr_val = to_string((long long)this->right);
		dot += string("\t") + ptr_val + " -- " + rptr_val + ";\n";
	}
	if (this->left != nullptr) this->left->convert_to_dot_language(dot);
	if (this->right != nullptr) this->right->convert_to_dot_language(dot);
	string label = string("") + this->c;
	if (this->c == '\0') label = ".";
	else if (this->c == '*') label = "*";
	else if (this->c == '|') label = "\|";
	dot += string("\t") + ptr_val + "[shape=circle,label=\"" + label + "\"];\n";
}

void abstract_syntax_tree::render_ast(string tree_name) {
	string dot = "graph {\n";
	this->convert_to_dot_language(dot);
	dot += "}";
	cout << dot << endl;
	filebuf fb;
	fb.open(tree_name, std::ios::out);
	ostream os(&fb);
	os << dot;
	fb.close();
	string cmd = "dot -Tpng " + tree_name + " -o " + tree_name + ".png";
	system(cmd.data());
	cmd = tree_name + ".png";
	system(cmd.data());
	remove(tree_name.data());
}