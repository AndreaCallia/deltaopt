#!/usr/bin/python

from sys import argv
from nltk import Tree


def tree_to_infix(t):
  if isinstance(t, basestring):
    return t
  s = '('
  if (t.label()) in ['+', '-', '*', '/', '^', '=', '>', '<', '>=', '<=']:
    if (len(t) == 1):
      s += t.label() + ' ' + tree_to_infix(t[0])
    else:
      s += tree_to_infix(t[0])
      for o in t[1:]:
        s += ' ' + t.label() + ' ' + tree_to_infix(o)
  elif (t.label() == 'centropy'):
    x = tree_to_infix(t[0])
    y = tree_to_infix(t[1])
    z = '1e-20'
    if (len(t) > 2): z = tree_to_infix(t[2])
    return '(' + x + ' * (log((' + x + ' + ' + z + ') / (' + y + ' + ' + z + '))))'
  elif (t.label() == 'signpower'):
    x = tree_to_infix(t[0])
    y = tree_to_infix(t[1])
    return 'ITE(' + x + ' >= 0, (' + x + ' ^ ' + y + '), (-(Abs(' + x + ') ^ ' + y + ')))'
  else:
    s += t.label() + '(' + tree_to_infix(t[0]) + ')'
  s += ')'
  return s

def main():
  smtfile = open(argv[1], 'r')
  smtlines = smtfile.readlines()
  smtfile.close()
  
  output = ''
  
  for num, line in enumerate(smtlines):
    # print "Line " + str(num + 1)
    if line.startswith('(assert'):
      t = Tree.fromstring(line)
      output += tree_to_infix(t[0]) + '\n'
  
  sympyfile = open(argv[1].split('.smt2')[0] + '.sympy', 'w')
  print >>sympyfile, output
  sympyfile.close()

main()


# Tree.fromstring("(smtcode " + f + " )")
