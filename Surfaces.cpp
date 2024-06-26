/*
   SURFACES.CPP a module to explore 3D surfaces

   Written in Microsoft Visual C++ by Paul de Leeuw.
*/

#include	<stdio.h>
#include	<time.h>
#include	"manp.h"
#include	"Fract.h"
#include	"resource.h"
#include	"fractype.h"
#include	"fractalp.h"
#include	"menu.h"
#include	"anim.h"
#include	"OscProcess.h"

extern	HWND	GlobalHwnd;			// This is the main windows handle
extern	COscProcess	OscProcess;

extern	double	x_rot;				// angle display plane to x axis
extern	double	y_rot;				// angle display plane to y axis
extern	double	z_rot;				// angle display plane to z axis
extern	BYTE	PerspectiveFlag;		// display using perspective
extern	int	CoordSystem;

extern	long	threshold;
extern	double	mandel_width;			/* width of display */
extern	double	hor;				/* horizontal address */
extern	double	vert;				/* vertical address */
extern	double	ScreenRatio;			// ratio of width / height for the screen
extern	int	OscPtrArray[];			// array of pointers to specific oscillators or fractal maps
extern	double	param[];
extern	WORD	type;				// fractal type
extern	int	subtype;			// A - E
extern	int	curpass, totpasses;
extern	int	OscillatorNum;
extern	int	height, xdots, ydots, width, bits_per_pixel;

static	double	xscale, yscale;
extern	double	iterations;
extern	double	VertBias;			// allow vertical stretching of the image
extern	double	zBias;				// allow stretching of the image in the z direction

extern	int	MaxDimensions;
extern	int	xAxis, yAxis, zAxis;		// numerical values for axes for chaotic oscillators
extern	int	FindCentre;			// statistical average or contained volume

extern	double 	*wpixels;			// an array of doubles holding slope modified iteration counts

extern	CTrueCol	    TrueCol;		// palette info
extern	ProcessType	OscAnimProc;
extern	RGBTRIPLE	OscBackGround;
extern	CDib	Dib;				// Device Independent Bitmap

static	double	xSum, ySum, zSum;
BOOL	RemoveHiddenPixels = TRUE;

void	CloseZvalues(void);
void	InitSurface(double c1[], int dimensions);
int	DisplaySurface(double c1[], DWORD colour, double i, int dimensions);
double	SignedPower(double, double);	// used to handle negative variables

extern	int	user_data(HWND);
extern	void	PlotExtras(void);
extern	void	PrintOsc(int);			// used for listing oscillator names in d:\temp\OscDump.txt

/**************************************************************************
	hyperbolic secant
***************************************************************************/

double	sech(double x)
    {
    return(2.0/(exp(x)+exp(-x)));
    }

/**************************************************************************
	4D Klein Bottle
	https://mathematica.stackexchange.com/questions/182436/how-to-visualize-a-4-dimensional-parametric-surface
	Alex Trounev - article
***************************************************************************/

int	Do4DKleinBottle(void)

    {
    int		i, j;
    double	c1[4], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, p, r, e, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z
    c1[3] = param[13]; // w

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    p = param[4];
    r = param[5];
    e = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 4);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = r * (cos(s * 2) * cos(t) - sin(s / 2) * sin(2 * t));
	    c1[1] = r * (sin(s * 2) * cos(t) + cos(s / 2) * sin(2 * t));
	    c1[2] = p * cos(s / 2) * (1 + e * sin(t));
	    c1[3] = p * sin(s) * (1 + e * sin(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 4) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Abundant Wave Solutions for Generalized Hietarinta Equation with Hirota's Bilinear Operator 
	Xindi Shen, Jalil Manafian, Meng Jiang, Onur Alp Ilhan, Shafik S. Shafik and Muhaned Zaidi February 4, 2022)
	https://www.researchgate.net/requests/r102631216
***************************************************************************/

int	DoAbundantWaveSolutions(void)

    {
    int		i, j;
    double	c1[3], x, y, z1, z2, z, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a1, a2, b1, b2, d1, d2, h2, e1, e2, e3, h11, h31, ColourFactor;

    double	b, n, k, c, f, s, r, q, u, p;
    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a1 = param[4];
    a2 = param[5];
    b1 = param[6];
    b2 = param[7];
    d1 = param[8];
    ColourFactor = param[9];

    d2 = 0.7; h2 = 2.0; e1 = 1.0; e2 = 2.0; t = 1.0;			// not enough params for these
    totpasses = 10;

    b = (sqr(d1) + sqr(d2));
    n = (sqr(d1) - sqr(d2));
    k = (sqr(a1) + sqr(a2));
    c = (sqr(a1) - sqr(a2));
    f = (a1 * d1 + a2 * d2);
    s = (a1 * d2 - a2 * d1);
    r = d1 * b2 - d2 * b1;
    q = b1 * d1 + b2 * d2;
    u = a1 * a2 * d1;
    p = a1 * a2 * d2;

    h11 = -0.5 * b * (h2 * s + r) / f / s;
    h31 = -(-sqr(k) * h11 + k * f * h2 + (d2 * c - 2 * u) * b2 - (d1 * c + 2 * p) * b1) / (c * n + 4 * u * d2);
    e3 = -(6 * sqr(k) + 6 * b * f) / (b * h31 - k * h11 + f * h2 - q);

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    y = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	x = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = x;
	    c1[1] = y;

	    z1 = (4 * (t * d1 + x * a1 + y * b1 + e1) * a1 + 4 * (t * (-d2) + x * a2 + y * b2 + e2) * a2);
	    z2 = (sqr(t * d1 + x * a1 + y * b1 + e1) + sqr(t * d2 + x * a2 + y * b2 + e2) + e3);

	    z = z1 / z2;
	    c1[2] = z * 250.0;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    x += ygap;
	    }
	y += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Antisymmetric Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:55)
***************************************************************************/

int	DoAntisymmetricTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    R = param[5];
    r = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * cos(s) * (a + sin(t))) * cos(t);
	    c1[1] = (R + r * cos(s) * (a + sin(t))) * sin(t);
	    c1[2] = r * sin(s) * (a + sin(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Apple I Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:50)
***************************************************************************/

int	DoAppleISurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (4 + 3.8 * cos(s));
	    c1[1] = sin(t) * (4 + 3.8 * cos(s));
	    c1[2] = (cos(s)+sin(s)-1) * (1+sin(s)) * log(1-PI * s/10)+7.5 * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Apple II Surface 
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:95)
***************************************************************************/

int	DoAppleIISurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (R1 + R2 * cos(s)) + pow((s / PI), 20);
	    c1[1] = sin(t) * (R1 + R2 * cos(s)) + 0.25 * cos(5*t);
	    c1[2] = -2.3 * log(1 - s * 0.3157) + 6 * sin(s) + 2 * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Archimedean Spiral
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:180)
***************************************************************************/

int	DoArchimedeanSpiral(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    H = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t * cos(t);
	    c1[1] = H * s;
	    c1[2] = t * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Asteroid Ellipsoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:132)
***************************************************************************/

int	DoAsteroidEllipsoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, sSinCubed, tSinCubed, tCosCubed, sCosCubed, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    tCosCubed = pow(cos(t), 3);
	    sCosCubed = pow(cos(s), 3);
	    tSinCubed = pow(sin(t), 3);
	    sSinCubed = pow(sin(s), 3);
//	    c1[0] = a * cos(s) * cos(s) * cos(s) * cos(t) * cos(t) * cos(t);
//	    c1[1] = b * sin(s) * sin(s) * sin(s) * cos(t) * cos(t) * cos(t);
//	    c1[2] = c * sin(t) * sin(t) * sin(t);
	    c1[0] = a * sCosCubed * tCosCubed;
	    c1[1] = b * sSinCubed * tCosCubed;
	    c1[2] = c * tSinCubed;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Asteroid Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:132)
***************************************************************************/

int	DoAsteroidTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * cos(s) * cos(s) * cos(s)) * cos(t);
	    c1[1] = r * sin(s) * sin(s) * sin(s);
	    c1[2] = (R + r * cos(s) * cos(s) * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Balls Cylindroid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:212)
***************************************************************************/

int	DoBallsCylindroid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, k, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    k = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * cos(t);
	    c1[1] = s * sin(t);
	    c1[2] = k * sin(t) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Banchoff Klein Bottle
	https://demonstrations.wolfram.com/BanchoffsKleinBottle/
***************************************************************************/

int	DoBanchoffKleinBottle(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
//    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    xEnd = 2 * b * c * PI;
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(s / c) * cos(s / b) * (a + cos(t)) + sin(s / b) * sin(t) * cos(t);
	    c1[1] = sin(s / c) * cos(s / b) * (a + cos(t)) + sin(s / b) * sin(t) * cos(t);
	    c1[2] = -sin(s / b) * (a + cos(t)) + cos(s / b) * sin(t) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bell
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:123)
***************************************************************************/

int	DoBell(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    r = sqrt (t*t + s*s);
	    c1[0] = t;
	    c1[1] = b * exp(-(a*a*r*r));
	    c1[2] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bell Polar
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:124)
***************************************************************************/

int	DoBellPolar(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, r, S, T, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    S = s * cos(t);
	    T = s * sin(t);
	    r = sqrt (T*T + S*S);
	    c1[0] = S;
	    c1[1] = b * exp(-(a*a*r*r));
	    c1[2] = T;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bell Wave
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:125)
***************************************************************************/

int	DoBellWave(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    r = sqrt(t * t + s * s);
	    c1[0] = s;
	    c1[1] = cos(c*r) * b * exp(-(a*a*r*r));
	    c1[2] = t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bent Horns
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:45)
***************************************************************************/

int	DoBentHorns(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (2 + cos(t)) * (s / 3 - sin(s));
	    c1[1] = (2 + cos(t + 2*PI / 3)) * (cos(s) - 1);
	    c1[2] = (2 + cos(t - 2*PI / 3)) * (cos(s) - 1);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bicorn Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:173)
***************************************************************************/

int	DoBicornSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(s) * cos(t);
	    c1[1] = COSSQR(s) * (2 + cos(s))/(3 + SINSQR(s));
	    c1[2] = sin(s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bicorn Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:156)
***************************************************************************/

int	DoBicornTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * COSSQR(s) * (2 + cos(s))/(3 + SINSQR(s))) * cos(t);
	    c1[1] = r + sin(s);
	    c1[2] = (R + r * COSSQR(s) * (2 + cos(s))/(3 + SINSQR(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bicorn Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:157)
***************************************************************************/

int	DoBicornTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * sin(s)) * cos(t);
	    c1[1] = r * COSSQR(s) * (2 + cos(s))/(3 + SINSQR(s));
	    c1[2] = (R + r * sin(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Binormal Surface
	https://resources.wolframcloud.com/FunctionRepository/resources/BinormalSurface
***************************************************************************/

int	DoBinormalSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, Root5, CosS, SinS, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    Root5 = sqrt(5.0);
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    CosS = cos(s);
	    SinS = sin(s);
	    c1[0] = CosS + 2 * t * SinS / Root5;
	    c1[1] = -2 * t * CosS / Root5 + SinS;
	    c1[2] = 2 * s + (t * CosS * CosS + SinS * SinS) / Root5;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Body of Revolution
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:80)
***************************************************************************/

#define FUNCTION(x) (sqr(sin(x))+1.0)

int	DoBodyRevolution(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = FUNCTION(t) * sin(s);
	    c1[2] = FUNCTION(t) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bohemian Dome
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:1)
***************************************************************************/

int	DoBohemianDome(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = a * cos(t);
	    c1[0] = b * cos(s) + a * sin(t);
	    c1[2] = c  * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bonan-Jeener-Klein Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:102)
***************************************************************************/

int	DoBonanJeenerKleinSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, M, T, a, W, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    M = param[4];
    T = param[5];
    a = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    W = sin((M - 1) * t) + T;
	    c1[0] = M * cos(t) - cos(M*t) - ((M - 1)/M) * W * sin((M + 1) * t/2) * cos(s);
	    c1[1] = W * sin(s);
	    c1[2] = M * sin(t) - sin(M*t) + ((M - 1)/M) * W * cos((M + 1) * t/2) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Borromean Rings
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:153)
***************************************************************************/

int	DoBorromeanRingsSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
									// Ring 1
	    c1[0] = (R + r * cos(s)) * cos(t);
	    c1[1] = r * sin(s) + 3 * sin(3*t + PI/2);
	    c1[2] =  (R + r * cos(s)) * sin(t) + R * sqrt(3.0)/3;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
									// Ring 2
	    c1[0] = (R + r * cos(s)) * cos(t) + 0.5 * R;
	    c1[1] = r * sin(s) + 3 * sin(3*t + PI/2);
	    c1[2] = (R + r * cos(s)) * sin(t) - R * sqrt(3.0)/6;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
									// Ring 3
	    c1[0] = (R + r * cos(s)) * cos(t) - 0.5 * R;
	    c1[1] = r * sin(s) + 3 * sin(3 * t + PI/2);
	    c1[2] =  (R + r * cos(s)) * sin(t) - R * sqrt(3.0)/6;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bour's Minimal Surface
	Attributed to Edmond Bour Equations by Roger Bagula 
	Graphics by Paul Bourke	February 2003 
	http://paulbourke.net/geometry/bour/
	Also:
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:191)
***************************************************************************/

int	DoBourMinimalSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    n = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = pow(s, n-1) * cos((n-1)*t)/(2*(n-1)) - pow(s, n+1) * cos((n+1)*t)/(2*(n+1));
	    c1[1] = pow(s, n-1) * sin((n-1)*t)/(2*(n-1)) + pow(s, n+1) * sin((n+1)*t)/(2*(n+1));
	    c1[2] = pow(s, n) * cos(n*t) / n;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bow Curve
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:164)
***************************************************************************/

int	DoBowCurve(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, T, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    T = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (2 + T * sin(2*PI*t)) * sin(4*PI*s);
	    c1[1] = (2 + T * sin(2*PI*t)) * cos(4*PI*s);
	    c1[2] = T * cos(2*PI*t) + 3 * cos(2*PI*s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Graphics by Paul Bourke February 2005
	Attributed to Roger Bagula 
	http://paulbourke.net/geometry/toroidal/
	Also:
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:237)
***************************************************************************/

int	DoBowTie(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, Root2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    Root2 = sqrt(2.0);

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t) / (Root2 + cos(s));
	    c1[1] = sin(t) / (Root2 + sin(s));
	    c1[2] = cos(t) / (1 + Root2) ;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Boy's Surface is an immersion of the real projective plane, RP^2, in R^3. It was constructed by Werner Boy, working under David Hilbert, in 1901. 
    It is continuous and has threefold symmetry, self-intersecting at a triple point. The Boy's Surface shown here is parametrized by three homogeneous 
    polynomials of degree four which are defined on the sphere, S^2. These polynomials are:

    F = Sqrt(3)/2 [(y^2 - z^2) (x^2 + y^2 + z^2) + zx (z^2 - x^2) + xy (y^2 - x^2)]
    G = 1/2 [(2x^2 - y^2 - z^2) (x^2 + y^2 + z^2) + 2yz (y^2 - z^2) + zx (x^2 - z^2) + xy (y^2 - x^2)]
    H = 1/8 (x + y + z) [(x + y + z)^3 + 4 (y - x) (z - y) (x - z)]

    where

    x = 0.577295 Cos(s) - 0.577295 Cos(t) Sin(s) - 0.3950426 Sin(s) Sin(t)
    y = 0.577295 Cos(s) + 0.577295 Cos(t) Sin(s) - 0.333365 Sin(s) Sin(t)
    z = 0.57746 Cos(s) + 0.728199 Sin(s) Sin(t)

    s ranges from 0 to Pi/2 and t ranges from 0 to 2 Pi 

    http://www.math.smith.edu/~patela/boysurface/AmyJenny/boy_surface.html

    See also http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:2)
**************************************************************************/

int	DoBoysSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, x, y, z, xfactor, yfactor, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z
    xfactor = param[0];
    yfactor = param[1];
    ColourFactor = param[2];
    totpasses = 10;

    ygap = PI * yfactor / iterations / 2.0;
    xgap = PI * 2.0 * xfactor / iterations;
    t = 0.0;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = 0.0;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    x = 0.577295 * cos(s) - 0.577295 * cos(t) * sin(s) - 0.3950426 * sin(s) * sin(t);
	    y = 0.577295 * cos(s) + 0.577295 * cos(t) * sin(s) - 0.333365 * sin(s) * sin(t);
	    z = 0.57746 * cos(s) + 0.728199 * sin(s) * sin(t);
	    c1[0] = sqrt(3.0)/2.0 * ((sqr(y) - sqr(z)) * (sqr(x) + sqr(y) + sqr(z)) + z * x * (sqr(z) - sqr(x)) + x * y * (sqr(y) - sqr(x)));
	    c1[1] = 1.0/2.0 * ((2 * sqr(x) - sqr(y) - sqr(z)) * (sqr(x) + sqr(y) + sqr(z)) + 2 * y * z * (sqr(y) - sqr(z)) + z * x * (sqr(x) - sqr(z)) + x * y * (sqr(y) - sqr(x)));
	    c1[2] = 1.0/8.0 * (x + y + z) * (pow((x + y + z), 3) + 4.0 * (y - x) * (z - y) * (x - z));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Braided Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:103)
***************************************************************************/

int	DoBraidedTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, n, r, R, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    a = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = r * cos(s) * cos(t) + R * cos(t) * (1 + a * cos(n*t));
	    c1[2] = 2.5 * (r * sin(s) + a * sin(n*t));
	    c1[0] = r * cos(s) * sin(t) + R * sin(t) * (1 + a * cos(n*t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Breather Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:169)
***************************************************************************/

int	DoBreatherSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, r, w, d, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    r = 1 - a*a;
    w = sqrt(r);
    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    d = a * (sqr(w * cosh(a*t)) + sqr(a * sin(w*s)));
	    c1[0] = -t + (2 * r * cosh(a*t) * sinh(a*t)/d);
	    c1[1] = 2 * w * cosh(a*t) * (-(w * cos(s) * cos(w*s)) - (sin(s) * sin(w*s)))/d;
	    c1[2] = 2 * w * cosh(a*t) * (-(w * sin(s) * cos(w*s)) + (cos(s) * sin(w*s)))/d;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Bullet Nose
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:176)
***************************************************************************/

int	DoBulletNose(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, cutoff, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    cutoff = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(s) * cos(t);
	    c1[1] = -b / tan(s);
	    if (c1[1] < cutoff)						// infinitely negative so set a limit somewhere
		c1[1] = cutoff;
	    c1[2] = a * cos(s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cardioid Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:158)
***************************************************************************/

int	DoCardioidTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (2 * cos(s) - cos(2*s))) * cos(t);
	    c1[1] = r * (2 * sin(s) - sin(2*s));
	    c1[2] = (R + r * (2 * cos(s) - cos(2*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cardioid Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:159)
***************************************************************************/

int	DoCardioidTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (2 * sin(s) - sin(2*s))) * cos(t);
	    c1[1] = r * (2 * cos(s) - cos(2*s));
	    c1[2] = (R + r * (2 * sin(s) - sin(2*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cassinian Oval Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:160)
***************************************************************************/

int	DoCassinianOvalTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, b, M, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    b = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    M = 2 * a*a * cos(2*s) + 2 * sqrt((- a*a*a*a + b*b*b*b) + a*a*a*a * COSSQR(2*s));
	    c1[0] = (R + sqrt(M / 2) * cos(s)) * cos(t);
	    c1[1] = sqrt(M / 2) * sin(s);
	    c1[2] = (R + sqrt(M / 2)*  cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cassinian Oval Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:161)
***************************************************************************/

int	DoCassinianOvalTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, b, M, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    b = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// patss in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    M = 2 * a*a * cos(2*s) + 2 * sqrt((- a*a*a*a + b*b*b*b) + a*a*a*a * COSSQR(2*s));
	    c1[0] = (R + sqrt(M / 2) * sin(s)) * cos(t);
	    c1[1] = sqrt(M / 2) * cos(s);
	    c1[2] = (R + sqrt(M / 2) * sin(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Catenoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:19)
***************************************************************************/

int	DoCatenoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = c * cosh(s/c) * cos(t);
	    c1[2] = c * cosh(s/c) * sin(t);
	    c1[0] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Catalan´s Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:25)
***************************************************************************/

int	DoCatalansSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t - sin(t) * cosh(s);
	    c1[1] = 1 - cos(t) * cosh(s);
	    c1[2] = -4 * sin(t/2) * sinh(s/2);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Catalan´s Surface II
	Jurgen Meier page: 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:216)
***************************************************************************/

int	DoCatalansSurfaceII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * (2 * t - (1 + 2 * s * s) * sin(2 * t));
	    c1[1] = a * (1 + 2 * s * s) * cos(2 * t);
	    c1[2] = 4 * a * s * sin(t) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cayley Surface
	Jurgen Meier page: 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:189)
***************************************************************************/

int	DoCayleySurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s + t;
	    c1[1] = (sqr(s) + sqr(t)) / 2 + a * (t - s);
	    c1[2] = (s*s*s + t*t*t) / 3 + a * (sqr(t) - sqr(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cissoid Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:148)
***************************************************************************/

int	DoCissoidCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + a * s*s / (s*s + 1)) * cos(t);
	    c1[1] = a*s*s*s / (s*s + 1);
	    c1[2] = (R + a * s*s / (s*s + 1)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Clawson Bottle 4D Volume
	Jurgen Meier page 2001
	http://www.3d-meier.de/ 
***************************************************************************/

int	DoClawsonBottle4D(void)

    {
    int		i, j;
    double	c1[4], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, w, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z
    c1[3] = param[13]; // w

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    w = 1.0;
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 4);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;

	    c1[0] = (sin(s) * sin(t) - sin(s * 2) * cos(t)) * sqrt(0.5) / (1.0 + w);
	    c1[1] = cos(s) / sin(t) / (1 + w);
	    c1[2] = cos(s / 2) * cos(t) / (1.0 + w);
	    c1[3] = (sin(s) * sin(t) + sin(s / 2) * cos(t)) / sqrt(2);
	    w = c1[3];
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 4) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cone (Body of Revolution)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:81)
***************************************************************************/

int	DoCone(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = a * t * sin(s);
	    c1[2] = a * t * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cornucopia
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:3)
***************************************************************************/

int	DoCornucopia(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = exp(b*s) * cos(s) + exp(a*s) * cos(t) * cos(s);
	    c1[2] = exp(b*s) * sin(s) + exp(a*s) * cos(t) * sin(s);
	    c1[0] = exp(a*s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cosine Surface I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:35)
***************************************************************************/

int	DoCosineSurfaceI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t);
	    c1[1] = cos(s);
	    c1[2] = cos(s+t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cosine Surface II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:179)
***************************************************************************/

int	DoCosineSurfaceII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t);
	    c1[1] = -COSSQR(s+t);
	    c1[2] = cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cosine Waves
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:121)
***************************************************************************/

int	DoCosineWaves(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = a * cos(b * sqrt(t*t + s*s));
	    c1[2] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Crescent
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:48)
***************************************************************************/

int	DoCrescent(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (a + sin(b*PI*t) * sin(b*PI*s)) * sin(c*PI*s);
	    c1[1] = (a + sin(b*PI*t) * sin(b*PI*s)) * cos(c*PI*s);
	    c1[2] = cos(b*PI*t) * sin(b*PI*s) + 4 * s - 2;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cross Cup
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:4)
***************************************************************************/

int	DoCrossCup(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = 1 - t*t + t*t * sin(s) * sin(s);
	    c1[0] = t*t * sin(s) * sin(s) + 2*t*t * sin(s) * cos(s);
	    c1[2] = sqrt((1-t*t) / 2) * t * (sin(s) + cos(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Crossed Trough Surface 
	Jurgen Meier page: 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:221)
***************************************************************************/

int	DoCrossedTrough(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * sin(t);
	    c1[1] = s * cos(t);
	    c1[2] = c * s * s * sqr(sin(2 * t)) / 4;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }
/**************************************************************************
    Cuspidal Beaks
    Jurgen Meier page: 2001
    http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:195)
***************************************************************************/

int	DoCuspidalBeaks(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = 2 * pow(t, 3) - s * s * t;
	    c1[2] = 3 * pow(t, 4) + s * s * t * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Cuspidal Butterfly
    Jurgen Meier page: 2001
    http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:197)
***************************************************************************/

int	DoCuspidalButterfly(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = 5 * pow(t, 4) + 2 * s * t;
	    c1[2] = 4 * pow(t, 5) + s * t * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Cuspidal Edge
    Jurgen Meier page: 2001
    http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:194)
***************************************************************************/

int	DoCuspidalEdge(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = t * t;
	    c1[2] = pow(t, 3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:96)
***************************************************************************/

int	DoCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    r = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = r * sin(t);
	    c1[0] = r * cos(t);
	    c1[2] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Cymbelloid Diatom
	JANICE L. PAPPAS 22 March 2005
	https://www.academia.edu/18157685/THEORETICAL_MORPHOSPACE_AND_ITS_RELATION_TO_FRESHWATER_GOMPHONEMOID_CYMBELLOID_DIATOM_BACILLARIOPHYTA_LINEAGES
***************************************************************************/

int	DoCymbelloidDiatom(void)

    {
    int		i, j;
    double	c1[3], s, t, st, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    st = sin(t / 2);
	    c1[1] = 3 * st;
	    c1[0] = sech(1.2 * t) * tanh(10 * s) - sech(t * t);
	    c1[2] = st;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Dini's Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:5)
***************************************************************************/

int	DoDinisSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, start, end, angle, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    start = param[0];
    end = param[1];
    angle = param[2];
    a = param[3];
    b = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (end - start) / iterations;
    xgap = angle / iterations;
    t = 0.0;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = start;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) * sin(s);
	    c1[2] = a * sin(t) * sin(s);
	    c1[1] = a * (cos(s) + log(tan(s / 2))) + b * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Double Ball
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:185)
***************************************************************************/

int	DoDoubleBall(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, d, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    d = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
									// Ball 1:
	    c1[0] = R1 * sin(t) * cos(s);
	    c1[1] = R1 * cos(t);
	    c1[2] = R1 * sin(t) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
									// Ball 2:
	    c1[0] = R2 * sin(t) * cos(s+PI);
	    c1[1] = R2 * cos(t) + d;
	    c1[2] = R2 * sin(t) * sin(s+PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Double Cone
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:106)
***************************************************************************/

int	DoDoubleCone(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * cos(t);
	    c1[1] = (s - 1) * cos(t + 2*PI/3);
	    c1[2] = (1 - s) * cos(t - 2*PI/3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Drop of Water
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:40)
***************************************************************************/

int	DoDropWater(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[2] = a * (b - cos(t)) * sin(t) * cos(s);
	    c1[1] = a * (b - cos(t)) * sin(t) * sin(s);
	    c1[0] = cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Dupin Cyclide
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:64)
***************************************************************************/

int	DoDupinCyclide(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, d, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    d = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    h = a - c * cos(t) * cos(s);
	    c1[0] = (d * (c - a * cos(t) * cos(s)) + b*b * cos(t))/h;
	    c1[1] = (b * sin(t) * (a - d * cos(s)))/h;
	    c1[2] = b * sin(s) * (c * cos(t) - d)/h;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Egg (Body of Revolution)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:83)
***************************************************************************/

int	DoEgg(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    xEnd = a;
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = c * sqrt(t * (t - a) * (t - b)) * sin(s);
	    c1[1] = t;
	    c1[2] = c * sqrt(t * (t - a) * (t - b)) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Eight Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:37)
***************************************************************************/

int	DoEightSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * sin(2*s);
	    c1[1] = sin(t) * sin(2*s);
	    c1[2] = sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Eight Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:63)
***************************************************************************/

int	DoEightTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (c + sin(s) * cos(t) - sin(2 * s) * sin(t)/2);
	    c1[1] = sin(t) * sin(s) + cos(t) * sin(2 * s)/2;
	    c1[2] = sin(t) * (c + sin(s) * cos(t) - sin(2 * s) * sin(t)/2);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Ellipsoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:36)
***************************************************************************/

int	DoEllipsoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) * sin(s);
	    c1[1] = b * sin(t) * sin(s);
	    c1[2] = c * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Elliptical Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:65)
***************************************************************************/

int	DoEllipticalTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (c + cos(s)) * cos(t);
	    c1[1] = (c + cos(s)) * sin(t);
	    c1[2] = sin(s) + cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Enneper´s Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:6)
***************************************************************************/

int	DoEnnepersSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t - (t*t*t/3) + t*s*s;
	    c1[1] = s - (s*s*s/3) + t*t*s;
	    c1[2] = t*t - s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Enneper´s Surface II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:238)
***************************************************************************/

int	DoEnnepersSurfaceII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = exp(s) / 6 * (3 * cos(t) - exp(2 * s) * cos(3 * t));
	    c1[1] = exp(s) / 6 * (-3 * sin(t) - exp(2 * s) * sin(3 * t));
	    c1[2] = exp(2 * s) / 2 * cos(2 * t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Enneper´s Surface III
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:239)
***************************************************************************/

int	DoEnnepersSurfaceIII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = -s * (-6 + s*s - 3 * t*t) / 6 / sqrt(2.0);
	    c1[1] = t * (-6 + t*t - 3 * s*s) / 6 / sqrt(2.0);
	    c1[2] = (s*s - t*t) / 2;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Enneper´s Surface (polar)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:127)
***************************************************************************/

int	DoEnnepersSurfacePolar(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, T, S, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    S = s * cos(t);
	    T = s * sin(t);
	    c1[0] = S - S*S*S / 3 + S*T*T;
	    c1[1] = T - T*T*T / 3 + T*S*S;
	    c1[2] = S*S - T*T;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Epicycloid Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:144)
***************************************************************************/

int	DoEpicycloidCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, H, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    H = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r) * cos(t) - h * cos(((R + r)/r) * t);
	    c1[1] = H * s;
	    c1[2] = (R + r) * sin(t) - h * sin(((R + r)/r) * t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Epicycloid Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:139)
***************************************************************************/

int	DoEpicycloidTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R, r, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R = param[5];
    r = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + (R + r) * cos(s) - h * cos(((R + r)/r) * s)) * cos(t);
	    c1[1] = (R + r) * sin(s) - h * sin(((R + r)/r) * s);
	    c1[2] = (R1 + (R + r) * cos(s) - h * cos(((R + r)/r) * s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Epicycloid Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:140)
***************************************************************************/

int	DoEpicycloidTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R, r, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R = param[5];
    r = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + (R + r) * sin(s) - h * sin(((R + r)/r) * s)) * cos(t);
	    c1[1] = (R + r) * cos(s) - h * cos(((R + r)/r) * s);
	    c1[2] = (R1 + (R + r) * sin(s) - h * sin(((R + r)/r) * s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Evolved Torus
	https://www.desmos.com/3d/773234b356?lang=pt-BR
***************************************************************************/

int	DoEvolvedTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, n, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    n = param[4];
    a = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(n * t) * cos(n * s) + (cos(10 * n * t) * (sin(n * s) + 3) + 7) * (-sin(n * t));
	    c1[1] = sin(n * t) * cos(n * s) + (cos(10 * n * t) * (sin(n * s) + 3) + 7) * (cos(n * t));
	    c1[2] = (sin(10 * n * t)) * (sin(n * s) - a);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Evolvent Conoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:188)
***************************************************************************/

int	DoEvolventConoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, l, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    l = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * l;
	    c1[1] = a * (cos(t) + t * sin(t));
	    c1[2] = a * s * (sin(t) - t * cos(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Facing Snail
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:98)
***************************************************************************/

int	DoFacingSnail(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(s) * cos(t);
	    c1[1] = sin(s) * cos(t);
	    c1[2] = (1 - a*s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Fano Planes
	https://paulbourke.net/geometry/fanoplanes/
	Roger Bagula - Paul Bourke
***************************************************************************/

int	DoFanoPlanes(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, sqrt3, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    sqrt3 = sqrt(3.0);

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    a = 2 * PI * (s - t / 3);
	    b = 2 * PI * (-s - t / 3);
	    c = 2 * PI * (s - sqrt3);

	    c1[0] = cos(a) * cos(b) * cos(c);
	    c1[1] = cos(2 * PI * (s - t / 3 + 0.666667)) * cos(2 * PI * (-s - t / 3 + 0.666667)) * cos(2 * PI * (-s - t + 1));
	    c1[2] = cos(2 * PI * (s - t / 3 - 0.666667)) * cos(2 * PI * (-s - t / 3 - 0.666667)) * cos(2 * PI * (-s + t + 1));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Fermat Spiral
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:182)
***************************************************************************/

int	DoFermatSpiral(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    H = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    if (t > 0)
		c1[0] = sqrt(fabs(t)) * cos(t);
	    else
		c1[0] = -sqrt(fabs(t)) * cos(t);
	    c1[1] = H * s;
	    c1[2] = sqrt(fabs(t)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Fish Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:43)
***************************************************************************/

int	DoFishSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (cos(t) - cos(2*t)) * cos(s)/4;
	    c1[1] = (sin(t) - sin(2*t)) * sin(s)/4;
	    c1[2] = cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Folium
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:73)
***************************************************************************/

int	DoFolium(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (2*s/PI - tanh(s));
	    c1[1] = cos(t + 2*PI/3)/cosh(s);
	    c1[2] = cos(t - 2*PI/3)/cosh(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Four Intersecting Discs 
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:231)
***************************************************************************/

int	DoFourIntersectingDiscs(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t * cos(s) - 0.5 * t * t * cos(2 * s);
	    c1[1] = -t * sin(s) - 0.5 * t * t * sin(2 * s);
	    c1[2] = 4 * t * t * cos(3 * s / 2) / 3;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Fresnel Elasticity Area
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:151)
***************************************************************************/

/*
#define	SINSQR(x)   (sin(x)*sin(x))
#define	COSSQR(x)   (cos(x)*cos(x))

int	DoFresnelElasticityArea(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1; 
	    c1[0] = (c*c/a) * ((COSSQR(t))	    / (SINSQR(s) + c*c * COSSQR(s) * (COSSQR(t) / a*a + SINSQR(t) / b*b)));
	    c1[1] = (c*c/b) * ((sin(t) * cos(s))    / (SINSQR(s) + c*c * COSSQR(s) * (COSSQR(t) / a*a + SINSQR(t) / b*b)));
	    c1[2] = c	    * ((sin(s))		    / (SINSQR(s) + c*c * COSSQR(s) * (COSSQR(t) / a*a + SINSQR(t) / b*b)));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }
*/

/**************************************************************************
	Funnel
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:23)
***************************************************************************/

int	DoFunnel(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t * cos(s);
	    c1[1] = t * sin(s);
	    c1[2] = log(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gaussian Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:150)
***************************************************************************/

int	DoGaussianCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    b = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + b * exp( -(a*a*s*s))) * cos(t);
	    c1[1] = s;
	    c1[2] = (R + b * exp( -(a*a*s*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gear Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:177)
***************************************************************************/

int	DoGearTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, b, n, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    b = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    r = a + tanh(b * sin(n*s))/b;
	    c1[0] = (R + r * cos(s)) * cos(t);
	    c1[1] = r * sin(s);
	    c1[2] = (R + r * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gerono Lemniscate Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:162)
***************************************************************************/

int	DoGeronoLemniscateTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * sin(s)) * cos(t);
	    c1[1] = r * sin(s) * cos(s);
	    c1[2] = (R + r * sin(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gerono Lemniscate Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:163)
***************************************************************************/

int	DoGeronoLemniscateTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * sin(s) * cos(s)) * cos(t);
	    c1[1] = r * sin(s);
	    c1[2] = (R + r * sin(s) * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Ghost Plane
	Contributed by Roger Bagula Graphics by Paul Bourke May 2003 
	http://paulbourke.net/geometry/ghostplane/
***************************************************************************/

int	DoGhostPlane(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, PlaneLimit;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    PlaneLimit = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * sinh(s) / (cosh(s) - cos(t));
	    c1[1] = cos(t) * sinh(t) / (cosh(s) - cos(t));
	    c1[2] = sin(t);
								// surface is infinite, so put boundaries on it
	    if (c1[0] < -PlaneLimit * ScreenRatio) c1[0] = -PlaneLimit * ScreenRatio; 
	    if (c1[0] > PlaneLimit * ScreenRatio) c1[0] = PlaneLimit * ScreenRatio; 
	    if (c1[1] < -PlaneLimit) c1[1] = -PlaneLimit; 
	    if (c1[1] > PlaneLimit) c1[1] = PlaneLimit; 
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Goblet
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:232)
***************************************************************************/

int	DoGoblet(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(s) * cos(2 * t);
	    c1[1] = sin(s) * cos(2 * t);
	    c1[2] = -2 * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gomphonemoid Diatom
	JANICE L. PAPPAS 22 March 2005
	https://www.academia.edu/18157685/THEORETICAL_MORPHOSPACE_AND_ITS_RELATION_TO_FRESHWATER_GOMPHONEMOID_CYMBELLOID_DIATOM_BACILLARIOPHYTA_LINEAGES
***************************************************************************/

int	DoGomphonemoidDiatom(void)

    {
    int		i, j;
    double	c1[3], s, t, st, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    st = sin(t / 2);
	    c1[1] = 3 * st - sech(t * t);
	    c1[0] = sech(1.2 * t) * tanh(10 * s);
	    c1[2] = st;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gomphocymbelloid Diatom
	JANICE L. PAPPAS 22 March 2005
	https://www.academia.edu/18157685/THEORETICAL_MORPHOSPACE_AND_ITS_RELATION_TO_FRESHWATER_GOMPHONEMOID_CYMBELLOID_DIATOM_BACILLARIOPHYTA_LINEAGES
***************************************************************************/

int	DoGomphocymbelloidDiatom(void)

    {
    int		i, j;
    double	c1[3], s, t, st, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    st = sin(t / 2);
	    c1[1] = 4 * st - cos(t / 2);
	    c1[0] = sech(1.2 * t) * tanh(10 * s) - sech(t / 4);
	    c1[2] = st;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gray's Klein Bottle
	Inspired by Roger Bagula, Graphics by Paul Bourke September 2012
	http://paulbourke.net/geometry/toroidal/
***************************************************************************/

int	DoGraysKleinbottle(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, n, m, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    n = param[5];
    m = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (a + cos(n*t/2.0) * sin(s) - sin(n*t/2.0) * sin(2*s)) * cos(m*t/2.0);
	    c1[1] = (a + cos(n*t/2.0) * sin(s) - sin(n*t/2.0) * sin(2*s)) * sin(m*t/2.0);
	    c1[2] = sin(n*t/2.0) * sin(s) + cos(n*t/2.0) * sin(2*s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Guimard Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:66)
***************************************************************************/

int	DoGuimardSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = ((1 - t) * a + t*b) * cos(s);
	    c1[1] = b * t * sin(s);
	    c1[2] = c * t * sin(s) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Gyro
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:143)
***************************************************************************/

int	DoGyro(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = pow((fabs(t)-1), 2) * cos(s);
	    c1[1] = t;
	    c1[2] = pow((fabs(t)-1), 2) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Handkerchief Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:7)
***************************************************************************/

int	DoHandkerchiefSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = t;
	    c1[1] = s;
	    c1[0] = t*t*t / 3 - t*s*s + 2*(t*t - s*s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Heart Surface I
	https://paulbourke.net/geometry/heart/
	Roger Bagula - Paul Bourke
***************************************************************************/
/*
int	DoHeartSurfaceI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    r = 4 * sqrt(fabs(1 - t * t)) * pow(fabs(sin(s)), s);
	    c1[2] = r;
	    c1[0] = r * sin(s);
	    c1[1] = -r * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }
*/

/**************************************************************************
	Helicoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:20)
***************************************************************************/

int	DoHelicoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[1] = t * cos(s);
	    c1[2] = t * sin(s);
	    c1[0] = c * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Henneberg´s Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:28)
***************************************************************************/

int	DoHennebergsSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[1] = 2 * cos(s) * sinh(t) - 0.667 * cos(3 * s) * sinh(3 * t);
	    c1[2] = 2 * sin(s) * sinh(t) + 0.667 * sin(3 * s) * sinh(3 * t);
	    c1[0] = 2 * cos(2 * s) * cosh(2 * t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hexagonal Drum
	Graphics by Paul Bourke
	July 2012
	Equations by Roger Bagula
	http://paulbourke.net/geometry/hexdrum/
***************************************************************************/

#define	    H(t) (fabs(cos(t)) + fabs(cos(t+2*PI/3)))
#define	    I(t) (fabs(cos(t+2*PI/3)) + fabs(cos(t-2*PI/3)))
#define	    J(t) (fabs(cos(t-2*PI/3)) + fabs(cos(t)))

int	DoHexagonalDrum(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = 16 * cos(t) * sin(s) / ((H(t) + I(t) + J(t)) * (H(s) + I(s) + J(s)));
	    c1[1] = 16 * sin(t) * sin(s) / ((H(t) + I(t) + J(t)) * (H(s) + I(s) + J(s)));
	    c1[2] = 4 * cos(t) / (H(t) + I(t) + J(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hole Discus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:112)
***************************************************************************/

int	DoHoleDiscus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[1] = sin(t) / (a + sin(s));
	    c1[2] = cos(s) / b;
	    c1[0] = cos(t) / (a + sin(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hole Disk
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:107)
***************************************************************************/

int	DoHoleDisk(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = sin(t)/(sqrt(2.0) + sin(s));
	    c1[1] = cos(t)/(sqrt(2.0) + sin(s));
	    c1[2] = cos(t)/(1 + sqrt(2.0));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Horn
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:44)
***************************************************************************/

int	DoHorn(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (a + t * cos(s)) * sin(b * PI * t);
	    c1[1] = (a + t * cos(s)) * cos(b * PI * t) + c * t;
	    c1[2] = t * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hornlet Surface 
	Jurgen Meier page: 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:219)
***************************************************************************/

int	DoHornlet(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (a + sin(b * PI * s) * sin(b * PI * t)) * sin(c * PI * t);
	    c1[1] = (a + sin(b * PI * s) * sin(b * PI * t)) * cos(c * PI * t);
	    c1[2] = cos(b * PI * s) * sin(b * PI * t) + h * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hyperbolic Helicoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:22)
***************************************************************************/

int	DoHyperbolicHelicoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = sinh(s) * cos(a * t)/(1+cosh(t) * cosh(s));
	    c1[0] = sinh(s) * sin(a * t)/(1+cosh(t) * cosh(s));
	    c1[1] = cosh(s) * sinh(t)/(1+cosh(t) * cosh(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hyperbolic Octahedron
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:46)
***************************************************************************/

int	DoHyperbolicOctahedron(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = pow(cos(t) * cos(s), 3);
	    c1[0] = pow(sin(t) * cos(s), 3);
	    c1[1] = pow(sin(s), 3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hyperbolic Paraboloid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:94)
***************************************************************************/

int	DoHyperbolicParaboloid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = t;
	    c1[0] = s * a;
	    c1[1] = t * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hyperbolic Spiral
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:181)
***************************************************************************/

int	DoHyperbolicSpiral(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    H = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) / t;
	    c1[1] = H * s;
	    c1[2] = sin(t) / t;
	    if (t != 0.0)
		if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		    return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hyperboloid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:26)
***************************************************************************/

int	DoHyperboloid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) * cosh(s);
	    c1[1] = a * sin(t) * cosh(s);
	    c1[2] = b * sinh(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hypocycloid Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:145)
***************************************************************************/

int	DoHypocycloidCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, H, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    H = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R - r) * cos(t) + h * cos(((R - r)/r) * t);
	    c1[1] = H * s;
	    c1[2] = (R - r) * sin(t) - h * sin(((R - r)/r) * t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hypocycloid Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:141)
***************************************************************************/

int	DoHypocycloidTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R, r, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R = param[5];
    r = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + (R - r) * cos(s) + h * cos(((R - r)/r) * s)) * cos(t);
	    c1[1] = (R - r) * sin(s) - h * sin(((R - r)/r) * s);
	    c1[2] = (R1 + (R - r) * cos(s) + h * cos(((R - r)/r) * s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Hypocycloid Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:142)
***************************************************************************/

int	DoHypocycloidTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R, r, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R = param[5];
    r = param[6];
    h = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + (R - r) * sin(s) - h * sin(((R - r)/r) * s)) * cos(t);
	    c1[1] = (R - r) * cos(s) + h * cos(((R - r)/r) * s);
	    c1[2] = (R1 + (R - r) * sin(s) - h * sin(((R - r)/r) * s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Immersion of the Klein Bottle
	https://www.geogebra.org/m/EPyjX7Ep#material/hthQ8yfN
	Tim Brzezinski
***************************************************************************/

int	DoImmersionOfTheKleinBottle(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    r = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (r + cos(s / 2) * sin(t) - sin(s / 2) * sin(2 * t)) * cos(s);
	    c1[1] = (r + cos(s / 2) * sin(t) - sin(s / 2) * sin(2 * t)) * sin(s);
	    c1[2] = sin(s / 2) * sin(t) + cos(s / 2) * sin(2 * t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Insulator (Body of Revolution)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:82)
***************************************************************************/

int	DoInsulator(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = (a + b * sin(c*t*2*PI)) * sin(s);
	    c1[2] = (a + b * sin(c*t*2*PI)) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Jet Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:39)
***************************************************************************/

int	DoJetSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (1 - cosh(t)) * sin(t) * cos(s)/2;
	    c1[1] = (1 - cosh(t)) * sin(t) * sin(s)/2;
	    c1[2] = cosh(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Jeener-Klein Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:101)
***************************************************************************/

int	DoJeenerKleinSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, S, T, a, W, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    S = param[4];
    T = param[5];
    a = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    W = ((S + 1)/4) * cos((S + 1) *  t + PI/T) + sqrt(a);
	    c1[0] = S * cos(t) + cos(S*t) - W * sin((S - 1) * t/2) * cos(s);
	    c1[1] = W * sin(s);
	    c1[2] = S * sin(t) - sin(S*t) - W * cos((S - 1) * t/2) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kai-Wing Fung 2 Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:234)
***************************************************************************/

int	DoKaiWingFung2Surface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = -s / 2 + 0.5 * sin(s) * cosh(t) - 0.5 * sin(s) * sinh(t);
	    c1[1] = -t / 2 + 0.5 * cos(s) * cosh(t) - 0.5 * cos(s) * sinh(t);
	    c1[2] = 2 * (cosh(t / 2) - sinh(t / 2)) * sin(s / 2);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kappa Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:175)
***************************************************************************/

int	DoKappaSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, cutoff, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    cutoff = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(s) * cos(t);
	    c1[1] = -a * cos(s)/tan(s);
	    if (c1[1] < cutoff)						// infinitely negative so set a limit somewhere
		c1[1] = cutoff;
	    c1[2] = a * cos(s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kidney Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:38)
***************************************************************************/

int	DoKidneySurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[1] = cos(t) * (3 * cos(s) - cos(3 * s));
	    c1[2] = sin(t) * (3 * cos(s) - cos(3 * s));
	    c1[0] = 3 * sin(s) - sin(3 * s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kinky Torus
	Equations by Roger Bagula - Graphics by Paul Bourke - June 2003
	http://paulbourke.net/geometry/toroidal/
***************************************************************************/

int	DoKinkyTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sech(t) - cos(s);
	    c1[1] = sin(s);
	    c1[2] = t / PI - tanh(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Klein Bottle
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:8)

	A German topologist named Klein Thought the Möbius Loop was divine
	Said he, "If you glue the edges of two you get a weird bottle like mine."

	Most containers have an inside and an outside, a Klein bottle is a closed 
	surface with no interior and only one surface. It is unrealisable in 
	3 dimensions without intersecting surfaces. It can be realised in 4 dimensions. 
	The classical representation is shown below. 
	A Klein bottle can be formed by taking two Möbius strips and join them along 
	their boundaries, this so called "Figure-8 Klein Bottle" can be parameterised as follows:

	    x = cos(u) (a + sin(v) cos(u/2) - sin(2v) sin(u/2)/2)
	    y = sin(u) (a + sin(v) cos(u/2) - sin(2v) sin(u/2)/2)
	    z = sin(u/2) sin(v) + cos(u/2) sin(2v)/2
	where
	   -π <= u <= π and -π <= v <= π
***************************************************************************/

int	DoKleinBottle(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, start, end, angle, a, b, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    start = param[0];
    end = param[1];
    angle = param[2];
    a = param[3];
    b = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (end - start) / iterations;
    xgap = angle / iterations;
    t = 0.0;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	r = 4.0 * (1.0 - cos(t) / 2);
	s = start;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    if (t >= 0.0 && t < PI)
		{
		c1[0] = a * cos(t) * (1.0 + sin(t)) + r * cos(t) * cos(s);
		c1[1] = b * sin(t) + r * sin(t) * cos(s);
		c1[2] = r * sin(s);
		}
	    else
		{
		c1[0] = a * cos(t) * (1.0 + sin(t)) + r * cos(s + PI);
		c1[1] = b * sin(t);
		c1[2] = r * sin(s);
		}
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Klein Bottle (Nordstrand)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:210)
***************************************************************************/

int	DoKleinBottleNordstrand(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, Root2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];

    Root2 = sqrt(2.0);
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(s) * (cos(s / 2) * (Root2 + cos(t)) + sin(s / 2) * sin(t) * cos(t));
	    c1[2] = sin(s) * (cos(s / 2) * (Root2 + cos(t)) + sin(s / 2) * sin(t) * cos(t));
	    c1[1] = -sin(s / 2) * (Root2 + cos(t)) + cos(s / 2) * sin(t) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Klein Cycloid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:104)
***************************************************************************/

int	DoKleinCycloid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];

    xEnd = 2*b*c*PI;
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t / c) * cos(t / b) * (a + cos(s)) + sin(t / b) * sin(s) * cos(s);
	    c1[1] = sin(t / c) * cos(t / b) * (a + cos(s)) + sin(t / b) * sin(s) * cos(s);
	    c1[2] = -sin(t / b) * (a + cos(s)) + cos(t / b) * sin(s) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Klein Surface
	Nature Inspired Shapes and Design of Engineering Applications
	Asutosh P and Raj C Thiagarajan'
	https://www.comsol.com/paper/nature-inspired-surfaces-and-engineering-applications-using-comsol-multiphysics-41321
***************************************************************************/

int	DoKleinSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];

    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (3 * (1 + sin(t)) + 2 * (1 - cos(t / 2)) * cos(s)) * cos(t);
	    c1[1] = (4 + 2 * (1 - cos(t / 2)) * cos(s)) * sin(t);
	    c1[2] = -2 * (1 - cos(t / 2)) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Knitterkugel (Crease Ball)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:227)
***************************************************************************/

int	DoKnitterkugel(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];

    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = sin(s) * sin(t) + 0.05 * cos(20 * t);
	    c1[1] = cos(s) * sin(t) + 0.05 * cos(20 * s);
	    c1[2] = cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Knotted Surface
	https://www.desmos.com/3d/1a7f86ac77?lang=pt-BR
***************************************************************************/

int	DoKnottedSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, fn, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];

    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    fn = (sqrt(PI * s) + sin(PI * s)) / PI;
	    c1[0] = s;
	    c1[1] = fn * cos(t);
	    c1[2] = fn * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kreisel Surface II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:226)
***************************************************************************/

int	DoKreiselSurfaceII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    r = cos(2 * s);
	    c1[0] = r * sin(s) * cos(t);
	    c1[1] = r * sin(s) * sin(t);
	    c1[2] = r * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kuen´s Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:51)
***************************************************************************/

int	DoKuensSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    h = 1.0 + t * t * sin(s) * sin(s);
	    c1[1] = (2 * (cos(t) + t * sin(t)) * sin(s)) / h;
	    c1[2] = (2 * (-t * cos(t) + sin(t)) * sin(s)) / h;
	    c1[0] = log(tan(s / 2)) + 2 * cos(s) / h;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Kyle Havens Surface
	https://www.geogebra.org/m/zbCYeuGM
***************************************************************************/

int	DoKyleHavensSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (1 - s) * (3 + cos(t)) * cos(4 * PI * s);
	    c1[1] = (1 - s) * (3 + cos(t)) * sin(4 * PI * s);
	    c1[2] = 3 * s + (1 - s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lawson Bottle
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:213)
***************************************************************************/

int	DoLawsonBottle(void)

    {
    int		i, j;
    double	c1[4], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, sqrt2, sqrtHalf, w, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z
    c1[3] = param[13]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    sqrt2 = sqrt(2.0);
    sqrtHalf = sqrt(0.5);

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 4);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[3] = (sin(s) * sin(t) + sin(s / 2) * cos(t)) / sqrt2;
	    w = c1[3];
	    c1[0] = (sin(s) * sin(t) - sin(s / 2) * cos(t)) * sqrtHalf / (1 + w);
	    c1[1] = cos(s) * sin(t) / (1 + w);
	    c1[2] = cos(s / 2) * cos(t) / (1 + w);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 4) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lemniscape
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:74)
***************************************************************************/

int	DoLemniscape(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(s) * sqrt(fabs(sin(2*t))) * cos(t);
	    c1[1] = cos(s) * sqrt(fabs(sin(2*t))) * sin(t);
	    c1[2] = sqr(c1[0]) - sqr(c1[1]) + 2*c1[0]*c1[1] * sqr(tan(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lemniscate Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:146)
***************************************************************************/

int	DoLemniscateCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, r, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    r = param[4];
    H = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = r * cos(t) / (1 + sin(t) * sin(t));
	    c1[1] = H * s;
	    c1[2] = r * sin(t) * cos(t) / (1 + sin(t) * sin(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lemniscate Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:137)
***************************************************************************/

int	DoLemniscateTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * cos(s) / (1 + sin(s) * sin(s))) * cos(t);
	    c1[1] = r * sin(s) * cos(s) / (1 + sin(s) * sin(s));
	    c1[2] = (R + r * cos(s) / (1 + sin(s) * sin(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lemniscate Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:138)
***************************************************************************/

int	DoLemniscateTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * sin(s) * cos(s) / (1 + sin(s) * sin(s))) * cos(t);
	    c1[1] = r * cos(s) / (1 + sin(s) * sin(s));
	    c1[2] = (R + r * sin(s) * cos(s) / (1 + sin(s) * sin(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Lemon
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:128)
***************************************************************************/

int	DoLemon(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    b = sqrt(R*R-r*r);
    yStart = -b;
    yEnd = b;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    a = sqrt(R*R - t*t) - r;
	    c1[0] = a * sin(s);
	    c1[1] = t;
	    c1[2] = a * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Limpet Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:105)
***************************************************************************/

int	DoLimpetTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, Root2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    Root2 = sqrt(2.0);
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t) / (Root2 + sin(s));
	    c1[1] = 1/(Root2 + cos(s));
	    c1[2] = sin(t) / (Root2 + sin(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Logarithmic Spiral
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:184)
***************************************************************************/

int	DoLogarithmicSpiral(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    H = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = exp(a*t) * cos(t);
	    c1[1] = H * s;
	    c1[2] = exp(a*t) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Loop
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:62)
***************************************************************************/

int	DoLoop(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = sin(s) * cos(t);
	    c1[1] = 2 * cos(s);
	    c1[2] = 4 * sin(s) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Maeder´s Owl
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:31)
***************************************************************************/

int	DoMaedersOwl(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[1] = s * cos(t) - 0.5 * s*s * cos(2 * t);
	    c1[0] = -s * sin(t) - 0.5 * s*s * sin(2 * t);
	    c1[2] = 4 * exp(1.5 * log(s)) * cos(3 * t/2)/3;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Menn´s Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:67)
***************************************************************************/

int	DoMennsSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = s;
	    c1[2] = a * pow(t, 4) + t*t*s - s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Milk Carton
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:68)
***************************************************************************/

int	DoMilkCarton(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, k, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    k = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = k * (1 + t) * cos(s);
	    c1[1] = k * (1 - t) * sin(s);
	    c1[2] = t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Möbius Band
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:9)
***************************************************************************/

int	DoMöbiusBand(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, radius, width, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    yStart = param[0];
    yEnd = param[1];
    radius = param[2];
    width = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    xStart = -width;
    xEnd = width;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (radius + s * cos(t / 2)) * cos(t);
	    c1[1] = (radius + s * cos(t / 2)) * sin(t);
	    c1[2] = s * sin(t / 2);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Monkey Saddle
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:10)
***************************************************************************/

int	DoMonkeySaddle(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = t;
	    c1[1] = s;
	    c1[0] = t*t*t / 3 - 3 * t*s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Naviculoid Diatom
	JANICE L. PAPPAS 22 March 2005
	https://www.academia.edu/18157685/THEORETICAL_MORPHOSPACE_AND_ITS_RELATION_TO_FRESHWATER_GOMPHONEMOID_CYMBELLOID_DIATOM_BACILLARIOPHYTA_LINEAGES
***************************************************************************/

int	DoNaviculoidDiatom(void)

    {
    int		i, j;
    double	c1[3], s, t, st, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    st = sin(t / 2);
	    c1[1] = 3 * st;
	    c1[0] = sech(1.2 * t) * tanh(10 * s);
	    c1[2] = st;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Neovius Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:190)
***************************************************************************/

int	DoNeoviusSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, f, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = t;
	    f = (-3 * cos(s) - 3 * cos(t)) / (-3 + 4 * cos(s) * cos(t));
	    c1[2] = acos(f);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Nephroid Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:135)
***************************************************************************/

int	DoNephroidTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (3 * cos(s) - cos(3*s))) * cos(t);
	    c1[1] = r * (3 * sin(s) - sin(3*s));
	    c1[2] = (R + r * (3 * cos(s) - cos(3*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Nephroid Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:136)
***************************************************************************/

int	DoNephroidTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (3 * sin(s) - sin(3*s))) * cos(t);
	    c1[1] = r * (3 * cos(s) - cos(3*s));
	    c1[2] = (R + r * (3 * sin(s) - sin(3*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Normal Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:54)
***************************************************************************/

int	DoNormalTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R+r * cos(s)) * cos(t);
	    c1[1] = (R+r * cos(s)) * sin(t);
	    c1[2] = r * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Oloid (two discs which are inserted into each other at 90°)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:93)
***************************************************************************/

int	DoOloid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 0.0;
	    c1[1] = s * sin(t);
	    c1[2] = 1.0 + s * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    c1[0] = s * sin(t);
	    c1[1] = 0.0;
	    c1[2] = -s * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Paper Bag
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:99)
***************************************************************************/

int	DoPaperBag(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * cos(t);
	    c1[1] = (s + b*t) * sin(t);
	    c1[2] = a * s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Parabolic Humming Top
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:186)
***************************************************************************/

int	DoParabolicHummingTop(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, h, p, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    h = param[4];
    p = param[5];
    xStart = param[0];
    xEnd = param[1];
    yStart = -h;
    yEnd = h;
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sqr((fabs(t) - h)) * cos(s) / (2 * p);
	    c1[1] = sqr((fabs(t) - h)) * sin(s) / (2 * p);
	    c1[2] = t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Paraboloid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:21)
***************************************************************************/

int	DoParaboloid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a*s * cos(t);
	    c1[1] = a*s * sin(t);
	    c1[2] = b * s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pillow Shape
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:42)
***************************************************************************/

int	DoPillowShape(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t);
	    c1[1] = cos(s);
	    c1[2] = a * sin(s) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Piriform Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:174)
***************************************************************************/

int	DoPiriformSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    r = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = b * (cos(s) * (r + sin(s))) * cos(t);
	    c1[1] = a * (r + sin(s));
	    c1[2] = b * (cos(s) * (r + sin(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Piriform Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:154)
***************************************************************************/

int	DoPiriformTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + (r * sin(s))) * cos(t);
	    c1[1] = cos(s) * (r + sin(s));
	    c1[2] = (R + (r * sin(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Piriform Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:155)
***************************************************************************/

int	DoPiriformTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + cos(s) * (r + sin(s))) * cos(t);
	    c1[1] = r + sin(s);
	    c1[2] = (R + cos(s) * (r + sin(s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pisot Triaxial
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:108)
***************************************************************************/

int	DoPisotTriaxial(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 0.655866 * cos(1.03002 + t) * (2 + cos(s));
	    c1[1] = 0.74878 * cos(1.40772 - t) * (2 + 0.868837 * cos(2.43773 + s));
	    c1[2] = 0.868837 * cos(2.43773 + t) * (2 + 0.495098 * cos(0.377696 - s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Plücker´s Conoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:11)
***************************************************************************/

int	DoPlückersConoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t * sqrt(1 - s * s);
	    c1[1] = t * s;
	    c1[2] = 1 - s * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Product Surface
	https://d1wqtxts1xzle7.cloudfront.net/90800274/s0734-189x_2884_2980010-920220908-1-mk1zw4-libre.pdf
***************************************************************************/

int	DoProductSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (3 * t * t + 4) * s;
	    c1[1] = t;
	    c1[2] = (3 * t * t + 5) * pow(s, 3) + t * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pseudodevelopable Helicoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:202)
***************************************************************************/

int	DoPseudodevelopableHelicoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) - s * sin(t);
	    c1[2] = a * sin(t) - s * cos(t);
	    c1[1] = -b * t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pseudo Cross Cap
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:47)
***************************************************************************/

int	DoPseudoCrossCap(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (1 - t*t) * sin(s);
	    c1[1] = (1 - t*t) * sin(2*s);
	    c1[2] = t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pseudocatenoid
	By Roger Bagula Graphics by Paul Bourke January 2003 
	http://paulbourke.net/geometry/pseudocatenoid/
***************************************************************************/

int	DoPseudocatenoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, PlaneLimit, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    PlaneLimit = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (2 + cos(s));
	    c1[1] = sin(t) * (2 + cos(s));
	    c1[2] = c1[0] * c1[0] - c1[1] * c1[1] + 2 * c1[0] * c1[1] * sqr(tan(s));
								// surface is infinite, so put boundaries on it
	    if (c1[0] < -PlaneLimit * ScreenRatio) c1[0] = -PlaneLimit * ScreenRatio; 
	    if (c1[0] > PlaneLimit * ScreenRatio) c1[0] = PlaneLimit * ScreenRatio; 
	    if (c1[1] < -PlaneLimit) c1[1] = -PlaneLimit; 
	    if (c1[1] > PlaneLimit) c1[1] = PlaneLimit; 
	    if (c1[2] < -PlaneLimit * ScreenRatio) c1[2] = -PlaneLimit; 
	    if (c1[2] > PlaneLimit * ScreenRatio) c1[2] = PlaneLimit; 
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Pseudosphere
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:27)
***************************************************************************/

int	DoPseudosphere(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t) * sin(s);
	    c1[1] = sin(t) * sin(s);
	    c1[2] = cos(s) + log(tan(s/2));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Rastrigin Surface
    https://demonstrations.wolfram.com/GlobalMinimumOfASurface/ + ChatGPT
***************************************************************************/

int	DoRastriginSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = t;
	    c1[2] = (20 + s*s + t*t) - 10 * (cos(2 * PI * s) + cos(2 * PI * t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Ribbon
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:62)
***************************************************************************/

int	DoRibbon(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = sin(t) * cos(s);
	    c1[1] = 2.0 * cos(t);
	    c1[2] = 4.0 * sin(t) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Richmond Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:32)
***************************************************************************/

int	DoRichmondSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, PlaneLimit;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    PlaneLimit = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (-3 * t - pow(t, 5) + 2 * pow(t, 3) * s*s + 3*t*pow(s,4))/(6 * (t*t + s*s));
	    c1[1] = (-3 * s - 3 * pow(t, 4) * s - 2 * t*t * pow(s,3) + pow(s, 5))/(6 * (t*t + s*s));
	    c1[2] = t;
								// surface is infinite, so put boundaries on it
	    if (c1[0] < -PlaneLimit * ScreenRatio) c1[0] = -PlaneLimit * ScreenRatio; 
	    if (c1[0] > PlaneLimit * ScreenRatio) c1[0] = PlaneLimit * ScreenRatio; 
	    if (c1[1] < -PlaneLimit) c1[1] = -PlaneLimit; 
	    if (c1[1] > PlaneLimit) c1[1] = PlaneLimit; 
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Richmond Surface III
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:243)
***************************************************************************/

int	DoRichmondSurfaceIII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, n;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    n = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = -cos(t) / (2 * s) - (pow(s, (2 * n + 1)) * cos(-(2 * n + 1) * t)) / (4 * n + 2);
	    c1[1] = -sin(t) / (2 * s) + (pow(s, (2 * n + 1)) * sin(-(2 * n + 1) * t)) / (4 * n + 2);
	    c1[2] = pow(s, n * cos(n * t)) / n;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Roman Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:12)
***************************************************************************/

int	DoRomanSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = 2*t * cos(s) * sqrt(1 - t*t);
	    c1[1] = 2*t * sin(s) * sqrt(1 - t*t);
	    c1[0] = 1 - 2*t*t * cos(s) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Rosenbrock Surface
    https://demonstrations.wolfram.com/GlobalMinimumOfASurface/ + ChatGPT
***************************************************************************/

int	DoRosenbrockSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = t;
	    c1[2] = 100 * sqr(t - s*s) + sqr(1 + s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Ruled Rotor Cylindroid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:187)
***************************************************************************/

int	DoRuledRotorCylindroid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    n = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[2] = (a + b * sin(n * s)) * cos(s) - t * sin(s);
	    c1[1] = (a + b * sin(n * s)) * sin(s) + t * cos(s);
	    c1[0] = b * cos(n * s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Rounded Cube
	Paul Bourke 2004
	http://paulbourke.net/geometry/cube/
***************************************************************************/

int	DoRoundedCube(void)
    {
    int	    i,	j,  index;
    double  theta,  phi;
    int	    N = 12;			// Mesh resolution
    double  rpower = 0.3;		// Power that controls corner strength
    double  c1[3], cold[3], ColourFactor;

    N = (int)pow(2.0, (int)param[0]);
    rpower = param[1];
    ColourFactor = param[2];
    iterations = N;
    totpasses = 10;

	// Pole is along the z axis
    InitSurface(c1, 3);							// pass in number of dimensions
    for (j=0;j<=N/2;j++) 
	{
	curpass = (int)(j * totpasses / iterations);
	for (i=0;i<=N;i++) 
	    {
	    index = j * (N+1) + i;
	    theta = i * 2 * PI / N;
	    phi = -0.5*PI + PI * j / (N/2.0);
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;

			// Unit sphere, power determines roundness
	    c1[0] = SignedPower(cos(phi),rpower) * SignedPower(cos(theta),rpower);
	    c1[1] = SignedPower(cos(phi),rpower) * SignedPower(sin(theta),rpower);
	    c1[2] = SignedPower(sin(phi),rpower);

         // Texture coordinates
         // Project texture coordinates from a surrounding sphere
//	    p[index].u = atan2(p[index].y,p[index].x) / (2*PI);
//	    if (p[index].u < 0)
//		p[index].u = 1 + p[index].u;
//	    p[index].v = 0.5 + atan2(p[index].z,sqrt(p[index].x*p[index].x+p[index].y*p[index].y)) / PI;

			// Seams
	    if (j == 0) 
		{
		c1[0] = 0;
		c1[1] = 0;
		c1[2] = -1;
		}
	    if (j == N/2) 
		{
		c1[0] = 0;
		c1[1] = 0;
		c1[2] = 1;
		}
	    if (i == N) 
		{
		c1[0] = cold[0];
		c1[1] = cold[1];
		}
	    cold[0] = c1[0];
	    cold[1] = c1[1];

	    if (DisplaySurface(c1, ((DWORD)(j / ColourFactor) % threshold), j, 3) < 0)
		return -1;
	    }
	}

    PlotExtras();
    CloseZvalues();
    return(0);
    }

/**************************************************************************
	Saddle Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:69)
***************************************************************************/

#define	F(x)	(1-sqr(cos(x)) - sqr(cos(x + 2*PI/3)))
#define	sign(x)	((x)>0)?1:((x)<0)?-1:0

double fcn(double u)
    {
    double a;

    a = 1 - cos(u)*cos(u) - cos(u+2*PI/3)*cos(u+2*PI/3);
    if (a < 0)
	return(-sqrt(-a));
    else
	return(sqrt(a));
}


int	DoSaddleTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (2 + cos(t)) * cos(s);
	    c1[1] = (2 + cos(t + 2 * PI/3)) * cos(s + 2 * PI/3);
	    c1[2] = (2 + fcn(t)) * fcn(s);
//	    c1[2] = (2 + sign(F(t)) * sqrt(fabs(F(t)))) * sign(F(s)) * sqrt(fabs(F(s)));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Schnecke (Snail)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:13)
***************************************************************************/

int	DoSchnecke(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3] * 2.0 * PI;					// number of twists
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    h = exp(t / (6*PI));
	    c1[0] = a * (1 - h) * cos(t) * cos(0.5 * s) * cos(0.5 * s);
	    c1[1] = 1 - exp(t/(b * PI)) - sin(s) + h * sin(s);
	    c1[2] = a * (-1 + h) * sin(t) * cos(0.5 * s) * cos(0.5 * s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Schnecke II (Snail)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:288)
***************************************************************************/

int	DoSchneckeII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];					// number of twists
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    h = pow(a, t);
	    c1[0] = h * sqr(sin(s)) * sin(t);
	    c1[1] = h * sqr(sin(s)) * cos(t);
	    c1[2] = h * sin(s) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Seashell
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:14)
***************************************************************************/

int	DoSeashell(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, h, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    h = 1 - s/2;
	    c1[0] = a * h * cos(n*s*PI) * (1 + cos(t*PI)) + c * cos(n*s*PI);
	    c1[2] = a * h * sin(n*s*PI) * (1 + cos(t*PI)) + c * sin(n*s*PI);
	    c1[1] = b * s/2 + a * h * sin(t*PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Scherk Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:33)
***************************************************************************/

int	DoScherkSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = s;
	    c1[2] = log(fabs(cos(c*t)/cos(c*s)))/c;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Scherk Surface II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:222)
***************************************************************************/

int	DoScherkSurfaceII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = log(1 + s * s + 2 * s * cos(t) / (1 + s * s - 2 * s * cos(t)));
	    c1[1] = log(1 + s * s - 2 * s * sin(t) / (1 + s * s + 2 * s * sin(t)));
	    c1[2] = 2 * atan(2 * s * s * sin(2 * t) / (pow(s, 4) - 1));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Shoe Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:15)
***************************************************************************/

int	DoShoeSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[2] = s;
	    c1[1] = t*t*t / 3 - s*s / 2;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sievert Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:178)
***************************************************************************/

int	DoSievertSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, C, p, a, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    C = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    p = -t/sqrt(C + 1) + atan(tan(t) * sqrt(C + 1));
	    a = 2/(C + 1 - C * sin(s) * sin(s) * cos(t) * cos(t));
	    r = a * sqrt((C + 1) * (1 + C * sin(t) * sin(t))) * sin(s)/sqrt(C);

	    c1[0] = r * cos(p);
	    c1[2] = r * sin(p);
	    c1[1] = (log(tan(s/2)) + a * (C + 1) * cos(s))/sqrt(C);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sine Cone
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:129)
***************************************************************************/

int	DoSineCone(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, n, k, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    n = param[4];
    k = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = t * cos(s);
	    c1[1] = k * t * cos(n*s);
	    c1[2] = t * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sine Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:16)
***************************************************************************/

int	DoSineSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t);
	    c1[2] = sin(s);
	    c1[1] = sin(s+t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sine Waves
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:120)
***************************************************************************/

int	DoSineWaves(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[2] = a * sin(b * sqrt(t*t + s * s));
	    c1[1] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sinewurfel (Sine Cube)
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:229)
***************************************************************************/

int	DoSinewurfel(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(s) * sin(t);
	    c1[2] = cos(s) * sin(t);
	    c1[1] = cos(s) * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
    Singularity Surface
    https://www-sop.inria.fr/galaad/surface/classification/ + ChatGPT to parametrize intrinsic equation
***************************************************************************/

int	DoSingularitySurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;
    int		type;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    type = (int)param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = s;
	    c1[1] = t;
	    switch (type)
		{
		case 0:
		    c1[2] = sqrt(s*s - t*t);				// A1
		    break;
		case 1:
		    c1[2] = sqrt(s*s*s - t*t);				// A2--
		    break;
		case 2:	    
		    c1[2] = sqrt(s*s*s + t*t);				// A2 +-
		    break;
		case 3:
		    c1[2] = sqrt(s*s*s*s - t*t);			// A3--
		    break;
		case 4:
		    c1[2] = sqrt(s*s*s*s + t*t);			// A3+-
		    break;
		case 5:
		    c1[2] = sqrt(s*s*s*s*s + t*t);			// A4
		    break;
		default:
		    c1[2] = sqrt(s*s - t * t);				// A1
		    break;
		}
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Skidan Ruled Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:220)
***************************************************************************/

int	DoSkidanRuledSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, h, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    h = param[5];
    n = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    b = h * cos(n * t);
	    c1[0] = (s * sin(a) + b * cos(a)) * cos(t);
	    c1[2] = (s * sin(a) + b * cos(a)) * sin(t);
	    c1[1] = s * cos(a) - b * sin(a);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Slippers Surface
	By Roger Bagula Graphics by Paul Bourke August 2002 
	http://paulbourke.net/geometry/slipper/
***************************************************************************/

int	DoSlippersSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (2 + cos(t)) * pow(cos(s), 3) * sin(s);
	    c1[2] = (2 + cos(t+2*PI/3)) * pow(cos(2*PI/3+s), 2) * pow(sin(2*PI/3+s), 2);
	    c1[1] = -(2 + cos(t-2*PI/3)) * pow(cos(2*PI/3-s), 2) * pow(sin(2*PI/3-s), 3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Snails and Shells
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:85-91)
***************************************************************************/

int	DoSnailsnShells(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, h, k, w, R, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    R = param[13];
    ColourFactor = param[14];

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    h = param[7];
    k = param[8];
    w = param[9];
    totpasses = 10;

    R = (R >+ 0.0) ? 1.0 : -1.0;					// direction of rotation

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = exp(w*t) * (h + a * cos(s)) * cos(c*t);
	    c1[2] = R*exp(w*t) * (h + a * cos(s)) * sin(c*t);
	    c1[1] = exp(w*t) * (k + b * sin(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Snail Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:34)
***************************************************************************/

int	DoSnailSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t * cos(s) * sin(t);
	    c1[2] = t * cos(s) * cos(t);
	    c1[1] = -t * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Soucoupoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:126)
***************************************************************************/

int	DoSoucoupoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * cos(s);
	    c1[2] = pow(sin(t), 3);
	    c1[1] = cos(t) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sphere I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:113)
***************************************************************************/

int	DoSphereI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t) * cos(s);
	    c1[2] = cos(t);
	    c1[1] = sin(t) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sphere II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:114)
***************************************************************************/

int	DoSphereII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * cos(s);
	    c1[2] = sin(s);
	    c1[1] = sin(t) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sphere III
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:115)
***************************************************************************/

int	DoSphereIII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 2*t / (1 + t*t + s*s);
	    c1[2] = (t*t + s*s - 1) / (1 + t*t + s*s);
	    c1[1] = 2*s / (1 + t*t + s*s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Sphere IV
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:116)
***************************************************************************/

int	DoSphereIV(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 2*t * (1 - s*s) / ((1 + t*t) * (1 + s*s));
	    c1[2] = (1 - t*t) / (1 + t*t);
	    c1[1] = (4*s*t) / ((1 + t*t) * (1 + s*s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spherical Frustum
	https://www.wolframalpha.com/input?i=Spherical+Frustum+
***************************************************************************/

int	DoSphericalFrustum(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, d, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    d = param[2];
    h = param[3];
    a = param[4];
    if (a == 0.0)
	a = 1;

    ColourFactor = param[5];
    totpasses = 10;

    yStart = acos((d+h)/a);
    yEnd = acos(d / a);
    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(s) * sin(t);
	    c1[2] = a * sin(s) * sin(t);
	    c1[1] = a * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spherical Harmonics
	Written by Paul Bourke February 1990
	http://paulbourke.net/geometry/sphericalh/
***************************************************************************/

int	DoSphericalHarmonics(void)

    {
    int		i, j;
    double	c1[3], r, s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, m[8];

    for (i = 0; i < 8; i++)
	m[i] = param[i+10];

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    r  = pow(sin(m[0]*t), m[1]);
	    r += pow(cos(m[2]*t), m[3]);
	    r += pow(sin(m[4]*s), m[5]);
	    r += pow(cos(m[6]*s), m[7]);

	    c1[0] = r * sin(t) * cos(s);
	    c1[1] = r * cos(t);
	    c1[2] = r * sin(t) * sin(s);	    
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spherical Rose
	https://www.desmos.com/3d/9f12243a20?lang=pt-BR
***************************************************************************/

int	DoSphericalRose(void)

    {
    int		i, j;
    double	c1[3], k, k1, k2, s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, m[8];

    for (i = 0; i < 8; i++)
	m[i] = param[i + 10];

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    k1 = cos(10 * PI * t + 20 * PI * s);
	    k2 = cos(10 * PI * t - 20 * PI * s);
	    k = (k1 >= k2) ? k1 : k2;
	    c1[0] = (0.25 + sin(PI * s) * (k - 0.5) / 20) * (-sin(PI * s)) * (-sin(2 * PI * t));
	    c1[1] = (0.25 + sin(PI * s) * (k - 0.5) / 20) * (-sin(PI * s)) * ( cos(2 * PI * t));
	    c1[2] = (0.25 + sin(PI * s) * (k - 0.5) / 20) * ( cos(PI * s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spindel Surface 
	https://www.wolframalpha.com/input?i=Spindel+surface
***************************************************************************/

int	DoSpindelSurface(void)

    {
    int		i, j;
    double	c1[3], a, c, s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor, m[8];

    for (i = 0; i < 8; i++)
	m[i] = param[i + 10];

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    c = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) * sqrt(s * s + 1);
	    c1[1] = a * sin(t) * sqrt(s * s + 1);
	    c1[2] = c * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spiral Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:167)
***************************************************************************/

int	DoSpiralTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, r, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    r = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + R2 * cos(n*t) + r * cos(s)) * cos(t);
	    c1[1] = r * sin(s) + R2 * sin(n*t);
	    c1[2] = (R1 + R2 * cos(n*t) + r * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spiral Waves
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:122)
***************************************************************************/

int	DoSpiralWaves(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * cos(t);
	    c1[2] = a * cos(b*t + c*s);
	    c1[1] = s * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Spring
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:78)
***************************************************************************/

int	DoSpring(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, L, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    L = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd * n - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = (R + r * cos(s)) * cos(t);
	    c1[1] = (R + r * cos(s)) * sin(t);
	    c1[2] = r * (sin(s) + L*t/PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Steinbach Screw
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:17)
***************************************************************************/

int	DoSteinbachScrew(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, start, end, angle, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    start = param[0];
    end = param[1];
    angle = param[2];
    ColourFactor = param[3];
    totpasses = 10;

    xgap = (end - start) / iterations;
    ygap = angle / iterations;
    t = start;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = 0.0;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t * cos(s);
	    c1[1] = t * sin(s);
	    c1[2] = s * cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Stiletto Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:49)
***************************************************************************/

int	DoStilettoSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    double temp = sqr(cos(s+2*PI/3)) * sqr(sin(s+2*PI/3));
	    c1[0] = (2+cos(t)) * pow(cos(s),3) * sin(s);
	    c1[1] = (2+cos(t+2*PI/3)) * temp;
	    c1[2] = -(2+cos(t-2*PI/3)) * temp;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Strangled Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:130-131)
***************************************************************************/

int	DoStrangledTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    n = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * sin(s)) * cos(t);
	    c1[1] = r * cos(s) * cos(n*t/2);
	    c1[2] = (R + r * sin(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Strophoid Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:147)
***************************************************************************/

int	DoStrophoidCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + a * (s*s - 1)/(s*s + 1)) * cos(t);
	    c1[1] = a*s * (s*s - 1)/(s*s + 1);
	    c1[2] = (R + a * (s*s - 1)/(s*s + 1)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Super Shape
	Paul Bourke 2002
	http://paulbourke.net/geometry/supershape/
***************************************************************************/

static	double	EvalSS(double m, double n1, double n2, double n3, double phi)
    {
    double  r;
    double  t1, t2;
    double  a = 1, b = 1;

    t1 = cos(m * phi / 4) / a;
    t1 = fabs(t1);
    t1 = pow(t1, n2);

    t2 = sin(m * phi / 4) / b;
    t2 = fabs(t2);
    t2 = pow(t2, n3);

    r = pow(t1+t2,1/n1);
    return r;
    }

int	DoSuperShape(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, m, n1, n2, n3, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    m = param[4];
    n1 = param[5];
    n2 = param[6];
    n3 = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = EvalSS(m, n1, n2, n3, t) * cos(t) * EvalSS(m, n1, n2, n3, s) * cos(s);
	    c1[1] = EvalSS(m, n1, n2, n3, t) * sin(t) * EvalSS(m, n1, n2, n3, s) * cos(s);
	    c1[2] = EvalSS(m, n1, n2, n3, s) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Super Torus
	Paul Bourke 1990
	http://paulbourke.net/geometry/toroidal/
***************************************************************************/

int	DoSuperTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, n1, n2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    n1 = param[6];
    n2 = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = SignedPower(cos(t), n1) * (R + r * SignedPower(cos(s), n2));
	    c1[1] = SignedPower(sin(t), n1) * (R + r * SignedPower(cos(s), n2));
	    c1[2] = r * SignedPower(sin(s), n1);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Swallow Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:29)
***************************************************************************/

int	DoSwallowSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = t*s*s + 3 * pow(s,4);
	    c1[0] = -2*t*s - 4*s*s*s;
	    c1[2] = t;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tanh Spiral
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:183)
***************************************************************************/

int	DoTanhSpiral(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, H, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    H = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sinh(2*t) / (cos(2*a*t) + cosh(2*t));
	    c1[1] = H * s;
	    c1[2] = sin(2*a*t) / (cos(2*a*t) + cosh(2*t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tetrahedral Ellipse
	Graphics by Paul Bourke October 2004 Attributed to Roger Bagula 
	http://paulbourke.net/geometry/tetraellipse/
***************************************************************************/

int	DoTetrahedralEllipse(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (1 - s) * cos(t);
	    c1[1] = (1 + s) * sin(t);
	    c1[2] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tooth Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:206)
***************************************************************************/

int	DoToothSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, d, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    d = (pow(sin(s), 4) + pow(cos(s), 4)) * pow(cos(t), 4) + pow(sin(t), 4);
	    r = -sqrt(1 / d);
	    c1[0] = r * cos(t) * cos(s);
	    c1[2] = r * cos(t) * sin(s);
	    c1[1] = -r * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tortuous Evolved Toroid 
	https://www.desmos.com/3d/a55d6ba0a3?lang=pt-BR
***************************************************************************/

int	DoTortuousEvolvedToroid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (((((cos(-10 * PI * s) + 4) * (cos(2 * PI * s)) + (sin(-10 * PI * s)) * (-sin(2 * PI * s))) * (1 + cos(20 * PI * t) / 3))
		    * (cos(40 * PI * t)) + (((cos(-10 * PI * s) + 4) * (sin(2 * PI * s)) + (sin(-10 * PI * s)) * (cos(2 * PI * s))) *
		    (1 + cos(20 * PI * t) / 3)) * (-sin(40 * PI * t)) + 9) * (cos(20 * PI * t)) + 25) * (cos(2 * PI * t)) + ((((cos(-10 * PI * s) + 4)
		    * (cos(2 * PI * s)) + (sin(-10 * PI * s)) * (-sin(2 * PI * s))) * (1 + cos(20 * PI * t) / 3)) * (sin(40 * PI * t)) +
		    (((cos(-10 * PI * s) + 4) * (sin(2 * PI * s)) + (sin(-10 * PI * s)) * (cos(2 * PI * s))) * (1 + cos(20 * PI * t) / 3)) *
		    (cos(40 * PI * t))) * (-sin(2 * PI * t));
	    c1[2] = (((((cos(-10 * PI * s) + 4) * (cos(2 * PI * s)) + (sin(-10 * PI * s)) * (-sin(2 * PI * s))) * (1 + cos(20 * PI * t) / 3))
		    * (cos(40 * PI * t)) + (((cos(-10 * PI * s) + 4) * (sin(2 * PI * s)) + (sin(-10 * PI * s)) * (cos(2 * PI * s))) *
		    (1 + cos(20 * PI * t) / 3)) * (-sin(40 * PI * t)) + 9) * (cos(20 * PI * t)) + 25) * (sin(2 * PI * t)) + ((((cos(-10 * PI * s) + 4)
		    * (cos(2 * PI * s)) + (sin(-10 * PI * s)) * (-sin(2 * PI * s))) * (1 + cos(20 * PI * t) / 3)) * (sin(40 * PI * t)) +
		    (((cos(-10 * PI * s) + 4) * (sin(2 * PI * s)) + (sin(-10 * PI * s)) * (cos(2 * PI * s))) * (1 + cos(20 * PI * t) / 3)) *
		    (cos(40 * PI * t))) * (cos(2 * PI * t));
	    c1[1] = ((((cos(-10 * PI * s) + 4) * (cos(2 * PI * s)) + (sin(-10 * PI * s)) * (-sin(2 * PI * s))) * (1 + cos(20 * PI * t) / 3))
		    * (cos(40 * PI * t)) + (((cos(-10 * PI * s) + 4) * (sin(2 * PI * s)) + (sin(-10 * PI * s)) * (cos(2 * PI * s))) *
		    (1 + cos(20 * PI * t) / 3)) * (-sin(40 * PI * t)) + 9) * (sin(20 * PI * t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Torus Nodes
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:168)
***************************************************************************/

int	DoTorusNodes(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, r, p, q, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    r = param[6];
    p = param[7];
    q = param[8];
    ColourFactor = param[9];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + R2 * cos(p*t) + r * cos(s)) * cos(q*t);
	    c1[1] = r * sin(s) + R2 * sin(p*t);
	    c1[2] = (R1 + R2 * cos(p*t) + r * cos(s)) * sin(q*t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tractrix Based Minimal Surface
	By Roger Bagula Graphics by Paul Bourke January 2003 
	http://paulbourke.net/geometry/tractrix/
***************************************************************************/

int	DoTractrixBasedMinimalSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (s - tanh(s));
	    c1[1] = cos(t) / cosh(s);
	    c1[2] = c1[0] * c1[0] - c1[1] * c1[1] + 2 * c1[0] * c1[1] * sqr(tanh(s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tractroid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:24)
***************************************************************************/

int	DoTractroid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sech(t) * cos(s);
	    c1[1] = sech(t) * sin(s);
	    c1[2] = t - tanh(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tranguloid Trefoil
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:53)
***************************************************************************/

int	DoTranguloidTrefoil(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 2 * sin(3 * t)/(2 + cos(s));
	    c1[1] = 2 * (sin(t) + 2 * sin(2 * t))/(2 + cos(s + 2 * PI/3));
	    c1[2] = (cos(t) - 2 * cos(2 * t)) * (2 + cos(s)) * (2 + cos(s + 2 * PI/3))/4;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Trash Can
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:77)
***************************************************************************/

int	DoTrashCan(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (b + s) * cos(t);
	    c1[1] = s * sin(t);
	    c1[2] = a * s*s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Trefoil Knot I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:52)
***************************************************************************/

int	DoTrefoilKnotI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, Bx, By, Bz, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    Bx = (R + r * cos(t / 2)) * cos(t / 3);
	    By = (R + r * cos(t / 2)) * sin(t / 3);
	    Bz = r + sin(t / 2);
	    c1[0] = Bx + r * cos(t/3) * cos(s-PI);
	    c1[1] = By + r * sin(t/3) * cos(s-PI);
	    c1[2] = Bz + r * sin(s-PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Trefoil Knot II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:152)
***************************************************************************/

int	DoTrefoilKnotII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, a, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    a = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = r * cos(s) * cos(t) + R * cos(t) * (1 + a * cos(n*t));
	    c1[1] = r * sin(s) + a * sin(n*t);
	    c1[2] = r * cos(s) * sin(t) + R * sin(t) * (1 + a * cos(n*t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triaxial Hexatorus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:109)
***************************************************************************/

int	DoTriaxialHexatorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t)/(sqrt(2.0) + cos(s));
	    c1[1] = sin(t + 2*PI/3)/(sqrt(2.0) + cos(s + 2*PI/3));
	    c1[2] = cos(t - 2*PI/3)/(sqrt(2.0) + cos(s - 2*PI/3));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triaxial Teardrop
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:41)
***************************************************************************/

int	DoTriaxialTeardrop(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (1 - cos(t)) * cos(t + 2 * PI/3) * cos(s + 2 * PI/3)/2;
	    c1[1] = (1 - cos(t)) * cos(t + 2 * PI/3) * cos(s - 2 * PI/3)/2;
	    c1[2] = cos(t - 2 * PI/3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triaxial Tritorus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:70)
***************************************************************************/

int	DoTriaxialTritorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t) * (1 + cos(s));
	    c1[1] = sin(t + 2*PI/3) * (1 + cos(s + 2*PI/3));
	    c1[2] = sin(t + 4*PI/3) * (1 + cos(s + 4*PI/3));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tricuspoid Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:133)
***************************************************************************/

int	DoTricuspoidTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (2 * cos(s) + cos(2*s))) * cos(t);
	    c1[1] = r * (2 * sin(s) - sin(2*s));
	    c1[2] = (R + r * (2 * cos(s) + cos(2*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Tricuspoid Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:134)
***************************************************************************/

int	DoTricuspoidTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + r * (2 * sin(s) - sin(2*s))) * cos(t);
	    c1[1] = r * (2 * cos(s) + cos(2*s));
	    c1[2] = (R + r * (2 * sin(s) - sin(2*s))) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triple Corkscrew I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:170)
***************************************************************************/

int	DoTripleCorkscrewI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, L, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    L = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = L * t;
	    c1[1] = r * (1 - fabs(t)) * cos(s) + R * (1 - fabs(t)) * cos(t*n*PI);
	    c1[2] = r * (1 - fabs(t)) * sin(s) + R * (1 - fabs(t)) * sin(t*n*PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triple Corkscrew II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:171)
***************************************************************************/

int	DoTripleCorkscrewII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, L, n, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    L = param[6];
    n = param[7];
    a = param[8];
    b = param[9];
    ColourFactor = param[13];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = L * t;
	    c1[1] = r * exp(-(b*b*t*t)) * cos(s) + R * exp(-(a*a*t*t)) * cos(t*n*PI);
	    c1[2] = r * exp(-(b*b*t*t)) * sin(s) + R * exp(-(a*a*t*t)) * sin(t*n*PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triple Corkscrew III
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:172)
***************************************************************************/

int	DoTripleCorkscrewIII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, L, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    L = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = L * t;
	    c1[1] = r * (1 - fabs(t)) * cos(s) + R * cos(t*PI/2) * cos(t*n*PI);
	    c1[2] = r * (1 - fabs(t)) * sin(s) + R * cos(t*PI/2) * sin(t*n*PI);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Triple Point Twist
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:75)
***************************************************************************/

int	DoTriplePointTwist(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    c = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = pow(s, 3) + c * s;
	    c1[2] = t * s + pow(s, 5) + c * pow(s, 3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Eight Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:56)
***************************************************************************/

int	DoTwistedEightTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R+r * (cos(t/2) * sin(s)-sin(t/2) * sin(2 * s))) * cos(t);
	    c1[1] = (R+r * (cos(t/2) * sin(s)-sin(t/2) * sin(2 * s))) * sin(t);
	    c1[2] = r * (sin(t/2) * sin(s) + cos(t/2) * sin(2 * s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Heart
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:110)
***************************************************************************/

int	DoTwistedHeart(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, InverseRoot2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InverseRoot2 = 1/sqrt(2.0);
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (fabs(s) - fabs(t) - fabs(tanh(InverseRoot2 * t) / InverseRoot2) + fabs(tanh(InverseRoot2 * s) / InverseRoot2)) * sin(s);
	    c1[1] = (fabs(s) + fabs(t) - fabs(tanh(InverseRoot2 * t) / InverseRoot2) - fabs(tanh(InverseRoot2 * s) / InverseRoot2)) * cos(s);
	    c1[2] = InverseRoot2 * (t*t + s*s) / (cosh(InverseRoot2 * t) * cosh(InverseRoot2 * s));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Pipe Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:61)
***************************************************************************/

int	DoTwistedPipeSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    h = sqrt(1 + sin(s) * sin(s));
	    c1[0] = cos(s) * (2 + cos(t))/h;
	    c1[1] = sin(s - 2*PI/3) * (2 + cos(t - 2*PI/3))/h;
	    c1[2] = sin(s + 2*PI/3) * (2 + cos(t + 2*PI/3))/h;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Plane Surface
	By Roger Bagula Rendering by Paul Bourke June 1999 
	http://paulbourke.net/geometry/twistplane/
***************************************************************************/

int	DoTwistedPlaneSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, h, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    h = sqrt(1 + sin(s) * sin(s));
	    c1[2] = sqrt(t*t + s*s) / sqrt(2.0);
	    c1[0] = (1-c1[2])*t + c1[2]*(1-t);
	    c1[1] = (1-c1[2])*s + c1[2]*(1-s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Ribbon Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:236)
***************************************************************************/

int	DoTwistedRibbonSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = 3 * sin(s) * cos(t);
	    c1[1] = 3 * sin(s) * sin(t);
	    c1[2] = cos(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Sphere
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:111)
***************************************************************************/

int	DoTwistedSphere(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = a * cos(t) * cos(s);
	    c1[1] = a * sin(s) + b * t;
	    c1[2] = a * sin(t) * cos(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Twisted Triaxial
	Supplied by Roger Bagula © Graphics by Paul Bourke August 1999
	http://paulbourke.net/geometry/triaxial/
***************************************************************************/

int	DoTwistedTriaxial(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, pp, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    pp = sqrt(t*t + s*s) / sqrt(2*PI*PI);
	    c1[0] = (1-pp)*cos(t)*cos(s)+pp*sin(t)*sin(s);
	    c1[1] = (1-pp)*cos(t+2*PI/3)*cos(s+2*PI/3)+pp*sin(t+2*PI/3)*sin(s+2*PI/3);
	    c1[2] = (1-pp)*cos(t+4*PI/3)*cos(s+4*PI/3)+pp*sin(t+4*PI/3)*sin(s+4*PI/3);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Two Torus
	By Roger Bagula - Graphics by Paul Bourke - July 2007
	http://paulbourke.net/geometry/toroidal/
***************************************************************************/

int	DoTwoTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ForthRootOf2, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ForthRootOf2 = pow(2.0, 0.25);

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t) * (ForthRootOf2 + cos(s));
	    c1[1] = sin(t) * (ForthRootOf2 + sin(s));
	    c1[2] = sqrt(sqr(t/PI) + sqr(s/PI));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Umbilical Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:57)
***************************************************************************/

int	DoUmbilicalTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, /*R, r, */ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = sin(t) * (7 + cos(t / 3 - 2*s) + 2 * cos(t / 3 + s));
	    c1[1] = cos(t) * (7 + cos(t / 3 - 2*s) + 2 * cos(t / 3 + s));
	    c1[2] = sin(t / 3 - 2*s) + 2 * sin(t / 3 + s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Umbrella Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:208)
***************************************************************************/

int	DoUmbrellaSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, h, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    h = param[5];
    n = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    r = R / n;
    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = pow(s, 0.33333333333) * ((R - r) * cos(t) + r * cos((n - 1) * t));
	    c1[2] = pow(s, 0.33333333333) * ((R - r) * sin(t) - r * sin((n - 1) * t));
	    c1[1] = h * (1 - s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Vase and Spear Head
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:100)
***************************************************************************/

int	DoVaseAndSpearHead(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = s * cos(b * s + t);
	    c1[1] = s * sin(t);
	    c1[2] = a * s * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Verrill Surface
	Mathematics by Roger L. Bagula
	4 July 2000
	Graphics by Paul Bourke
	https://paulbourke.net/geometry/verrill/

	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:71)
***************************************************************************/

int	DoVerrillSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = -2 * t * cos(s) + (2 * cos(s)) / t - (2 * t*t*t * cos(3 * s)) / 3;
	    c1[1] = 6 * t * sin(s) - (2 * sin(s)) / t - (2 * t*t*t * sin(3 * s)) / 3;
	    c1[2] = 4 * log(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Versiera of the Agnesi Cylinder
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:149)
***************************************************************************/

int	DoVersieraAgnesiCylinder(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    a = param[5];
    ColourFactor = param[6];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + a * 2.0 / (s*s + 1)) * cos(t);
	    c1[1] = 2.0 * a*s;
	    c1[2] = (R + a * 2.0 / (s*s + 1)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Virich Cyclic Surface
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:203)
***************************************************************************/

int	DoVirichCyclicSurface(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, d, fv, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    d = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    fv = a * b / (sqrt(a * a * sqr(sin(t)) + b * b * sqr(cos(t))));
	    c1[0] = 0.5 * (fv * (1 + cos(s)) + (sqr(d) - sqr(c)) * (1 - cos(s)) / fv) * cos(t);
	    c1[1] = 0.5 * (fv * (1 + cos(s)) + (sqr(d) - sqr(c)) * (1 - cos(s)) / fv) * sin(t);
	    c1[2] = 0.5 * (fv - (sqr(d) - sqr(c)) / fv) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wallis´s Conical Edge
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:30)
***************************************************************************/

int	DoWallisConicalEdge(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = s * cos(t);
	    c1[1] = s * sin(t);
	    c1[2] = c * sqrt(a*a - b*b * cos(t) * cos(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += xgap;
	    }
	t += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wave Ball
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:59)
***************************************************************************/

int	DoWaveBall(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t * cos(cos(t)) * cos(s);
	    c1[1] = t * cos(cos(t)) * sin(s);
	    c1[2] = t * sin(cos(t));
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wave Torus
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:58)
***************************************************************************/

int	DoWaveTorus(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R, r, a, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R = param[4];
    r = param[5];
    a = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R + (r + a * sin(n*t)) * cos(s)) * cos(t);
	    c1[1] = (R + (r + a * sin(n*t)) * cos(s)) * sin(t);
	    c1[2] = (r + a * sin(n*t)) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Waves
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:119)
***************************************************************************/

int	DoWaves(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, b, c, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    b = param[5];
    c = param[6];
    ColourFactor = param[7];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t;
	    c1[1] = a * cos(b*t) * cos(c*s);
	    c1[2] = s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wavy Enneper´s Surface 
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:240)
***************************************************************************/

int	DoWavyEnnepersSurface(void)

    {
    int		i, j;
    double	c1[3], s, u, v, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    s = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    v = yStart;
    InitSurface(c1, 3);						// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	u = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)			// user pressed a key?
		return -1;
	    c1[0] = u * cos(v) - pow(u, (2 * s - 1)) / (2 * s - 1) * cos((2 * s - 1) * v);
	    c1[1] = -u * sin(v) - pow(u, (2 * s - 1)) / (2 * s - 1) * sin((2 * s - 1) * v);
	    c1[2] = 2 / s * pow(u, s) * cos(s * v);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    u += xgap;
	    }
	v += ygap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wavy Torus I
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:165)
***************************************************************************/

int	DoWaveyTorusI(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, r, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    r = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + r * cos(s)) * cos(t);
	    c1[1] = r * sin(s) + R2 * sin(n*t);
	    c1[2] = (R1 + r * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wavy Torus II
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:166)
***************************************************************************/

int	DoWaveyTorusII(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, R1, R2, r, n, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    R1 = param[4];
    R2 = param[5];
    r = param[6];
    n = param[7];
    ColourFactor = param[8];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = (R1 + R2 * cos(n*t) + r * cos(s)) * cos(t);
	    c1[1] = r * sin(s);
	    c1[2] = (R1 + R2 * cos(n*t) + r * cos(s)) * sin(t);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	What is the Lowest Order of the Fractional-Order Chaotic Systems to Behave Chaotically…?
	Dong Peng, Kehui Sun, Shaobo He and AbdulazizO.A. Alamodi 26 December 2018
	https://www.researchgate.net/requests/r41338629
***************************************************************************/

int	DoWhatLowestOrderFractionalOrderChaoticSystems(void)

    {
    int		i;
    double	j, c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, fi, teta, ColourFactor;

    c1[0] = param[10];	// x
    c1[1] = param[11];	// y
    c1[2] = param[12];	// z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    fi = sin(j);
	    s += fi;
	    teta = j * a + s;
	    c1[0] = fi * cos(teta);
	    c1[1] = t;
	    c1[2] = fi * sin(teta);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Whitney Umbrella
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:18)
***************************************************************************/

int	DoWhitneyUmbrella(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[1] = s * t;
	    c1[0] = t;
	    c1[2] = s * s;
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Wreath
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:60)
***************************************************************************/

int	DoWreath(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    ColourFactor = param[4];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = cos(t + 4)/3;
	    c1[1] = cos(t) * sin(s) * cos(s) * sin(t) * cos(s) + 0.5 * cos(t);
	    c1[2] = cos(s) * sin(t) * cos(t) * sin(s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Zindler´s Conoid
	Meier 2001
	http://www.3d-meier.de/ (Parametrische Flächen und Körper 3:72)
	Konrad Zindler - 1934 - DEATH YEAR
	https://www.mathcurve.com/surfaces/zindler/zindler.shtml
***************************************************************************/

int	DoZindlersConoid(void)

    {
    int		i, j;
    double	c1[3], s, t, xgap, ygap, xStart, xEnd, yStart, yEnd, a, ColourFactor;

    c1[0] = param[10]; // x
    c1[1] = param[11]; // y
    c1[2] = param[12]; // z

    xStart = param[0];
    xEnd = param[1];
    yStart = param[2];
    yEnd = param[3];
    a = param[4];
    ColourFactor = param[5];
    totpasses = 10;

    ygap = (yEnd - yStart) / iterations;
    xgap = (xEnd - xStart) / iterations;
    t = yStart;
    InitSurface(c1, 3);							// pass in number of dimensions
    for (i = 0; i < iterations; i++)
	{
	s = xStart;
	curpass = (int)(i * totpasses / iterations);
	for (j = 0; j < iterations; j++)
	    {
	    if (user_data(GlobalHwnd) == -1)				// user pressed a key?
		return -1;
	    c1[0] = t * cos(s);
	    c1[1] = t * sin(s);
	    c1[2] = a * tan(2*s);
	    if (DisplaySurface(c1, ((DWORD)(i / ColourFactor) % threshold), i, 3) < 0)
		return -1;
	    s += ygap;
	    }
	t += xgap;
	}
    PlotExtras();
    CloseZvalues();
    return 0;
    }

/**************************************************************************
	Close z value memory
***************************************************************************/

void	CloseZvalues(void)
    {
    return OscProcess.CloseZvalues();
    }

/**************************************************************************
	Common Setup Routines
***************************************************************************/

int	SurfaceSetup(void)
    {
    xSum = 0.0; 
    ySum = 0.0; 
    zSum = 0.0;
    return 0;
    }

/**************************************************************************
	Common init Routines
***************************************************************************/

void	InitSurface(double c1[], int dimensions)
    {
    OscProcess.InitOscProc(dimensions, mandel_width, ScreenRatio, xdots, ydots, type, subtype, OscAnimProc, &xAxis, &yAxis, &zAxis, BOUNDARY, &Dib, c1, PerspectiveFlag, iterations, OscBackGround, &x_rot, &y_rot, &z_rot, &RemoveHiddenPixels, GlobalHwnd, wpixels, &TrueCol);
    }

/**************************************************************************
	Common Display Routines
***************************************************************************/

int	DisplaySurface(double c1[], DWORD colour, double i, int dimensions)
    {
    return OscProcess.DisplayOscillator(c1, c1, 0.0, colour, i, dimensions, FALSE, 1, 1, hor, vert, VertBias, zBias, OscAnimProc, CoordSystem, threshold, 0.0, 0);
    }

/**************************************************************************
	Compare function for Fractal Map name sort
**************************************************************************/

int	CompareSurface(int	*p1, int *p2)

    {
    char    *a1, *a2;

    a1 = SurfaceSpecific[*p1].name;
    a2 = SurfaceSpecific[*p2].name;
    if (*a1 == '\"')			    // ignore " character
	a1 = a1++;
    if (*a2 == '\"')
	a2 = a2++;
    return (_stricmp(a1, a2));
    }

/**************************************************************************
    Paul's signed power 
**************************************************************************/

double SignedPower(double v,double n)
    {
    if (v >= 0)
	return(pow(v,n));
    else
	return(-pow(-v,n));
    }

/**************************************************************************
	General Surface Routines
***************************************************************************/

int	setup_Surface(void)
    {
    int	    i;

    OscillatorNum = 0;
			// we'd better count how many records we have
    while (SurfaceSpecific[OscillatorNum].name)
	OscillatorNum++;
    for (i = 0; i < OscillatorNum; i++) 
	OscPtrArray[i] = i;
    qsort((void *)OscPtrArray, (size_t)OscillatorNum, sizeof(OscPtrArray[0]), (int (__cdecl *)(const void *, const void *)) CompareSurface);
#ifdef	PRINTOSC
    PrintOsc(OscillatorNum);		// used for listing oscillator names in d:\temp\OscDump.txt
#endif
    return 0;
    }

#ifdef	MULTITHREAD
int	DoSurface(PMYVARIABLES var)
#else
int	DoSurface(void)
#endif
    {
    return (SurfaceSpecific[subtype].calctype());
    }



