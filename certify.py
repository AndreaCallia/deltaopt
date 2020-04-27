#!/usr/bin/python

import sys
from sympy import sympify

out = ''

objkeyword = 'Obj: '

def error(line, constring):
  global out
  out += "Error, a constraint does not evaluate as true.\n"
  out += "Line: " + str(line) + "\n"
  out += "  Constraint: " + constring + "\n"

def is_real_rec(exp, model, level):
  global out
  if (hasattr(exp, 'args') == False):
    return True
  if (exp.subs(model).is_real != True):
    extra = ''
    if (level > 0): extra = "a subexpression in "
    out += "Error: The value of " + extra + "the objective is not a real value.\n"
    out += "Unevaluated expression: " + str(exp) + "\n"
    out += "Evaluated expression: " + str(exp.subs(model)) + "\n"
    return False
  result = True
  for subexp in exp.args:
    result = is_real_rec(subexp, model, level + 1)
  return result

def is_objective_real(objstring, model):
  global out
  #out += "Going to parse: " + objstring
  obj = sympify(objstring)
  error = False
  if (not hasattr(obj.subs(model), 'is_real')):
    out += "Error: The objective value is not a legal expression.\n"
    out += "Unevaluated objective: " + str(obj) + "\n"
    out += "Evaluated objective: " + str(obj.subs(model)) + "\n"
    return False
  return is_real_rec(obj, model, 0)

def lhs(f):
  if (hasattr(f, 'lhs')):
    return f.lhs
  return f.args[0].lhs

def rhs(f):
  if (hasattr(f, 'rhs')):
    return f.rhs
  return f.args[0].rhs

def main():
  global objkeyword
  global out
  
  problemfile=open(sys.argv[1], "r")
  problemlines = problemfile.read().splitlines()
  problemfile.close()
  
  modelfile=open(sys.argv[2], "r")
  model = eval(modelfile.read())
  modelfile.close()
  
  delta = 0
  
  if (len(sys.argv) > 3):
    delta = float(sys.argv[3])
    
  model['delta'] = delta
  
  satisfied = True
  
  for line, constring in enumerate(problemlines, 1):
    if (constring != ""):
      if (constring.startswith('Obj: ')):
        if (is_objective_real(constring[len(objkeyword):], model) != True): satisfied = False
      else:
        constraint = sympify(constring)
        if (not hasattr(constraint, 'subs')):
          if (constraint != True):
            error(line, constring)
            out += "Constraint value: " + str(constraint) + "\n"
            if (hasattr(constraint, 'subs')):
              out += "Value after substitution: " + str(constraint.subs(model)) + "\n"
            satisfied = False
        elif (constraint.subs(model) != True):
          error(line, constring)
          out += "  LHS Value: " + str(lhs(constraint).subs(model)) + "\n"
          out += "  RHS Value: " + str(rhs(constraint).subs(model)) + "\n"
          out += "  Constraint value: " + str(constraint.subs(model)) + "\n"
          satisfied = False
  
  benchname = sys.argv[1][:sys.argv[1].index('.sympy')]
  if (satisfied == True):
    print "Benchmark " + benchname + ": all constraints are satisfied by the model."
  else:
    print "Benchmark " + benchname + ": some constraints are not satisfied by the model."
  if (out != ''): print out

main()
