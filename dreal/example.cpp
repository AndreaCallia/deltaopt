#include<iostream>
#include<vector>
#include<assert.h>

#include <dreal/dreal.h>

using namespace std;
using namespace dreal;

int main(int argc, char* argv[]) {
  solver s;
  expr x = s.var("x");
  s.add(x>0);
  s.add(x<5);
  //s.solve();
  s.push();
  s.add(x>10);
  //s.solve();
  s.pop();
  s.solve();
  return 0;
}

/*
  
  while (true)
    {
      cout << "Insert a lower bound for x: " << endl;
      cin >> lb;
      if (lb == -1234) break;
      s.add(x > lb);
      if (s.check()) s.print_model();
      cout << "Insert an upper bound for x: " << endl;
      cin >> ub;
      if (ub == -1234) break;
      s.add(x < ub);
      if (s.check()) s.print_model();
    }
  return 0;
}

*/

