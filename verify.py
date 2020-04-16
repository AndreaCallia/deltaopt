#!/usr/bin/python

import sys
from sympy import sympify


objkeyword = 'Obj: '

def error(line, constring):
  print "Error, a constraint does not evaluate as true."
  print "Line: " + str(line)
  print "  Constraint: " + constring

def check_objective(objstring, model):
  #print "Going to parse: " + objstring
  obj = sympify(objstring)
  error = False
  if (not hasattr(obj.subs(model), 'is_real')):
    print "Error: The objective value not a legal expression."
    error = True
  if (obj.subs(model).is_real != True):
    print "Error: The objective value is not a real value."
    error = True
  if (error):
    print "Unevaluated objective: " + str(obj)
    print "Evaluated objective: " + str(obj.subs(model))
  return (not error)

def main():
  global objkeyword
  
  problemfile=open(sys.argv[1], "r")
  problemlines = problemfile.read().splitlines()
  problemfile.close()
  
  modelfile=open(sys.argv[2], "r")
  model = eval(modelfile.read())
  modelfile.close()
  
  satisfied = True
  
  for line, constring in enumerate(problemlines, 1):
    if (constring != ""):
      if (constring.startswith('Obj: ')):
        satisfied = check_objective(constring[len(objkeyword):], model)
      else:
        constraint = sympify(constring)
        if (not hasattr(constraint, 'subs')):
          if (constraint != True):
            error(line, constring)
            print "Constraint value: " + str(constraint)
            if (hasattr(constraint, 'subs')):
              print "Value after substitution: " + str(constraint.subs(model))
            satisfied = False
        elif (constraint.subs(model) != True):
          error(line, constring)
          print "  LHS Value: " + str(constraint.lhs.subs(model))
          print "  RHS Value: " + str(constraint.rhs.subs(model))
          print "  Constraint value: " + str(constraint.subs(model))
          satisfied = False

  if (satisfied == True):
    print "All constraints are satisfied by the model."

main()
