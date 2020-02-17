
#include<iostream>
#include<vector>
#include<string>
//#include<assert.h>

#include<map>

#include <experimental/optional>
#include <dreal/dreal.h>

#include "sexpr_parser.hpp"

//#include <tuple>

using namespace std;
using namespace dreal;
using namespace dreal::drake;

namespace sp = sexpr_parser;

bool is_number(const std::string& s);
Formula tree2formula(Context&, sp::TreeNode, std::map<string, sp::TreeNode>);
Expression tree2expr(Context&, sp::TreeNode, std::map<string, sp::TreeNode>);
Variable tree2var(Context&, sp::TreeNode);
bool add_var(Context&, sp::TreeNode);
bool add_assert(Context&, sp::TreeNode);
bool add_cmd(Context&, sp::TreeNode);
void add_smt2(Context&, sp::TreeNode);

std::map<string, Variable> variables;

vector<sp::TreeNode> toparse;
vector<Formula> parsed;

std::map<string, std::function<symbolic::Expression(symbolic::Expression)>> smtfunc;

std::map<string, std::function<symbolic::Expression(void)>> smtconst;

void populate_smtmaps(void) {
  smtconst["smt_pi"] = symbolic::Expression::Pi;
  smtconst["smt_e"] = symbolic::Expression::E;
  smtfunc["log"] = symbolic::log;
  smtfunc["Abs"] = symbolic::abs;
  smtfunc["exp"] = symbolic::exp;
  smtfunc["sqrt"] = symbolic::sqrt;
  smtfunc["sin"] = symbolic::sin;
  smtfunc["cos"] = symbolic::cos;
  smtfunc["tan"] = symbolic::tan;
  smtfunc["asin"] = symbolic::asin;
  smtfunc["acos"] = symbolic::acos;
  smtfunc["atan"] = symbolic::atan;
//  smtfunc["atan2"] = symbolic::atan2;
  smtfunc["sinh"] = symbolic::sinh;
  smtfunc["cosh"] = symbolic::cosh;
  smtfunc["tanh"] = symbolic::tanh;
}

std::map<string, Variable::Type> str2vartype = {
  {"Real", Variable::Type::CONTINUOUS},
  {"Int", Variable::Type::INTEGER}
};

std::map<Variable::Type, string> vartype2str = {
  {Variable::Type::CONTINUOUS, "Real"},
  {Variable::Type::INTEGER, "Int"}
};
 
std::experimental::optional<dreal::Box> currentModel = {};

std::string success = "";
bool exitCmd = false;

int main(int argc, char* argv[]) {
  //  Config c;
  // xsstring smtinput;

  //  if (argc > 1) {
  //    double accuracy = strtod(argv[1], NULL);
  //    cerr << "accuracy is: " << accuracy << endl;
  //    c.mutable_precision() = accuracy;
  //  }
  
  Context s;

  // if (argc > 1) {
  //  double accuracy = strtod(argv[1], NULL);
  //  cerr << "accuracy is: " << accuracy << endl;
  //  s.mutable_config().mutable_precision() = accuracy;
  // }
  string smtinput;
  while (getline(cin, smtinput)) {
    // cerr << "Contents of the 'toparse' vector:" << endl;
    // for (sp::TreeNode x: toparse) cerr << x.ToSexpr() << endl;
    // cerr << "Contents of the 'parsed' vector:" << endl;
    // for (expr x: parsed) cerr << x << endl;
    // s.print_problem(cerr);
    //cerr << "Insert a new SMT2 instruction: " << endl;
    //getline(cin, smtinput);
    if (smtinput == "") {
      //cerr << "Empty string" << endl;
      continue;
    }
    if (smtinput.at(0) == ';') {
      continue;
    }
    sp::TreeNode smt2 = sp::Parse(smtinput)[0];
    toparse.push_back(smt2);
    add_smt2(s, smt2);
    cout << success;
    if (exitCmd) return 0;
  }
}

bool is_number(const std::string& s)
{
  std::string::const_iterator it = s.begin();
  //while ((it != s.end()) && (std::isdigit(*it)) || (*it == '.') || (*it == '-')) ++it;
  while (((it != s.end()) && (std::isdigit(*it))) || (*it == '.') || (*it == '-') || (*it == 'e')) ++it;
  return !s.empty() && it == s.end();
}

Formula tree2formula(Context& s, sp::TreeNode t, std::map<string, sp::TreeNode> let_vars) {
  string v = t.GetValue();
  string o = "";
  auto& c = t.GetChildren();
  if (c.empty() && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}
  if (! c.empty()) o = c[0].ToSexpr();
  if ((o=="") && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}

  if (o == "=") return (tree2expr(s, c[1], let_vars) == tree2expr(s, c[2], let_vars));
  if (o == "<") return (tree2expr(s, c[1], let_vars) < tree2expr(s, c[2], let_vars));
  if (o == "<=") return (tree2expr(s, c[1], let_vars) <= tree2expr(s, c[2], let_vars));
  if (o == ">") return (tree2expr(s, c[1], let_vars) > tree2expr(s, c[2], let_vars));
  if (o == ">=") return (tree2expr(s, c[1], let_vars) >= tree2expr(s, c[2], let_vars));

  /*
     Examples of let constructs:
     - (assert (let ((z (<= x 5.0))) z))
     - (assert (let ((z 5.0)) (<= x z)))
   */
  
  if (o == "let") {
    string val =  c[1].ToSexpr().substr(1, c[1].ToSexpr().length() - 2);
    sp::TreeNode let_code = sp::Parse(val)[0];
    auto &let_c = let_code.GetChildren();
    string let_var_name = let_c[0].ToSexpr();
    sp::TreeNode let_node = let_c[1];
    let_vars.insert(std::pair<string, sp::TreeNode>(let_var_name, let_node));
    //let_vars.insert(std::pair<string, sp::TreeNode>("__type_of_" + let_var_name, sp::Parse("(formula)")[0]));
    //cerr << "LET has been found. Value: " << let_vars.at(let_var_name).ToSexpr() << endl;
    //exit(1);
    return tree2formula(s, c[2], let_vars);
  }

  if (((o == "and") || (o == "or")) && (c.size() > 2)) {
    Formula f = tree2formula(s, c[1], let_vars);
    for (int i = 2; i < c.size() ; i++) {
      if (o == "and") f = f && tree2formula(s, c[i], let_vars);
      else if (o == "or") f = f || tree2formula(s, c[i], let_vars);
    }
    return f;
  }

  // if (o == "and") return (tree2formula(s, c[1], let_vars) && tree2formula(s, c[2], let_vars));
  // if (o == "or") return (tree2formula(s, c[1], let_vars) || tree2formula(s, c[2], let_vars));


  if (o == "not") return (!tree2formula(s, c[1], let_vars));

  if (v == "true") return Formula::True();
  if (v == "false") return Formula::False();

  if (isalpha(v.c_str()[0]) || v.c_str()[0] == '.' || v.c_str()[0] == '_')  {
    if (let_vars.find(v) != let_vars.end())
      return tree2formula(s, let_vars.at(v), let_vars);
  }
  cerr << "Unexpected values in parsing. Syntax error? [t = " << t.ToSexpr() << ", o = " << o << ", v = " << v << "]" << endl;
  //cout << "Unexpected values in parsing. Syntax error? [o = " << o << ", v = " << v << "]" << endl;
  exit(1);
}

Expression tree2expr(Context& s, sp::TreeNode t, std::map<string, sp::TreeNode> let_vars) {
  // if (!smtfunc.empty()) cerr "smtfunc is not empty, its size is " << smtfunc.size() << endl;  
  string v = t.GetValue();
  string o = "";
  auto& c = t.GetChildren();
  if (c.empty() && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}
  if (! c.empty()) o = c[0].ToSexpr();
  if ((o=="") && (v == "")) {cerr << "Unexpected empty value in parsing. Syntax error?" << endl; exit(1);}

  if (o == "let") {
    string val =  c[1].ToSexpr().substr(1, c[1].ToSexpr().length() - 2);
    sp::TreeNode let_code = sp::Parse(val)[0];
    auto &let_c = let_code.GetChildren();
    string let_var_name = let_c[0].ToSexpr();
    sp::TreeNode let_node = let_c[1];
    let_vars.insert(std::pair<string, sp::TreeNode>(let_var_name, let_node));
    //NOTE: The code below is old and should be useless.
    //let_vars.insert(std::pair<string, sp::TreeNode>("__type_of_" + let_var_name, sp::Parse("(formula)")[0]));
    //cerr << "LET has been found. Type key: " << ("__type_of_" + let_var_name) << endl;
    return tree2expr(s, c[2], let_vars);
  }

  if ((o == "-") && (c.size() == 2)) return ( - tree2expr(s, c[1], let_vars));

  if (((o == "+") || (o == "-") || (o == "*") || (o == "/")) && (c.size() > 2)) {
    Expression e = tree2expr(s, c[1], let_vars);
    for (int i = 2; i < c.size() ; i++) {
      if (o == "+") e = e + tree2expr(s, c[i], let_vars);
      else if (o == "-") e = e - tree2expr(s, c[i], let_vars);
      else if (o == "*") e = e * tree2expr(s, c[i], let_vars);
      else if (o == "/") e = e / tree2expr(s, c[i], let_vars);
    }
    return e;
  }


  
  // if (o == "+") return (tree2expr(s, c[1], let_vars) + tree2expr(s, c[2], let_vars));
  // cerr << "[c[1] = " << c[1].ToSexpr() << ", c[2] = " << c[2].ToSexpr() << ", len(c) =  " << c.size() << "]" << endl; exit(1); 
  // if ((o == "-") && (c.size() == 3)) return (tree2expr(s, c[1], let_vars) - tree2expr(s, c[2], let_vars));
  // if (o == "*") return (tree2expr(s, c[1], let_vars) * tree2expr(s, c[2], let_vars));
  // if (o == "/") return (tree2expr(s, c[1], let_vars) / tree2expr(s, c[2], let_vars));

  if (o == "^") return symbolic::pow(tree2expr(s, c[1], let_vars), tree2expr(s, c[2], let_vars));

  if (smtfunc.find(o) != smtfunc.end()) return smtfunc[o](tree2expr(s, c[1], let_vars));
  
  if (is_number(v)) return Expression(atof(v.c_str()));
  
  if (smtconst.find(v) != smtconst.end()) return smtconst[v]();
  
  if (isalpha(v.c_str()[0]) || v.c_str()[0] == '.' || v.c_str()[0] == '_') {
    if (let_vars.find(v) == let_vars.end())
      return Expression(variables[v]);
    return tree2expr(s, let_vars.at(v), let_vars);
  }
  cerr << "Unexpected values in parsing. Syntax error? [t = " << t.ToSexpr() << ", o = " << o << ", v = " << v << ", len(c) = " << c.size() << "]" << endl;
  exit(1);
}

bool add_var(Context& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  int type_pos = -1;
  if (c[0].ToSexpr() == "declare-const") type_pos = 2;
  if (c[0].ToSexpr() == "declare-fun") {
    if (c[2].ToSexpr() != "()") {
      cerr << "Error: incorrect or unsupported function definition." << endl;
      exit(1);
    }
    type_pos = 3;
  }
  if (type_pos > 0) {
    string x = c[1].ToSexpr();
    //cerr << "Adding variable " << x << endl;
    Variable v((x.c_str()), str2vartype[c[type_pos].ToSexpr()]);
    variables[x] = v;
    s.DeclareVariable(v);
    success = "success\n";
    return true;
  }
  return false;
}

bool add_assert(Context& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  if (c[0].ToSexpr() == "assert") {
    //cerr << "Assertion to be parsed:" << endl;
    //cerr << t.ToSexpr() << endl;
    Formula* e = new Formula;
    std::map<string, sp::TreeNode> let_vars;
    *e = tree2formula(s, c[1], let_vars);
    // FOR DEBUGGING:
    cerr << "Adding the following formula:" << endl;
    cerr << *e << endl;
    parsed.push_back(*e);
    s.Assert(*e);
    success = "success\n";
    return true;
  }
  return false;
}

bool add_cmd(Context& s, sp::TreeNode t) {
  auto& c = t.GetChildren();
  if (c[0].ToSexpr() == "set-option") {success = "success\n"; return true;}
  if (c[0].ToSexpr() == "set-logic") {success = "success\n"; return true;}
  if (c[0].ToSexpr() == "exit") {success = "success\n"; exitCmd = true; return true;}
  if (c[0].ToSexpr() == "push") {s.Push(1); success = "success\n"; return true;}
  if (c[0].ToSexpr() == "pop") {s.Pop(1); success = "success\n"; return true;}
  if (c[0].ToSexpr() == "check-sat") {
    // s.push();
    currentModel = s.CheckSat();
    string resMsg = (currentModel)?"sat":"unsat";
    cout << resMsg << endl;
    // s.pop();
    success = "";
    return true;
  }
  if (c[0].ToSexpr() == "get-model") {
    cout << "(model" << endl;
    std::map<string, Variable>::iterator it;
    for (it = variables.begin(); it != variables.end() ; ++it) {
      double value = (*currentModel)[it->second].lb();
      if (isinf(value)) value = (*currentModel)[it->second].ub();
      if (isinf(value)) value = 0;
      cout << "  (define-fun " << it->first << " () " << vartype2str[(it->second).get_type()] << endl;
      cout << "    " << value << ")" << endl;
    }
    cout << ")" << endl;
    success = "";
    return true;
  }
  if (c[0].ToSexpr() == "get-value") {
    string varname = c[1].GetChildren()[0].ToSexpr();
    double lb = (*currentModel)[variables[varname]].lb();
    double ub = (*currentModel)[variables[varname]].ub();
    if ((lb == ub) && isinf(lb)) cout << "Error: it seems lower and upper bound are equal and both infinity." << endl;  
    double value = (*currentModel)[variables[varname]].lb();
    if (isinf(value)) value = (*currentModel)[variables[varname]].ub();
    if (isinf(value)) value = 0;
    cout << "((" << varname << " " << value << "))" << endl;
    success = "";
    return true;
  }
  return false;
}

void add_smt2(Context& s, sp::TreeNode t) {
  if (smtfunc.empty()) populate_smtmaps();
  if (t.GetChildren().empty()) {cerr << "Error: unexpected smt2 command. Ignoring... (check the syntax)" << endl; return;}
  if (add_var(s, t)) return;
  if (add_assert(s, t)) return;
  if (add_cmd(s, t)) return;
  //cerr << "Error: unexpected smt2 command. Ignoring... (check the syntax)" << endl;
}


