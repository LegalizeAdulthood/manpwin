/*
    BIGFUNCTIONS.CPP a module for the per pixel calculations of Bignum fractals.
    
    Written in Microsoft Visual 'C++' by Paul de Leeuw.

    This program is written in "standard" C. Hardware dependant code
    (console drivers & serial I/O) is in separate machine libraries.
*/

#include	<math.h>
#include	"manp.h"
#include	"fractype.h"
#include	"Complex.h"
#include	"big.h"
#include	"BigDouble.h"
#include	"BigComplex.h"
#include	"pixel.h"

/**************************************************************************
	Initialise functions for each pixel
**************************************************************************/

int	CPixel::BigInitFunctions(WORD type, BigComplex *zBig, BigComplex *qBig)
    {
    switch (type)
	{
	case MANDELFP:					// Mandelbrot
	case MANDEL:					// to handle fractint par files
	case JULIA:					// to handle fractint par files
	case BURNINGSHIP:				// Burning Ship
	    if (!juliaflag)
		{
		zBig->x = qBig->x + param[0];
		zBig->y = qBig->y + param[1];
		}
    	    break;

	case BURNINGSHIPPOWER:				// Burning Ship to higher power
	case POWER:					// Power
	    *degree = (int)param[0];
	    if (*degree < 1)
		*degree = 1;
	    if (!juliaflag)
		{
		zBig->x = qBig->x + param[1];
		zBig->y = qBig->y + param[2];
		}
	    break;

	case CUBIC:					// Art Matrix Cubic
	    {
	    BigComplex	tempBig;

	    switch ((int)param[0])
		{
		case 0:
		    subtype = 'B';
		    break;
		case 1:
		    subtype = 'C';
		    break;
		case 2:
		    subtype = 'F';
		    break;
		case 3:
		    subtype = 'K';
		    break;
		default:
		    subtype = 'B';
		    break;
		}
	    if ((int)param[1] < 0)
		special = 0;
	    else
		special = (int)param[1];

	    period_level = FALSE;			// no periodicity checking
	    if (subtype == 'B')				// CBIN 
		{
		t3Big = *qBig * 3.0;			// T3 = 3*T
		t2Big = qBig->CSqr();			// T2 = T*T
		aBig = (t2Big + 1.0) / t3Big;		// A  = (T2 + 1)/T3
							// B  = 2*A*A*A + (T2 - 2)/T3    
		tempBig = aBig.CCube();			// A*A*A
		tempBig = tempBig.CDouble();		// 2*A*A*A
		bBig = (t2Big - 2.0) / t3Big + tempBig;	// B  = 2*A*A*A + (T2 - 2)/T3
		}
	    else if (subtype == 'C' || subtype == 'F')	// CCIN or CFIN
		{
		aBig = *qBig;				// A = T
							// find B = T + 2*T*T*T
		tempBig = qBig->CCube();		// B = T*T*T
		if (subtype == 'C')
		    bBig = tempBig.CDouble() + *qBig;	// B = B * 2 + T
		else
		    {
		    bBig = (tempBig - *qBig);		// B = B - T
		    bBig = bBig.CDouble();		// B = B * 2 - 2 * T
		    a2Big = aBig.CDouble();
		    }
		}
	    else if (subtype == 'K')			// CKIN 
		{
		aBig = 0;
		vBig = 0;
		bBig = *qBig;				// B = T
		}
	    aa3Big = aBig.CSqr()*3.0;			// AA3 = A*A*3
	    if (!juliaflag)
		*zBig = -aBig;				// Z = -A
	    }
	    break;

	case SPECIALNEWT:				// Art Matrix Newton
	    l2Big = qBig->CSqr();
	    aBig = -l2Big + 0.25;
	    bBig = -(l2Big + 0.75);
	    lm5Big = *qBig - 0.5;
	    lp5Big = *qBig + 0.5;
	    break;

	case MATEIN:					// Art Matriuc Matein fractal
	    {
	    period_level = FALSE;			// no periodicity checking
	    double	one = 1.0;
	    if ((absolute = qBig->CSumSqr()) > one)
		return(-1);				// not inside set
	    zBig->x = 1.0;
	    zBig->y = 0.0;
	    /* DO 300 I = 1,100 */
	    /* 300  Z = L*(Z + 1/Z) */
	    for (int i = 0; i < 100; ++i)
		*zBig = (*zBig + zBig->CInvert()) * *qBig;

	    distance = 1.0;				// D = 1 
	    ozBig = zBig->CInvert();			// OZ = 1/Z
	    break;
	    }

	case SINFRACTAL:				// Sine
	    if (!juliaflag)
		{
		BigDouble BigPi;

		mpfr_const_pi(BigPi.x, MPFR_RNDN);

//		zBig.x = PI / 2.0;		// dz_real = HALF_PI;
//		zBig->x = BigPi / 2.0;		// dz_real = HALF_PI;
		zBig->x = param[3];
		zBig->y = 0.0;
		}
	    break;

	case REDSHIFTRIDER:				// RedShiftRider    a*z^2 +/- z^n + c
	    aBig.x = param[0];
	    aBig.y = param[1];
	    *degree = (WORD)param[2];
	    if (!juliaflag)
		{
		zBig->x = qBig->x + param[3];
		zBig->y = qBig->y + param[4];
		}
	    break;

	case TALIS:					// Talis Power    Z = Z^N/(M + Z^(N-1)) + C
	    *degree = (int)param[0];
	    if (*degree < 1)
		*degree = 1;
	    if (!juliaflag)
		{
		zBig->x = qBig->x + param[2];
		zBig->y = qBig->y + param[3];
		}
	    break;

	case POLYNOMIAL:
	    if (!juliaflag)
		{
		zBig->x = qBig->x + param[0];
		zBig->y = qBig->y + param[1];
		}
	    for (int i = 0; i < MAXPOLYDEG; i++)			// find highest order of polynomial for use with fwd diff calcs
		{
		if (param[2 + i] != 0.0)
		    {
		    fractalspecific[type].SlopeDegree = MAXPOLYDEG - i;
		    break;
		    }
		}
	    break;

	case MANDELDERIVATIVES:				// a group of Mandelbrot Derivatives
	    BigInitManDerFunctions(subtype, zBig, qBig);
	    break;

	case TIERAZON:					// a group of Mandelbrot Derivatives
	    BigInitTierazonFunctions(subtype, zBig, qBig);
	    break;
	}
    return 0;
    }


/**************************************************************************
	Run functions for each iteration
**************************************************************************/

int	CPixel::BigRunFunctions(WORD type, BigComplex *zBig, BigComplex *qBig, BYTE *SpecialFlag, long *iteration)
    {
    switch (type)
	{
	case MANDELFP:					// Mandelbrot
	case MANDEL:					// to handle fractint par files
	case JULIA:					// to handle fractint par files
	    {
	    BigDouble	t, realimagBig;
	    BigComplex	sqrBig;

	    // use direct function calls for Mandelbrot for speed. Approx 10 - 15 % faster.
	    mpfr_mul(realimagBig.x, zBig->x.x, zBig->y.x, MPFR_RNDN);
	    mpfr_mul(sqrBig.x.x, zBig->x.x, zBig->x.x, MPFR_RNDN);
	    mpfr_mul(sqrBig.y.x, zBig->y.x, zBig->y.x, MPFR_RNDN);
	    mpfr_sub(t.x, sqrBig.x.x, sqrBig.y.x, MPFR_RNDN);
	    mpfr_add(zBig->x.x, qBig->x.x, t.x, MPFR_RNDN);
	    mpfr_add(t.x, realimagBig.x, qBig->y.x, MPFR_RNDN);
	    mpfr_add(zBig->y.x, realimagBig.x, t.x, MPFR_RNDN);
	    mpfr_add(t.x, sqrBig.x.x, sqrBig.y.x, MPFR_RNDN);
	    return (mpfr_cmp(t.x, BigBailout.x) > 0);
	    }

	case BURNINGSHIP:				// Burning Ship
	    {
	    BigDouble	t, realimagBig;
	    BigComplex	sqrBig;

	    sqrBig.x = zBig->x.BigSqr();
	    sqrBig.y = zBig->y.BigSqr();
	    t = zBig->x * zBig->y;
	    realimagBig = t.BigAbs();
	    zBig->x = qBig->x + sqrBig.x - sqrBig.y;
	    zBig->y = realimagBig + realimagBig - qBig->y;
	    return (sqrBig.x + sqrBig.y > BigBailout);
	    }

	case BURNINGSHIPPOWER:				// Burning Ship to higher power
	    zBig->x = zBig->x.BigAbs();
	    zBig->y = -zBig->y.BigAbs();
	    *zBig = *qBig + zBig->CPolynomial(*degree);
	    return (zBig->x.BigSqr() + zBig->y.BigSqr() > BigBailout);

	case POWER:			// Power
	    *zBig = *qBig + zBig->CPolynomial(*degree);
	    return (zBig->CSumSqr() > rqlim);
//	    return (zBig->x.BigSqr() + zBig->y.BigSqr() > BigBailout);

	case CUBIC:					// Art Matrix Cubic
	    {
	    BigComplex	tempBig;

	    if (subtype == 'K')				// CKIN
		*zBig = zBig->CCube() + bBig;		// Z = Z*Z*Z + B
	    else					// Z = Z*Z*Z - AA3*Z + B
		{
		tempBig = zBig->CCube() + bBig;		// Z = Z*Z*Z + B
		*zBig = tempBig - aa3Big * *zBig;	// Z = Z*Z*Z - AA3*Z + B
		}
	    if (zBig->CSumSqr() >= 100.0)
		return (TRUE);
	    else
		{
		if (subtype == 'F')
		    {
		    if (qBig->CSumSqr() <= 0.111111)
			{
			*iteration = special;
			*SpecialFlag = TRUE;		// for decomp and biomorph
			return (TRUE);
			}
		    vBig = *zBig + a2Big;
		    }
		else if (subtype == 'K')
		    vBig = *zBig - vBig;
		else
		    vBig = *zBig - aBig;
		if (vBig.CSumSqr() <= 0.000001)
		    {
		    *iteration = special;
		    *SpecialFlag = TRUE;		// for decomp and biomorph
		    return (TRUE);
		    }
		}
	    return(FALSE);
	    }

	case SPECIALNEWT:				// Art Matrix Newton
	    {
	    BigComplex	z2Big;

	    z2Big = zBig->CSqr();
	    temp1Big = z2Big * zBig->CDouble() + aBig;
	    temp2Big = z2Big * 3.0 + bBig;
	    *zBig = temp1Big / temp2Big;

	    vBig = *zBig - 1.0;
	    if (vBig.CSumSqr() <= 0.000001)
		{
		phaseflag = 0;				// first phase
		return(TRUE);
		}
	    // v_real = dz_real - lm5_real;
	    vBig = *zBig - lm5Big;			// v_imag = dz_imag - lm5_imag;
	    if (vBig.CSumSqr() <= 0.000001)
		{
		phaseflag = 1;				// second phase
		return(TRUE);
		}
	    // v_real = dz_real + lp5_real;
	    vBig = *zBig + lp5Big;			// v_imag = dz_imag + lp5_imag;
	    if (vBig.CSumSqr() <= 0.000001)
		{
		phaseflag = 2;				// third phase
		return(TRUE);
		}
	    return(FALSE);
	    }

	case MATEIN:					// Art Matriuc Matein fractal
	    {
	    double	epsilon = 0.01;
	    double	escape = 10.0E20;
	    BigComplex	t;

	    *zBig = *qBig * (*zBig + ozBig);		// Z = L*(Z + OZ)
	    ozBig = zBig->CInvert();			// OZ = 1/Z
	    t = -ozBig / *zBig;				// T = 1 - OZ/Z
	    t.x = t.x + 1.0;
	    // D = D*ABSL*(REAL(T)*REAL(T) + IMAG(T)*IMAG(T))
	    distance = distance * absolute * t.CSumSqr();

	    if (distance <= epsilon)
		{
		phaseflag = 0;				// first phase
		return(TRUE);
		}
	    if (distance > escape)
		{
		phaseflag = 1;				// second phase
		return(TRUE);
		}
	    return(FALSE);
	    }

	case SINFRACTAL:				// Sine
	    {
	    BigDouble	a, b;

	    if (param[2] == 0.0)
		*zBig = *qBig * zBig->CSin();
	    else
		*zBig = *qBig + zBig->CSin();
	    a = 80.0;
	    b = -80.0;

	    if (zBig->x > a || zBig->x < b || zBig->y > a || zBig->y < b)
		return(TRUE);
	    return(FALSE);
	    }

	case EXPFRACTAL:				// Exponential
	    {
	    int		compare;
	    BigDouble	a, b;

	    *zBig = *qBig * zBig->CExp();			    // Z = L*EXP(Z)
	    a = 10.0;
	    b = -10.0;

	    switch (subtype)
		{
		case 'R':
		    compare = (zBig->x > a);
		    break;
		case 'I':
		    compare = (zBig->y > a);
		    break;
		case 'M':
		    compare = (zBig->x > a || zBig->x < b || zBig->y > a || zBig->y < b);
		    break;
		default:
		    compare = (zBig->x > a);
		    break;
		}
	    if (compare)
		return(TRUE);
	    return(FALSE);
	    }

	case REDSHIFTRIDER:				// RedShiftRider    a*z^2 +/- z^n + c
	    *zBig = aBig * *zBig * *zBig + zBig->CPolynomial(*degree) * ((param[5] == 1.0) ? 1.0 : -1.0);
	    *zBig = *zBig + *qBig;
	    return (zBig->CSumSqr() > rqlim);

	case TALIS:					// Talis Power    Z = Z^N/(M + Z^(N-1)) + C
	    {
	    double	m;
	    BigComplex	z1;

	    m = param[1];
	    z1 = zBig->CPolynomial(*degree - 1);
	    *zBig = (z1 * *zBig) / (z1 + m) + *qBig;
	    return (zBig->CSumSqr() > rqlim);
	}

	case POLYNOMIAL:				// Polynomial
	    {
	    BigComplex	InitialZ = *zBig;
	    BigComplex	FinalZ = { 0.0, 0.0 };

	    for (int m = 0; m < MAXPOLYDEG; m++)
		{
		BigComplex BigComplexTemp = InitialZ;
		if (param[2 + m] != 0.0)
		    {
		    for (int k = 0; k < MAXPOLYDEG - m - 1; k++)
			BigComplexTemp *= InitialZ;
		    FinalZ += (BigComplexTemp * param[2 + m]);
		    }
		}
	    *zBig = FinalZ + *qBig;
	    return (zBig->CSumSqr() >= rqlim);
	    }

	case MANDELDERIVATIVES:				// a group of Mandelbrot Derivatives
	    return (BigRunManDerFunctions(subtype, zBig, qBig, SpecialFlag, iteration));


	case TIERAZON:					// a group of Mandelbrot Derivatives
	    return (BigRunTierazonFunctions(subtype, zBig, qBig, SpecialFlag, iteration));
	}
    return 0;
    }
