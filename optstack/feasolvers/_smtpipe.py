# import sympy.codegen.ast
from .. import utils
import sys
import math
import subprocess
# from sympy import simplify
from sympy import sympify, Integer, Rational
from sympy import Rational
from sympy import Function
from nltk import Tree

obj_keyword = ";obj_smt="

enablerational=False
delta = 0.001

solver_name = ""

solver_in = None
solver_out = None
solver_err = None
solver_process = None

objective = None
objstring = None

rounding = None

nonlinear = ["abs", "exp", "sin"]

model = {}

def check_formula(f):
  try:
    Tree.fromstring("(smtcode " + f + " )")
  except ValueError as e:
    utils.error(utils.UNEXPECTED_ERROR, "feasibility: The " + solver_name + " solver found a parsing error:\n" + e.message + "\nERROR")
    
def get_variables(f):
  global model
  for cmd in Tree.fromstring("(smtcode " + f + " )"):
    try:
      if (cmd.label() in ["declare-const", "declare-fun"]): model[cmd[0]] = None # print "Variable: " + cmd[0]
    except AttributeError:
      pass

def send_smt2(s):
  global solver_process
  s = " ".join(s.split()) # s.replace("\n", " ").replace('  ', ' ')
  # print ("sending: " + s)
  solver_process.stdin.write(s + "\n")
  solver_process.stdin.flush()
  
def receive_smt2():
  global solver_process
  return solver_process.stdout.readline().strip()

def check_success():
  out = receive_smt2()
  # print "out is: " + out
  if (out != "success") : utils.error(utils.UNEXPECTED_ERROR, "feasibility: The " + solver_name + " solver answers:\n" + out + "\nERROR")

def tree_to_infix(t):
  global nonlinear
  if isinstance(t, basestring):
    return t
  s = '('
  if (t.label()) in ['+', '-', '*', '/', '^']:
    if (len(t) == 1):
      s += t.label() + ' ' + tree_to_infix(t[0])
    else:
      s += tree_to_infix(t[0])
      for o in t[1:]:
        s += ' ' + t.label() + ' ' + tree_to_infix(o)
#  elif (t.label()) in nonlinear:
#    s += t.label() + "(" + tree_to_infix(t[0])
#    for o in t[1:]:
#      s += ', ' + tree_to_infix(o)
#    s += ")"
  else:
    s += t.label() + '(' + tree_to_infix(t[0]) + ')'
  s += ')'
  return s

def load_input(f):
  for cmd in Tree.fromstring("(smtcode " + f + " )"):
    try:
      if (cmd.label() in ["declare-const", "declare-fun", "assert"]):
        send_smt2(str(cmd))
        check_success()
    except AttributeError:
      pass

def obtain_model():
  global enablerational
  for x in model.keys():
    send_smt2("(get-value (" + x + "))")
    model[x] = sympify(Tree.fromstring(receive_smt2())[0][0], evaluate=False, rational=enablerational)
    # model[x] = simplify(Tree.fromstring(receive_smt2())[0][0])

def configure(inpcode, options):
  verbprint = utils.verbprint
  global solver_name
  global solver_in
  global solver_out
  global solver_err
  global solver_process
  global objective
  global objstring
  global enablerational
  global rounding
  global delta
  solver_name = options["smtpipe"].split("/")[-1]
  solver_cmd = options["smtpipe"]
  solver_logic = options["pipe_logic"]
  rounding = 4
  verbprint(2, "Setting rounding: " + str(rounding), True)
  verbprint(2, "Parsing the objective...", False)
  for line in inpcode.splitlines():
    if "".join(line.split()).startswith(obj_keyword):
      objstring = line[line.index("=") + 1:]
      verbprint(2, "\nThe objective is: " + objstring, True)
      verbprint(2, "Infix notation: " + str(tree_to_infix(Tree.fromstring(objstring))), True)
      objective = sympify(tree_to_infix(Tree.fromstring(objstring)), evaluate=False, rational=enablerational)
      objective.replace(Function("centropy"), lambda x, y, z = 1e-20 : x * sympify("log")((x + z) / (y + z)))
      objective.replace(Function("signpower"), lambda x, y : sympify("ITE")(x >= 0, x**y, -1*(sympify("Abs")(x)**y)))
  verbprint(2, "done.", True)
  inpcode.replace("\n", " ")
  verbprint(2, "Parsing the input...", False)
  check_formula(inpcode)
  get_variables(inpcode)
  verbprint(2, "done.", True)
  verbprint(2, "Initializing the solver...", False)
  solver_process = subprocess.Popen([solver_cmd], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  send_smt2("(set-logic " + solver_logic + ")")
  check_success()
  verbprint(2, "done.", True)
  verbprint(2, "Loading the input file in the solver...", False)
  load_input(inpcode)
  verbprint(2, "done.", True)

def check_feasibility():
  unknown = False
  verbprint = utils.verbprint
  utils.stats_create("Solving time", 0.0)
  utils.stats_create("Solver calls", 0)
  verbprint(3, "Checking satisfiability...", False)
  utils.time_start("Solving time")
  send_smt2("(check-sat)")
  result = receive_smt2()
  utils.stats_update("Solving time", utils.time_take("Solving time"))
  utils.stats_update("Solver calls", 1)
  verbprint(3, "done.", True)
  # verbprint(3, "result is: " + result, True)
  if (result == "unsat"):
    return None
  obtain_model()
  # print "Model:"
  # print model
  # print "Model:"
  # for x in model.keys():
  #   print x + ": " + str(model[x])
  # print "Objective:",
  # print objective.subs(model)
  # exit(0)
  return model

def push(n):
  for i in range(0, n):
    send_smt2("(push)")
    check_success()

def pop(n):
  for i in range(0, n):
    send_smt2("(pop)")
    check_success()

def model_vars(model):
  return model.keys()

def get_model_value(model, x):
  return str(model[x])

def get_obj_value(model):
  global objective
  return str(objective.subs(model))
  
def get_float(model, x, accuracy):
  val = model[x]
  decdigits = int(-math.log(accuracy, 10)) + 2
  return val

def get_obj_float(model, accuracy):
  global objective
  return objective.subs(model)

def negate_obj():
  global objective
  global objstring
  objective = - objective
  objstring = "(- 0 " + objstring + ")"
  
def add_obj_lt(v):
  verbprint = utils.verbprint
  global objstring
  global rounding
  global objective
  if (type(v) in [Integer, Rational]): round_v = v
  else:
    round_v = sympify(str(v.round(rounding)), rational=enablerational)
    verbprint(2, "Rounded value: " + str(round_v), True)
    if (round_v < v):
      verbprint(2, "It was rounded down. Adding " + str(10**(-rounding)), True)
      round_v = sympify(str((v + 10**(-rounding)).round(rounding)), rational=enablerational)
      verbprint(2, "New value: " +  str(round_v), True)
  if (type(round_v) == Rational): round_v = "(/ " + str(round_v.as_numer_denom()[0]) + " " + str(round_v.as_numer_denom()[1]) + ")"
  # send_smt2("(push)")
  # check_success()
  send_smt2("(assert (< " + objstring + " " + str(round_v) + "))")
  check_success()



'''

from pysmt.shortcuts import Or, Symbol, Solver, And, Real, LE, GE, get_env, ToReal
from pysmt.smtlib.solver import SmtLibSolver
from pysmt.smtlib.parser import SmtLibParser, get_formula, Tokenizer
from pysmt.typing import REAL #, INT
from pysmt.exceptions import SolverReturnedUnknownResultError


def get_float(model, x, accuracy):
  val = model.get_py_value(Symbol(x, REAL))
  decdigits = int(-math.log(accuracy, 10)) + 2
  return float(val)

def get_obj_float(model, accuracy):
  modeldict = dict([(x, ToReal(v)) for (x, v) in model])
  res = obj_fnode.substitute(modeldict)  
  # print "\nThe value to be returned is: " + str(res)
  # quit()
  # res = res.simplify()
  # return simplify(str(res)) 
  print "\nThe value to be returned is: " + str(simplify(str(res)))
  quit()

def add_lt(x_str, v):
  global solver
  x = Symbol(x_str, REAL)
  solver.push()
  solver.add_assertion(x < v)

def add_obj_lt(v):
  global solver
  solver.push()
  solver.add_assertion(obj_fnode < v)
  
def is_int(model, x_str):
  x = Symbol(x_str, REAL)
  tmp = (Real(float(model.get_value(x))) - Real(math.floor(float(model.get_value(x))))).simplify().is_zero()
  return tmp

def get_obj_value(model):
  modeldict = dict([(x, v) for (x, v) in model])
  res = obj_fnode.substitute(modeldict).simplify()
  return str(float(res))
  
def get_floor(model, x_str):
  x = Symbol(x_str, REAL)
  flvalue = math.floor(float(model.get_value(x).simplify()))
  return str(flvalue)
  #return str(Real(math.floor(float(model.get_value(x).simplify()))).simplify())
  # return str(simplify(ToInt(model[x])))

def get_ceil(model, x_str):
  x = Symbol(x_str, REAL)
  ceilvalue = math.ceil(float(model.get_value(x).simplify()))
  return str(ceilvalue)
  #return str(float(Real(math.floor(float(model.get_value(x).simplify())).simplify()) + 1))
  # return str(simplify(ToInt(model[x]) + 1))

def exclude_interval(x, a, b):
  global solver
  solver.push()
  var = Symbol(x, REAL)
  solver.add_assertion(Or(LE(var, Real(float(a))), GE(var, Real(float(b)))))
  
def add_opposite_eq(neg_obj_str, obj_str):
  global solver
  solver.push()
  neg_obj = Symbol(neg_obj_str, REAL)
  obj = Symbol(obj_str, REAL)
  solver.add_assertion(obj + neg_obj == 0)
  
'''
