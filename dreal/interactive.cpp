#include<iostream>
#include<vector>
#include<string>
#include<assert.h>

#include<map>

#include <dreal/dreal.h>

#include "sexpr_parser.hpp"

using namespace std;
using namespace dreal;

namespace sp = sexpr_parser;

bool is_number(const std::string& s);
expr tree2expr(solver&, sp::TreeNode);
void add_var(solver&, sp::TreeNode);
void add_assert(solver&, sp::TreeNode);
void add_cmd(solver&, sp::TreeNode);
void add_smt2(solver&, sp::TreeNode);

map<string, expr> variables;

vector<sp::TreeNode> toparse;
vector<expr> parsed;

int main(int argc, char* argv[]) {
  solver s;
  // xsstring smtinput;

  if (argc > 1) {
    double accuracy = strtod(argv[1], NULL);
    cout << "accuracy is: " << accuracy << endl;
    s.set_delta(accuracy);
  }

  while (true) {
    // cout << "Contents of the 'toparse' vector:" << endl;
    // for (sp::TreeNode x: toparse) cout << x.ToSexpr() << endl;
    // cout << "Contents of the 'parsed' vector:" << endl;
    // for (expr x: parsed) cout << x << endl;
    s.print_problem(cout);
    cout << "Insert a new SMT2 instruction: " << endl;
    string smtinput;
    getline(cin, smtinput);
    sp::TreeNode smt2 = sp::Parse(smtinput)[0];
    toparse.push_back(smt2);
    add_smt2(s, smt2);
  }
}

bool is_number(const std::string& s)
{
  std::string::const_iterator it = s.begin();
  //while ((it != s.end()) && (std::isdigit(*it)) || (*it == '.') || (*it == '-')) ++it;
  while (((it != s.end()) && (std::isdigit(*it))) || (*it == '.') || (*it == '-')) ++it;
  return !s.empty() && it == s.end();
}

expr tree2expr(solver& s, sp::TreeNode t) {
  string v = t.GetValue();
  string o = "";
  auto& c = t.GetChildren();
  if (c.empty() && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}
  if (! c.empty()) o = c[0].ToSexpr();
  if ((o=="") && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}

  if (o == "=") return (tree2expr(s, c[1]) == tree2expr(s, c[2]));
  if (o == "<") return (tree2expr(s, c[1]) < tree2expr(s, c[2]));
  if (o == "<=") return (tree2expr(s, c[1]) <= tree2expr(s, c[2]));
  if (o == ">") return (tree2expr(s, c[1]) > tree2expr(s, c[2]));
  if (o == ">=") return (tree2expr(s, c[1]) >= tree2expr(s, c[2]));
  if (o == "+") return (tree2expr(s, c[1]) + tree2expr(s, c[2]));
  if (o == "-") return (tree2expr(s, c[1]) - tree2expr(s, c[2]));
  if (o == "*") return (tree2expr(s, c[1]) * tree2expr(s, c[2]));
  if (o == "/") return (tree2expr(s, c[1]) / tree2expr(s, c[2]));
  if (o == "^") return pow(tree2expr(s, c[1]), tree2expr(s, c[2]));
  if (o == "and") return (tree2expr(s, c[1]) && tree2expr(s, c[2]));
  if (o == "or") return (tree2expr(s, c[1]) || tree2expr(s, c[2]));
  if (o == "not") return (!tree2expr(s, c[1]));

  if (v == "true") return s.T();
  if (v == "false") return s.F();
  if (is_number(v)) return s.num(atof(v.c_str()));
  if (isalpha(v.c_str()[0])) return variables[v];

  cout << "Unexpected values in parsing. Syntax error? [o = " << o << ", v = " << v << "]" << endl;
  exit(1);
}

void add_var(solver& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  if (c[0].ToSexpr() == "declare-const") {
    string x = c[1].ToSexpr();
    // cout << "Adding variable " << x << endl;
    variables[x] = s.var(x.c_str());
  }
}

void add_assert(solver& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  if (c[0].ToSexpr() == "assert") {
    expr* e = new expr;
    *e = tree2expr(s, c[1]);
    parsed.push_back(*e);
    //    cout << "Adding assertion: " << e << endl;
    s.add(*e);
  }
}

void add_cmd(solver& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  if (c[0].ToSexpr() == "push") {s.push(); return;}
  if (c[0].ToSexpr() == "pop") {s.pop(); return;}
  if (c[0].ToSexpr() == "reset") {s.reset(); return;}
  if (c[0].ToSexpr() == "check-sat") {
    s.push();
    string res = (s.check())?"sat":"unsat";
    cout << res << endl;
    s.pop();
    return;
  }
  if (c[0].ToSexpr() == "get-model") {s.print_model(); return;}
}

void add_smt2(solver& s, sp::TreeNode t) {
  if (t.GetChildren().empty()) {cout << "Error: unexpected smt2 command. Ignoring... (check the syntax)" << endl; return;}
  add_var(s, t);
  add_assert(s, t);
  add_cmd(s, t);
  cout << "Error: unexpected smt2 command. Ignoring... (check the syntax)" << endl;
}


