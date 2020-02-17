from .. import utils
from pysmt.logics import *
from pysmt.shortcuts import Or, Symbol, Solver, And, Real, LE, GE, get_env, ToReal
from pysmt.smtlib.solver import SmtLibSolver
from pysmt.smtlib.parser import SmtLibParser, get_formula, Tokenizer
from pysmt.typing import REAL #, INT
from pysmt.exceptions import SolverReturnedUnknownResultError
import sys
import math
from sympy import simplify

obj_keyword = ";obj_smt="

solver = None
solver_name = ""
env = None

objective = None
obj_fnode = None    

def configure(inpcode, options):
  global solver
  global solver_name
  global objective
  global obj_fnode
  if ("pysmt" in options):
    solver_name = options["pysmt"]
  elif ("smtpipe" in options):
    solver_name = "custom: " + options["smtpipe"].split("/")[-1]
    solver_cmd = options["smtpipe"]
    # solver_logics = [BOOL, LIA, LRA, NIA, NRA, QF_LRA, QF_NIA, QF_NRA, QF_UFLIA, QF_UFLRA, QF_UFNIA, QF_UFNRA, UFLIRA, UFLRA, UFNIA, AUFNIRA]
    # solver_logics = [AUFNIRA]
    solver_logic = eval(options["pipe_logic"])
    solver_logics = [solver_logic]
    env = get_env()
    env.factory.add_generic_solver(solver_name, solver_cmd, solver_logics)
  verbprint = utils.verbprint
  verbprint(2, "Parsing the input...", False)
  parser = SmtLibParser()
  script = parser.get_script(inpcode)
  formula = script.get_last_formula()
  # print "Got a formula: " + str(f)
  verbprint(2, "done.", True)
  verbprint(2, "Initializing the solver...", False)
  if ("smtpipe" in options):
    solver = Solver(name=solver_name, logic=solver_logic)
    #solver = SmtLibSolver(options["smtpipe"], env, LRA)
  else:
    solver = Solver(name=solver_name)
  verbprint(2, "done.", True)
  verbprint(2, "Loading the input file in the solver...", False)
  solver.add_assertion(formula)
  verbprint(2, "done.", True)
  #Parsing the objective annotation
  for line in inpcode.splitlines():
    if "".join(line.split()).startswith(obj_keyword):
      objective = line[line.index("=") + 1:]
      objparser = SmtLibParser
      tokens = Tokenizer(objective)
      obj_fnode = parser.get_expression(tokens)
      # obj_evaluator = lisp_to_evaluator(objective)
      # obj_fnode = lisp_to_fnode(objective)
      # tmpvar = parser.get_script(objective)
  # print "obj_fnode is: " + str(obj_fnode)
  # print "objective variables: " + str(obj_fnode.get_free_variables())
  # print "Finished configuration of the solver."
  # exit(1)
  

def check_feasibility():
  global solver
  global solver_name
  unknown = False
  verbprint = utils.verbprint
  utils.stats_create("Solving time", 0.0)
  utils.stats_create("Solver calls", 0)
  verbprint(3, "Checking satisfiability...", False)
  utils.time_start("Solving time")
  try:
    result = solver.solve()
  except SolverReturnedUnknownResultError:
    unknown = True
  utils.stats_update("Solving time", utils.time_take("Solving time"))
  utils.stats_update("Solver calls", 1)
  verbprint(3, "done.", True)
  if unknown:
    utils.error(utils.UNKNOWN_ERROR, "feasibility: The " + solver_name + " solver returned unknown\nUNKNOWN")
  if (result == False):
    return None
  # if (result != sat):
  #  utils.error(utils.UNKNOWN_ERROR, "feasibility: The Z3 solver returned: " + str(result))
  return solver.get_model()

# Auxiliary functions required by above layers                                                                         
def pop(n):
  global solver
  for i in range(0, n):
    solver.pop()

def model_vars(model):
  return [str(x) for (x, y) in model]

def is_int(model, x_str):
  x = Symbol(x_str, REAL)
  tmp = (Real(float(model.get_value(x))) - Real(math.floor(float(model.get_value(x))))).simplify().is_zero()
  return tmp

def get_model_value(model, x_str):
  x = Symbol(x_str, REAL)
  return str(model[x])

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
