#!/usr/bin/python

from sys import argv
from nltk import Tree

relmap = {}
relmap['='] = 'Eq'
relmap['!='] = 'Ne'
relmap['>'] = 'Gt'
relmap['<'] = 'Lt'
relmap['>='] = 'Ge'
relmap['<='] = 'Le'

#   'Rel', 'Eq', 'Ne', 'Lt', 'Le', 'Gt', 'Ge',
#    'Relational', 'Equality', 'Unequality', 'StrictLessThan', 'LessThan',
#    'StrictGreaterThan', 'GreaterThan',


def tree_to_infix(t):
  global relmap
  if isinstance(t, basestring):
    return t
  s = '('
  op = t.label()
  if (op in relmap):
    op = relmap[op]
    x = tree_to_infix(t[0])
    y = tree_to_infix(t[1])
    s += op + '(' + x + ', ' + y + ')'
  elif (op) in ['+', '-', '*', '/', '^']:
    if (len(t) == 1):
      s += op + ' ' + tree_to_infix(t[0])
    else:
      s += tree_to_infix(t[0])
      for o in t[1:]:
        s += ' ' + op + ' ' + tree_to_infix(o)
  elif (op == 'centropy'):
    x = tree_to_infix(t[0])
    y = tree_to_infix(t[1])
    z = '1e-20'
    if (len(t) > 2): z = tree_to_infix(t[2])
    return '(' + x + ' * (log((' + x + ' + ' + z + ') / (' + y + ' + ' + z + '))))'
  elif (op == 'signpower'):
    x = tree_to_infix(t[0])
    y = tree_to_infix(t[1])
    return 'ITE(' + x + ' >= 0, (' + x + ' ^ ' + y + '), (-(Abs(' + x + ') ^ ' + y + ')))'
  else:
    s += op + '(' + tree_to_infix(t[0]) + ')'
  s += ')'
  return s

def main():
  smtfile = open(argv[1], 'r')
  smtlines = smtfile.readlines()
  smtfile.close()
  
  objkeyword = '; obj_smt = '
  
  output = ''
  
  for num, line in enumerate(smtlines):
    # print "Line " + str(num + 1)
    if line.startswith('(assert'):
      # print 'Parsing formula: ' + line
      t = Tree.fromstring(line)
      output += tree_to_infix(t[0]) + '\n'
    elif line.startswith(objkeyword):
      t = Tree.fromstring(line[len(objkeyword):])
      output += "Obj: " + tree_to_infix(t) + '\n'
      
  
  sympyfile = open(argv[1].split('.smt2')[0] + '.sympy', 'w')
  print >>sympyfile, output
  sympyfile.close()

main()

