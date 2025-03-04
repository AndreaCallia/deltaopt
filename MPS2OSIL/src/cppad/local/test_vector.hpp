# ifndef CPPAD_TEST_VECTOR_INCLUDED
# define CPPAD_TEST_VECTOR_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-07 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin test_vector$$
$spell
	CppADvector
	hpp
	std
	endif
	ublas
	Dir
	valarray
	stdvector
$$

$index CPPAD_TEST_VECTOR$$
$index vector, test$$
$index test, vector$$

$section Choosing The Vector Testing Template Class$$

$head Syntax$$
$syntax%CPPAD_TEST_VECTOR<%Scalar%>
%$$

$head Introduction$$
Many of the CppAD $xref/example//examples/$$ and tests use 
the $code CPPAD_TEST_VECTOR$$ template class to pass information.
The default definition for this template class is
$xref/CppAD_vector//CppAD::vector/$$.

$head CppAD::vector$$
By default $code CPPAD_CPPADVECTOR$$ is true
and $code CPPAD_TEST_VECTOR$$ is defined by the following source code
$codep */
// The next line is C++ source code and its replacement is discussed below
# if CPPAD_CPPADVECTOR
# define CPPAD_TEST_VECTOR CppAD::vector
# endif
/* $$
You can replace this definition of the preprocessor symbol
$code CPPAD_TEST_VECTOR$$ by any other $xref/SimpleVector/$$ template class.
This will test using your replacement template vector class with CppAD.

$head std::vector$$
If you specify $code --with-stdvector$$ on the 
$cref/configure/InstallUnix/Configure/$$
command line during CppAD installation,
$code CPPAD_STDVECTOR$$ is true
and $code CPPAD_TEST_VECTOR$$ is defined by the following source code
$code CPPAD_TEST_VECTOR$$ 
$codep */
# if CPPAD_STDVECTOR
# include <vector>
# define CPPAD_TEST_VECTOR std::vector
# endif
/* $$
In this case CppAD will use $code std::vector$$ for its examples and tests.
Use of $code CppAD::vector$$, $code std::vector$$,
and $code std::valarray$$ with CppAD is always tested to some degree.
Specifying $code --with-stdvector$$ will increase the amount of
$code std::vector$$ testing. 

$head boost::numeric::ublas::vector$$
If you specify a value for $italic BoostDir$$ on the configure
command line during CppAD installation,
$code CPPAD_BOOSTVECTOR$$ is true 
and $code CPPAD_TEST_VECTOR$$ is defined by the following source code
$codep */
# if CPPAD_BOOSTVECTOR
# include <boost/numeric/ublas/vector.hpp>
# define CPPAD_TEST_VECTOR boost::numeric::ublas::vector
# endif
/* $$
In this case CppAD will use Ublas vectors for its examples and tests.
Use of $code CppAD::vector$$, $code std::vector$$,
and $code std::valarray$$ with CppAD is always tested to some degree.
Specifying $italic BoostDir$$ will increase the amount of
Ublas vector testing. 

$head Deprecated$$
$index CppADvector, deprecated$$
$index deprecated, CppADvector$$
The preprocessor symbol $code CppADvector$$ is defined to
have the same value as $code CPPAD_TEST_VECTOR$$ but its use is deprecated
$codep */
# define CppADvector CPPAD_TEST_VECTOR
/* $$
$end
------------------------------------------------------------------------ 
*/

# endif
