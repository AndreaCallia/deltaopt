# ifndef CPPAD_NEAR_EQUAL_INCLUDED
# define CPPAD_NEAR_EQUAL_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-06 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin NearEqual$$
$spell
	cppad.hpp
	sqrt
	cout
	endl
	Microsoft
	std
	Cpp
	namespace
	const
	bool
$$

$section Determine if Two Values Are Nearly Equal$$

$index NearEqual$$
$index equal, near$$
$index absolute, difference$$
$index relative, difference$$
$index difference, absolute$$
$index difference, relative$$

$head Syntax$$

$code # include <cppad/near_equal.hpp>$$
$pre
$$
$syntax%%b% = NearEqual(%x%, %y%, %r%, %a%)%$$


$head Purpose$$
Returns true,
if $italic x$$ and $italic y$$ are nearly equal,
and false otherwise.

$head x$$
The argument $italic x$$ 
has one of the following possible prototypes
$syntax%
	const %Type%               &%x%,
	const std::complex<%Type%> &%x%, 
%$$

$head y$$
The argument $italic y$$ 
has one of the following possible prototypes
$syntax%
	const %Type%               &%y%,
	const std::complex<%Type%> &%y%, 
%$$

$head r$$
The relative error criteria $italic r$$ has prototype
$syntax%
	const %Type% &%r%
%$$
It must be greater than or equal to zero.
The relative error condition is defined as:
$latex \[
	| x - y | \leq r ( |x| + |y| ) 
\] $$

$head a$$
The absolute error criteria $italic a$$ has prototype
$syntax%
	const %Type% &%a%
%$$
It must be greater than or equal to zero.
The absolute error condition is defined as:
$latex \[
	| x - y | \leq a
\] $$

$head b$$
The return value $italic b$$ has prototype
$syntax%
	bool %b%
%$$
If either $italic x$$ or $italic y$$ is infinite or not a number, 
the return value is false.
Otherwise, if either the relative or absolute error 
condition (defined above) is satisfied, the return value is true.
Otherwise, the return value is false.

$head Type$$
The type $italic Type$$ must be a
$xref/NumericType/$$.
The routine $xref/CheckNumericType/$$ will generate
an error message if this is not the case.
In addition, the following operations must be defined objects
$italic a$$ and $italic b$$ of type $italic Type$$:
$table
$bold Operation$$     $cnext 
	$bold Description$$ $rnext
$syntax%%a% <= %b%$$  $cnext 
	less that or equal operator (returns a $code bool$$ object)
$tend

$head Include Files$$
The file $code cppad/near_equal.hpp$$ is included by $code cppad/cppad.hpp$$
but it can also be included separately with out the rest of 
the $code CppAD$$ routines.

$head Example$$
$children%
	example/near_equal.cpp
%$$
The file $xref/Near_Equal.cpp/$$ contains an example
and test of $code NearEqual$$.
It return true if it succeeds and false otherwise.

$head Exercise$$
$index exercise, NearEqual$$
Create and run a program that contains the following code:
$codep
	using std::complex;
	using std::cout;
	using std::endl;

	complex<double> one(1., 0), i(0., 1);
	complex<double> x = one / i;
	complex<double> y = - i;
	double          r = 1e-12;
	double          a = 0;
	bool           ok = CppAD::NearEqual(x, y, r, a);
	if( ok )
		cout << "Ok"    << endl;
	else	cout << "Error" << endl;
$$

$end

*/

# include <complex>
# include <cppad/local/cppad_assert.hpp>
# include <cppad/check_numeric_type.hpp>

namespace CppAD { // Begin CppAD namespace

// determine if both x and y are finite values (z1 and z2 are zero).
template <class Type>
bool isfinite(const Type &z1, const Type &z2, const Type &x , const Type &y)
{	Type infinity = Type(1) / z1;
	Type nan      = z1      / z2;

	// handle bug where some compilers return true for nan == nan
	bool xNan = ( x != x || x == nan );
	bool yNan = ( y != y || y == nan );

	// infinite cases
	bool xInf = (x == infinity   || x == - infinity);
	bool yInf = (x == infinity   || x == - infinity);
	
	return ! (xNan | yNan | xInf | yInf);
}

template <class Type>
bool NearEqual(const Type &x, const Type &y, const Type &r, const Type &a)
{
	CheckNumericType<Type>();
	Type zero(0);

	CPPAD_ASSERT_KNOWN(
		zero <= r,
		"Error in NearEqual: relative error is less than zero"
	);
	CPPAD_ASSERT_KNOWN(
		zero <= a,
		"Error in NearEqual: absolute error is less than zero"
	);

	// check for special cases
	if( ! isfinite(zero, zero, x, y) )
		return false;

	Type ax = x;
	if( ax <= zero )
		ax = - ax;

	Type ay = y;
	if( ay <= zero )
		ay = - ay;

	Type ad = x - y;
	if( ad <= zero )
		ad = - ad;

	if( ad <= a )
		return true;

	if( ad <= r * (ax + ay) )
		return true;

	return false;
}

template <class Type>
bool NearEqual(
	const std::complex<Type> &x , 
	const std::complex<Type> &y , 
	const              Type  &r , 
	const              Type  & a )
{
	CheckNumericType<Type>();
	Type zero(0);

	CPPAD_ASSERT_KNOWN(
		zero <= r,
		"Error in NearEqual: relative error is less than zero"
	);
	CPPAD_ASSERT_KNOWN(
		zero <= a,
		"Error in NearEqual: absolute error is less than zero"
	);

	// check for special cases
	if( ! isfinite(zero, zero, x.real(), x.imag()) )
		return false;
	if( ! isfinite(zero, zero, y.real(), y.imag()) )
		return false;

	std::complex<Type> d = x - y;

	Type ad = std::abs(d);
	if( ad <= a )
		return true;

	Type ax = std::abs(x);
	Type ay = std::abs(y);
	if( ad <= r * (ax + ay) )
		return true;

	return false;
}

template <class Type>
bool NearEqual(
	const std::complex<Type> &x , 
	const              Type  &y , 
	const              Type  &r , 
	const              Type  & a )
{
	return NearEqual(x, std::complex<Type>(y, Type(0)), r, a);
}

template <class Type>
bool NearEqual(
	const              Type  &x , 
	const std::complex<Type> &y , 
	const              Type  &r , 
	const              Type  & a )
{
	return NearEqual(std::complex<Type>(x, Type(0)), y, r, a);
}

} // END CppAD namespace

# endif
