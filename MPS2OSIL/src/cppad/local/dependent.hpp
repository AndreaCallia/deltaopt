# ifndef CPPAD_DEPENDENT_INCLUDED
# define CPPAD_DEPENDENT_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-07 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin Dependent$$
$spell 
	Taylor
	ADvector
	const
$$

$spell
$$

$section Stop Recording and Store Operation Sequence$$

$index ADFun, operation sequence$$
$index operation, sequence store$$
$index sequence, operation store$$
$index recording, stop$$
$index tape, stop recording$$
$index Dependent$$

$head Syntax$$
$syntax%%f%.Dependent(%x%, %y%)%$$

$head Purpose$$
Stop recording and the AD of $italic Base$$
$xref/glossary/Operation/Sequence/operation sequence/1/$$
that started with the call
$syntax%
	Independent(%x%)
%$$
and store the operation sequence in $italic f$$.
The operation sequence defines an 
$xref/glossary/AD Function/AD function/$$
$latex \[
	F : B^n \rightarrow B^m
\] $$
where $latex B$$ is the space corresponding to objects of type $italic Base$$.
The value $latex n$$ is the dimension of the 
$xref/SeqProperty/Domain/domain/$$ space for the operation sequence.
The value $latex m$$ is the dimension of the 
$xref/SeqProperty/Range/range/$$ space for the operation sequence
(which is determined by the size of $italic y$$).

$head f$$
The object $italic f$$ has prototype
$syntax%
	ADFun<%Base%> %f%
%$$
The AD of $italic Base$$ operation sequence is stored in $italic f$$; i.e.,
it becomes the operation sequence corresponding to $italic f$$.
If a previous operation sequence was stored in $italic f$$,
it is deleted. 

$head x$$
The argument $italic x$$ 
must be the vector argument in a previous call to
$cref/Independent/$$.
Neither its size, or any of its values, are allowed to change
between calling
$syntax%
	Independent(%x%)
%$$
and 
$syntax%
	%f%.Dependent(%x%, %y%)
%$$.

$head y$$
The vector $italic y$$ has prototype
$syntax%
	const %ADvector% &%y%
%$$
(see $xref/FunConstruct//ADvector/$$ below).
The length of $italic y$$ must be greater than zero
and is the dimension of the range space for $italic f$$.

$head ADvector$$
The type $italic ADvector$$ must be a $xref/SimpleVector/$$ class with
$xref/SimpleVector/Elements of Specified Type/elements of type/$$
$syntax%AD<%Base%>%$$.
The routine $xref/CheckSimpleVector/$$ will generate an error message
if this is not the case.

$head Taping$$
The tape,
that was created when $syntax%Independent(%x%)%$$ was called, 
will stop recording.
The AD operation sequence will be transferred from
the tape to the object $italic f$$ and the tape will then be deleted.

$head Forward$$
No $xref/Forward/$$ calculation is preformed during this operation.
Thus, directly after this operation,
$syntax%
	%f%.size_taylor()
%$$ 
is zero (see $xref/size_taylor/$$).

$head Example$$
The file
$xref/FunCheck.cpp/$$ 
contains an example and test of this operation.
It returns true if it succeeds and false otherwise.

$end
----------------------------------------------------------------------------
*/


// BEGIN CppAD namespace
namespace CppAD {

template <typename Base>
template <typename ADvector>
void ADFun<Base>::Dependent(const ADvector &y)
{	ADTape<Base> *tape = AD<Base>::tape_ptr();
	CPPAD_ASSERT_KNOWN(
		tape != CPPAD_NULL,
		"Can't store current operation sequence in this ADFun object"
		"\nbecause there is no active tape (for this thread)."
	);

	// code above just determines the tape and checks for errors
	Dependent(tape, y);
}

template <typename Base>
template <typename ADvector>
void ADFun<Base>::Dependent(const ADvector &x, const ADvector &y)
{
	CPPAD_ASSERT_KNOWN(
		x.size() > 0,
		"Dependent: independent variable vector has size zero."
	);
	CPPAD_ASSERT_KNOWN(
		Variable(x[0]),
		"Dependent: independent variable vector has been changed."
	);
	ADTape<Base> *tape = AD<Base>::tape_ptr(x[0].id_);
	CPPAD_ASSERT_KNOWN(
		tape->size_independent == x.size(),
		"Dependent: independent variable vector has been changed."
	);
# ifndef NDEBUG
	size_t i, j;
	for(j = 0; j < x.size(); j++)
	{	CPPAD_ASSERT_KNOWN(
		x[j].taddr_ == (j+1),
		"ADFun<Base>: independent variable vector has been changed."
		);
		CPPAD_ASSERT_KNOWN(
		x[j].id_ == x[0].id_,
		"ADFun<Base>: independent variable vector has been changed."
		);
	}
	for(i = 0; i < y.size(); i++)
	{	CPPAD_ASSERT_KNOWN(
		CppAD::Parameter( y[i] ) | (y[i].id_ == x[0].id_) ,
		"ADFun<Base>: dependent vector contains a variable for"
		"\na different tape (thread) than the independent variables."
		);
	}
# endif

	// code above just determines the tape and checks for errors
	Dependent(tape, y);
}
		

template <typename Base>
template <typename ADvector>
void ADFun<Base>::Dependent(ADTape<Base> *tape, const ADvector &y)
{
	size_t   m = y.size();
	size_t   n = tape->size_independent;
	size_t   i, j;
	size_t   y_taddr;

	// check ADvector is Simple Vector class with AD<Base> elements
	CheckSimpleVector< AD<Base>, ADvector>();

	CPPAD_ASSERT_KNOWN(
		y.size() > 0,
		"ADFun operation sequence dependent variable size is zero size"
	); 

	// set total number of variables in tape, parameter flag, 
	// make a tape copy of dependent variables that are parameters, 
	// and store tape address for each dependent variable
	CPPAD_ASSERT_UNKNOWN( NumVar(ParOp) == 1 );
	dep_parameter.resize(m);
	dep_taddr.resize(m);
	totalNumVar = tape->Rec.TotNumVar();
	for(i = 0; i < m; i++)
	{	dep_parameter[i] = CppAD::Parameter(y[i]);
		if( dep_parameter[i] )
		{	y_taddr = tape->RecordParOp( y[i].value_ );
			totalNumVar++;
		}
		else	y_taddr = y[i].taddr_;

		CPPAD_ASSERT_UNKNOWN( y_taddr > 0 );
		CPPAD_ASSERT_UNKNOWN( y_taddr < totalNumVar );
		dep_taddr[i] = y_taddr;
	}

	// now that each dependent variable has a place in the tape,
	// we can make a copy for this function and erase the tape.
	Rec = tape->Rec;

	// now we can delete the tape
	AD<Base>::tape_delete( tape->id_ );

	// total number of varables in this recording 
	CPPAD_ASSERT_UNKNOWN( totalNumVar == Rec.TotNumVar() );

	// used to determine if there is an operation sequence in *this
	CPPAD_ASSERT_UNKNOWN( totalNumVar > 0 );

	// free old buffers
	if( Taylor != CPPAD_NULL )
		CPPAD_TRACK_DEL_VEC(Taylor);
	if( ForJac != CPPAD_NULL )
		CPPAD_TRACK_DEL_VEC(ForJac);

	// initialize buffers
	Taylor  = CPPAD_NULL;
	ForJac  = CPPAD_NULL;

	// initial row and column dimensions
	// memoryMax  = 0;
	taylor_per_var= 0;
	ForJacColDim  = 0;
	ForJacBitDim  = 0;
	TaylorColDim  = 0;

	// set tape address 
	ind_taddr.resize(n);
	CPPAD_ASSERT_UNKNOWN(
		n < totalNumVar
	);
	for(j = 0; j < n; j++)
	{	CPPAD_ASSERT_UNKNOWN( Rec.GetOp(j+1) == InvOp );
		ind_taddr[j] = j+1;
	}

}

} // END CppAD namespace

# endif
