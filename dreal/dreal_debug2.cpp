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

    Variable x1("x1", Variable::Type::CONTINUOUS);
    Variable x2("x2", Variable::Type::CONTINUOUS);
    Variable x3("x3", Variable::Type::CONTINUOUS);
    
    s.DeclareVariable(x1);
    s.DeclareVariable(x2);
    s.DeclareVariable(x3);
    
    auto low = -1000000000000000000;
    auto up  =  1000000000000000000;
    
    s.Assert(x1 >= -100);
    s.Assert(x1 <= 100);

    s.Assert(x2 >= low);
    s.Assert(x3 >= low);
    s.Assert(x2 <= up);
    s.Assert(x3 <= up);
        
    s.Assert(x1 + x2 + x3 == 99);
    
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
