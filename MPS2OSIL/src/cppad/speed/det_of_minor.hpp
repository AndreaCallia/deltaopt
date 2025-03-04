# ifndef CPPAD_DET_OF_MINOR_INCLUDED
# define CPPAD_DET_OF_MINOR_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-07 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin det_of_minor$$
$spell
	cppad.hpp
	hpp
	std
	Det
	const
	CppADvector
	namespace
$$

$index det_of_minor$$
$index determinant, matrix minor$$
$index matrix, minor determinant$$
$index minor, matrix determinant$$

$section Determinant of a Minor$$

$head Syntax$$
$syntax%# include <cppad/speed/det_of_minor.hpp>
%$$ 
$syntax%%d% = det_of_minor(%a%, %m%, %n%, %r%, %c%)%$$ 


$head Inclusion$$
The template function $code det_of_minor$$ is defined in the $code CppAD$$
namespace by including 
the file $code cppad/speed/det_of_minor.hpp$$ 
(relative to the CppAD distribution directory).
It is only intended for example and testing purposes, 
so it is not automatically included by
$cref/cppad.hpp/cppad/$$.

$head Purpose$$
This template function
returns the determinant of a minor of the matrix $latex A$$
using expansion by minors.
The elements of the $latex n \times n$$ minor $latex M$$ 
of the matrix $latex A$$ are defined,
for $latex i = 0 , \ldots , n-1$$ and $latex j = 0 , \ldots , n-1$$, by
$latex \[
	M_{i,j} = A_{R(i), C(j)}
\]$$
where the functions 
$latex R(i)$$ is defined by the $cref/argument r/det_of_minor/r/$$ and
$latex C(j)$$ is defined by the $cref/argument c/det_of_minor/c/$$.
$pre

$$
This template function
is for example and testing purposes only.
Expansion by minors is chosen as an example because it uses
a lot of floating point operations yet does not require much source code
(on the order of $italic m$$ factorial floating point operations and 
about 70 lines of source code including comments).
This is not an efficient method for computing a determinant;
for example, using an LU factorization would be better.

$head Determinant of A$$
If the following conditions hold, the minor is the
entire matrix $latex A$$ and hence $code det_of_minor$$
will return the determinant of $latex A$$:

$list number$$
$latex n = m$$.
$lnext
for $latex i = 0 , \ldots , m-1$$, $latex r[i] = i+1$$, 
and $latex r[m] = 0$$.
$lnext
for $latex j = 0 , \ldots , m-1$$, $latex c[j] = j+1$$,
and $latex c[m] = 0$$.
$lend

$head a$$
The argument $italic a$$ has prototype
$syntax%
	const std::vector<%Scalar%>& %a%
%$$ 
and is a vector with size $latex m * m$$
(see description of $cref/Scalar/det_of_minor/Scalar/$$ below).
The elements of the $latex m \times m$$ matrix $latex A$$ are defined,
for $latex i = 0 , \ldots , m-1$$ and $latex j = 0 , \ldots , m-1$$, by
$latex \[
	A_{i,j} = a[ i + j * m ]
\] $$

$head m$$
The argument $italic m$$ has prototype
$syntax%
	size_t %m%
%$$
and is the size of the square matrix $latex A$$.

$head n$$
The argument $italic n$$ has prototype
$syntax%
	size_t %n%
%$$
and is the size of the square minor $latex M$$.

$head r$$
The argument $italic r$$ has prototype
$syntax%
	std::vector<size_t>& %r%
%$$
and is a vector with $latex m + 1$$ elements.
This vector defines the function $latex R(i)$$ 
which specifies the rows of the minor $latex M$$.
To be specific, the function $latex R(i)$$ 
for $latex i = 0, \ldots , n-1$$ is defined by 
$latex \[
\begin{array}{rcl}
	R(0)   & = & r[m]
	\\
	R(i+1) & = & r[ R(i) ]
\end{array}
\] $$
All the elements of $italic r$$ must have value
less than or equal $italic m$$.
The elements of vector $italic r$$ are modified during the computation, 
and restored to their original value before the return from
$code det_of_minor$$.

$head c$$
The argument $italic c$$ has prototype
$syntax%
	std::vector<size_t>& %c%
%$$
and is a vector with $latex m + 1$$ elements
This vector defines the function $latex C(i)$$ 
which specifies the rows of the minor $latex M$$.
To be specific, the function $latex C(i)$$ 
for $latex j = 0, \ldots , n-1$$ is defined by 
$latex \[
\begin{array}{rcl}
	C(0)   & = & c[m]
	\\
	C(j+1) & = & c[ C(j) ]
\end{array}
\] $$
All the elements of $italic c$$ must have value
less than or equal $italic m$$.
The elements of vector $italic c$$ are modified during the computation, 
and restored to their original value before the return from
$code det_of_minor$$.

$head d$$
The result $italic d$$ has prototype
$syntax%
	%Scalar% %d%
%$$
and is equal to the determinant of the minor $latex M$$.

$head Scalar$$
If $italic x$$ and $italic y$$ are objects of type $italic Scalar$$
and $italic i$$ is an object of type $code int$$,
the $italic Scalar$$ must support the following operations:
$table
$bold Syntax$$ 
	$cnext $bold Description$$
	$cnext $bold Result Type$$
$rnext
$syntax%%Scalar% %x%$$
	$cnext default constructor for $italic Scalar$$ object.
$rnext
$syntax%%Scalar% %x%(%i%)%$$
	$cnext construct $italic x$$ with value corresponding to $italic i$$
$rnext
$syntax%%x% = %y%$$
	$cnext set value of $italic x$$ to current value of $italic y$$
$rnext
$syntax%%x% + %y%$$
	$cnext value of $italic x$$ plus of $italic y$$
	$cnext $italic Scalar$$
$rnext
$syntax%%x% - %y%$$
	$cnext value of $italic x$$ minus of $italic y$$
	$cnext $italic Scalar$$
$rnext
$syntax%%x% * %y%$$
	$cnext value of $italic x$$ times value of $italic y$$
	$cnext $italic Scalar$$
$tend

$children%
	speed/example/det_of_minor.cpp%
	omh/det_of_minor_hpp.omh
%$$

$head Example$$
The file
$cref/det_of_minor.cpp/$$ 
contains an example and test of $code det_of_minor.hpp$$.
It returns true if it succeeds and false otherwise.

$head Source Code$$
The file
$xref/det_of_minor.hpp/$$ 
contains the source for this template function.


$end
---------------------------------------------------------------------------
*/
// BEGIN PROGRAM
namespace CppAD { // BEGIN CppAD namespace
template <class Scalar>
Scalar det_of_minor(
	const std::vector<Scalar>& a  , 
	size_t                     m  , 
	size_t                     n  , 
	std::vector<size_t>&       r  , 
	std::vector<size_t>&       c  )
{	size_t         R0;  // R(0)
	size_t         Cj;  // C(j)
	size_t        Cj1;  // C(j-1)
	size_t          j;  // column index in M
	Scalar       detS;  // determinant of sub-minor of M
	Scalar        M0j;  // an element of the first row of M
	int            s;   // sign of the current sub-minor in summation

	// value of R(0) and C(0)
	R0 = r[m];
	Cj = c[m];

	// check for 1 by 1 case
	if( n == 1 )
		return a[ R0 * m + Cj ];

	// initialize determinant of the minor M
	Scalar detM(0);

	// initialize sign of permutation
	s = 1;

	// remove row with index R0 from sub-minors of M
	r[m] = r[R0];

	// initialize previous column index; Cj = c[ Cj1 ]
	Cj1  = m;

	// for each column of M
	for(j = 0; j < n; j++)
	{	// M(0, j)
		M0j = a[ R0 * m + Cj ];

		// remove column index j from sub-minor of M
		c[Cj1] = c[Cj];

		// compute determinant of sub-minor of M 
		// where row R0 and column Cj are removed
		detS = det_of_minor(a, m, n - 1, r, c);

		// restore column Cj to sub-minor of M
		c[Cj1] = Cj;

		// include in summation
		if( s > 0 )
			detM = detM + M0j * detS;
		else	detM = detM - M0j * detS;

		// advance to next column of M
		Cj1 = Cj;
		Cj  = c[Cj];
		s   = -s;		
	}

	// restore row index zero to the minor M
	r[m] = R0;

	// return the determinant of the minor M
	return detM;
}
} // END CppAD namespace
// END PROGRAM
# endif
