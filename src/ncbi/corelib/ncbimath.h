/*   ncbimath.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
* File Name:  ncbimath.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   10/23/91
*
* $Revision: 2.3 $
*
* File Description:
*   	prototypes for portable math library
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#ifndef _NCBIMATH_
#define _NCBIMATH_

#ifdef __cplusplus
extern "C" {
#endif

/* log(x+1) for all x > -1 */
double LIBCALL Nlm_Log1p PROTO((double));

/* exp(x)-1 for all x */
double LIBCALL Nlm_Expm1 PROTO((double));

/* Factorial */
double LIBCALL Nlm_Factorial PROTO((int));

/* gamma(x) */
double LIBCALL Nlm_Gamma PROTO((double));

/* log(gamma(x)) */
double LIBCALL Nlm_LnGamma PROTO((double));

/* log(gamma(n)), integral n */
double LIBCALL Nlm_LnGammaInt PROTO((int));

/* digamma(x) 1st order derivative of log(gamma(x)) */
double LIBCALL Nlm_DiGamma PROTO((double));

/* trigamma(x) 2nd order derivative of log(gamma(x)) */
double LIBCALL Nlm_TriGamma PROTO((double));

/* Nth order derivative of log(gamma) */
double LIBCALL Nlm_PolyGamma PROTO((double x, int order));

/* Change gamma coefficients */
void LIBCALL Nlm_GammaCoeffSet PROTO((double PNTR coef, unsigned dimension));

/* Nth order derivative of ln(u) */
double LIBCALL Nlm_LogDerivative PROTO((int order, double PNTR u));


/* Combined Newton-Raphson and Bisection root solver */
double LIBCALL Nlm_NRBis PROTO((double y, double (LIBCALL *f) (double), double (LIBCALL *df) (double), double p, double x, double q, double tol));

/* Romberg numerical integrator */
double LIBCALL Nlm_RombergIntegrate PROTO((double (LIBCALL *f) (double, Nlm_VoidPtr), Nlm_VoidPtr fargs, double p, double q, double eps, int epsit, int itmin));

/* Greatest common divisor */
long LIBCALL Nlm_Gcd PROTO((long, long));

/* Nearest integer */
long LIBCALL Nlm_Nint PROTO((double));

/* Integral power of x */
double LIBCALL Nlm_Powi PROTO((double x, int n));

/* Random no. seeder and generator */
void LIBCALL Nlm_RandomSeed PROTO((long n));
long LIBCALL Nlm_RandomNum PROTO((void));


#define Log1p	Nlm_Log1p
#define Expm1	Nlm_Expm1
#define Factorial	Nlm_Factorial
#define Gamma	Nlm_Gamma
#define LnGamma	Nlm_LnGamma
#define DiGamma	Nlm_DiGamma
#define TriGamma	Nlm_TriGamma
#define PolyGamma	Nlm_PolyGamma
#define GammaCoeffSet	Nlm_GammaCoeffSet
#define LogDerivative	Nlm_LogDerivative
#define NRBis	Nlm_NRBis
#define RombergIntegrate	Nlm_RombergIntegrate
#define Gcd	Nlm_Gcd
#define Nint	Nlm_Nint
#define Powi	Nlm_Powi
#define RandomSeed	Nlm_RandomSeed
#define RandomNum	Nlm_RandomNum

/* Error codes for the CTX_NCBIMATH context */
#define ERR_NCBIMATH_INVAL	1 /* invalid parameter */
#define ERR_NCBIMATH_DOMAIN	2 /* domain error */
#define ERR_NCBIMATH_RANGE	3 /* range error */
#define ERR_NCBIMATH_ITER	4 /* iteration limit exceeded */

#define LOGDERIV_ORDER_MAX	4
#define POLYGAMMA_ORDER_MAX	LOGDERIV_ORDER_MAX

#define NCBIMATH_PI	3.1415926535897932384626433832795
#define NCBIMATH_E	2.7182818284590452353602874713527
/* Euler's constant */
#define NCBIMATH_EULER 0.5772156649015328606065120900824
/* Catalan's constant */
#define NCBIMATH_CATALAN	0.9159655941772190150546035149324

/* sqrt(2) */
#define NCBIMATH_SQRT2	1.4142135623730950488016887242097
/* sqrt(3) */
#define NCBIMATH_SQRT3	1.7320508075688772935274463415059
/* sqrt(PI) */
#define NCBIMATH_SQRTPI 1.7724538509055160272981674833411
/* Natural log(2) */
#define NCBIMATH_LN2	0.69314718055994530941723212145818
/* Natural log(10) */
#define NCBIMATH_LN10	2.3025850929940456840179914546844
/* Natural log(PI) */
#define NCBIMATH_LNPI	1.1447298858494001741434273513531

#ifdef __cplusplus
}
#endif

#endif /* !_NCBIMATH_ */
