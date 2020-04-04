#!/usr/bin/python

import sys
from sympy import sympify


def error(line, constring):
  print "Error, a constraint does not evaluate as true."
  print "Line: " + str(line)
  print "  Constraint: " + constring

def main():
  problemfile=open(sys.argv[1], "r")
  problemlines = problemfile.read().splitlines()
  problemfile.close()
  
  modelfile=open(sys.argv[2], "r")
  model = eval(modelfile.read())
  modelfile.close()
  
  satisfied = True
  
  for line_, constring in enumerate(problemlines):
    line = line_ + 1 
    if (constring != ""):
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
