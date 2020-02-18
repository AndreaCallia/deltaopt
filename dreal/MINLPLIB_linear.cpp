//
//  test_dreal.cpp
//  
//
//  Created by Andrea Callia D'Iddio on 25/03/2018.
//

#include <iostream>
//#include <vector>
#include <string>

#include <experimental/optional>
#include <dreal/dreal.h>

using namespace std;
using namespace dreal;

std::experimental::optional<dreal::Box> currentModel = {};

void print_model(void);

int main(int argc, char* argv[]) {
    string t;
    Context s;

    Variable x2("x2", Variable::Type::CONTINUOUS);
    Variable x3("x3", Variable::Type::CONTINUOUS);
    Variable x4("x4", Variable::Type::CONTINUOUS);
    Variable x5("x5", Variable::Type::CONTINUOUS);
    Variable x6("x6", Variable::Type::CONTINUOUS);
    Variable x7("x7", Variable::Type::CONTINUOUS);
    Variable x8("x8", Variable::Type::CONTINUOUS);
    Variable x9("x9", Variable::Type::CONTINUOUS);
    Variable x10("x10", Variable::Type::CONTINUOUS);
    Variable x11("x11", Variable::Type::CONTINUOUS);
    Variable x12("x12", Variable::Type::CONTINUOUS);
    Variable x13("x13", Variable::Type::CONTINUOUS);
    Variable x14("x14", Variable::Type::CONTINUOUS);
    Variable x15("x15", Variable::Type::CONTINUOUS);
    Variable x16("x16", Variable::Type::CONTINUOUS);
    Variable x17("x17", Variable::Type::CONTINUOUS);
    Variable x18("x18", Variable::Type::CONTINUOUS);
    Variable x19("x19", Variable::Type::CONTINUOUS);
    Variable x20("x20", Variable::Type::CONTINUOUS);
    Variable x21("x21", Variable::Type::CONTINUOUS);
    Variable x22("x22", Variable::Type::CONTINUOUS);
    Variable x23("x23", Variable::Type::CONTINUOUS);
    Variable x24("x24", Variable::Type::CONTINUOUS);
    Variable x25("x25", Variable::Type::CONTINUOUS);
    
    s.DeclareVariable(x2);
    s.DeclareVariable(x3);
    s.DeclareVariable(x4);
    s.DeclareVariable(x5);
    s.DeclareVariable(x6);
    s.DeclareVariable(x7);
    s.DeclareVariable(x8);
    s.DeclareVariable(x9);
    s.DeclareVariable(x10);
    s.DeclareVariable(x11);
    s.DeclareVariable(x12);
    s.DeclareVariable(x13);
    s.DeclareVariable(x14);
    s.DeclareVariable(x15);
    s.DeclareVariable(x16);
    s.DeclareVariable(x17);
    s.DeclareVariable(x18);
    s.DeclareVariable(x19);
    s.DeclareVariable(x20);
    s.DeclareVariable(x21);
    s.DeclareVariable(x22);
    s.DeclareVariable(x23);
    s.DeclareVariable(x24);
    s.DeclareVariable(x25);
    
    s.Assert(x2 >= -100);
    s.Assert(x3 >= -100);
    s.Assert(x4 >= -100);
    s.Assert(x5 >= -100);
    s.Assert(x6 >= -100);
    s.Assert(x7 >= -100);
    s.Assert(x8 >= -100);
    s.Assert(x9 >= -100);
    s.Assert(x10 >= -100);
    s.Assert(x11 >= -100);
    s.Assert(x12 >= -100);
    s.Assert(x13 >= -100);
    s.Assert(x14 >= -100);
    s.Assert(x15 >= -100);
    s.Assert(x16 >= -100);
    s.Assert(x17 >= -100);
    s.Assert(x18 >= -100);
    s.Assert(x19 >= -100);
    s.Assert(x20 >= -100);
    s.Assert(x21 >= -100);

    s.Assert(x2 <= 100);
    s.Assert(x3 <= 100);
    s.Assert(x4 <= 100);
    s.Assert(x5 <= 100);
    s.Assert(x6 <= 100);
    s.Assert(x7 <= 100);
    s.Assert(x8 <= 100);
    s.Assert(x9 <= 100);
    s.Assert(x10 <= 100);
    s.Assert(x11 <= 100);
    s.Assert(x12 <= 100);
    s.Assert(x13 <= 100);
    s.Assert(x14 <= 100);
    s.Assert(x15 <= 100);
    s.Assert(x16 <= 100);
    s.Assert(x17 <= 100);
    s.Assert(x18 <= 100);
    s.Assert(x19 <= 100);
    s.Assert(x20 <= 100);
    s.Assert(x21 <= 100);
    
    s.Assert(x2 + x22 + 85*x23 + 76*x24 + 44*x25 == 99);
    s.Assert(x3 + x22 + 82*x23 + 78*x24 + 42*x25 == 93);
    s.Assert(x4 + x22 + 75*x23 + 73*x24 + 42*x25 == 99);
    s.Assert(x5 + x22 + 74*x23 + 72*x24 + 44*x25 == 97);
    s.Assert(x6 + x22 + 76*x23 + 73*x24 + 43*x25 == 90);
    s.Assert(x7 + x22 + 74*x23 + 69*x24 + 46*x25 == 96);
    s.Assert(x8 + x22 + 73*x23 + 69*x24 + 46*x25 == 93);
    s.Assert(x9 + x22 + 96*x23 + 80*x24 + 36*x25 == 130);
    s.Assert(x10 + x22 + 93*x23 + 78*x24 + 36*x25 == 118);
    s.Assert(x11 + x22 + 70*x23 + 73*x24 + 37*x25 == 88);
    s.Assert(x12 + x22 + 82*x23 + 71*x24 + 46*x25 == 89);
    s.Assert(x13 + x22 + 80*x23 + 72*x24 + 45*x25 == 93);
    s.Assert(x14 + x22 + 77*x23 + 76*x24 + 42*x25 == 94);
    s.Assert(x15 + x22 + 67*x23 + 76*x24 + 50*x25 == 75);
    s.Assert(x16 + x22 + 82*x23 + 70*x24 + 48*x25 == 84);
    s.Assert(x17 + x22 + 76*x23 + 76*x24 + 41*x25 == 91);
    s.Assert(x18 + x22 + 74*x23 + 78*x24 + 31*x25 == 100);
    s.Assert(x19 + x22 + 71*x23 + 80*x24 + 29*x25 == 98);
    s.Assert(x20 + x22 + 70*x23 + 83*x24 + 39*x25 == 101);
    s.Assert(x21 + x22 + 64*x23 + 79*x24 + 38*x25 == 80);
    
    currentModel = s.CheckSat();
    if (currentModel) cout << "SAT" << endl;
    else cout << "UNSAT" << endl;
    if (currentModel) print_model();
    return 0;
}

void print_model(void) {
    cout << "Model:" << endl;
    cout << *currentModel << endl;
    
    cout << "\nDict encoding:" << endl;
    
    std::vector<Variable>variables = (*currentModel).variables();
    
    std::vector<Variable>::iterator it;
    
    cout << "{";
    for (it = variables.begin(); it != variables.end() ; ++it) {
      //cout << *it << ": " << (*currentModel)[*it] << endl;
      double value = (*currentModel)[*it].lb();
      if (isinf(value)) value = (*currentModel)[*it].ub();
      if (isinf(value)) value = 0;
      cout << "\"" << *it << "\": " << value << ", ";
    }
    cout << "}" << endl;

//    for (it = (*currentModel).variables().begin(); it != (*currentModel).variables().end() ; ++it) {
//      cout << "blabla" << endl;
//    }

    /*
    std::map<string, Variable>::iterator it;
    for (it = variables.begin(); it != variables.end() ; ++it) {
      double value = (*currentModel)[it->second].lb();
      if (isinf(value)) value = (*currentModel)[it->second].ub();
      if (isinf(value)) value = 0;
      cout << it->first << ": " << value << endl;
    }
    */
}
