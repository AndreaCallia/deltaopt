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

int main(int argc, char* argv[]) {
    string t;
    Context s;
    Variable x("x", Variable::Type::CONTINUOUS);
    s.DeclareVariable(x);
    s.Assert(x <= 5);
    auto res = s.CheckSat();
    if (res) cout << "SAT" << endl;
    else cout << "UNSAT" << endl;
    cin >> t;
    return 0;
}
