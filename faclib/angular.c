/*
 *   FAC - Flexible Atomic Code
 *   Copyright (C) 2001-2015 Ming Feng Gu
 *   Portions Copyright (C) 2010-2015 Evgeny Stambulchik
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/************************************************************
  Implementation of module "angular".

  All angular momentum arguments are twice their actual
  values to represent half-integer using integers.

  Author: M. F. Gu, mfgu@stanford.edu
*************************************************************/

#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_coupling.h>

#include "consts.h"
#include "angular.h"

double LnFactorial(unsigned int n)
{
    if (n == 0) {
        return 0.0;
    } else {
        return gsl_sf_lnfact(n);
    }
}

double LnInteger(unsigned int n)
{
    if (n == 0) {
        return -100.0;
    } else {
        return log((double) n);
    }
}

/* 
** FUNCTION:    Triangle.
** PURPOSE:     check for triangular relation.
** INPUT:       {int j1},
**              angular momentum.
**              {int j2},
**              angular momentum.
**              {int j3},
**              angular momentum.
** RETURN:      {int},
**              0: triangular relation failed.
**              1: triangular relation holds.
** SIDE EFFECT: 
** NOTE:        
*/
int Triangle(int j1, int j2, int j3) {
  int i;

  j1++;
  j2++;
  j3++;

  i = j2 - j3;
  if (j1 >= abs(i) + 1 && j1 <= j2 + j3 - 1) 
    return 1;
  else 
    return 0;
}

/* 
** calculate the Wigner 3j symbols.
*/
double W3j(int j1, int j2, int j3, int m1, int m2, int m3) {
  return gsl_sf_coupling_3j(j1, j2, j3, m1, m2, m3);
}

/* 
** FUNCTION:    W6j.
** PURPOSE:     calculate the 6j symbol.
*/
double W6j(int j1, int j2, int j3, int i1, int i2, int i3) {
  return gsl_sf_coupling_6j(j1, j2, j3, i1, i2, i3);
}

/* 
** FUNCTION:    W6jTriangle.
** PURPOSE:     determine if 6j symbol is permitted
**              by the triangular constraints.
** INPUT:       {int j1},
**              angular momentum.
**              {int j2},
**              angular momentum.
**              {int j3},
**              angular momentum.
**              {int i1},
**              angular momentum.
**              {int i2},
**              angular momentum.
**              {int i3},
**              angular momentum.
** RETURN:      {int},
**              0: 6j symbol forbidden.
**              1: 6j symbol allowed.
** SIDE EFFECT: 
** NOTE:        
*/
int W6jTriangle(int j1, int j2, int j3, int i1, int i2, int i3) {
  return (Triangle(j1, j2, j3) &&
	  Triangle(j1, i2, i3) &&
	  Triangle(i1, j2, i3) &&
	  Triangle(i1, i2, j3));
}
  
/* 
** FUNCTION:    W9j.
** PURPOSE:     calculate the 9j symbol.
*/     
double W9j(int j1, int j2, int j3,
	   int i1, int i2, int i3,
	   int k1, int k2, int k3) {
  return gsl_sf_coupling_9j(j1, j2, j3, i1, i2, i3, k1, k2, k3);
}

/* 
** FUNCTION:    W9jTriangle.
** PURPOSE:     determine if 9j symbol is allowed by
**              the triangular constraints.
** INPUT:       {int j1},
**              angular momentum.
**              {int j2},
**              angular momentum.
**              {int j3},
**              angular momentum.
**              {int i1},
**              angular momentum.
**              {int i2},
**              angular momentum.
**              {int i3},
**              angular momentum.
**              {int k1},
**              angular momentum.
**              {int k2},
**              angular momentum.
**              {int k3},
**              angular momentum.
** RETURN:      {int},
**              0: 9j symbol forbidden.
**              1: 9j symbol allowed.
** SIDE EFFECT: 
** NOTE:        
*/     
int W9jTriangle(int j1, int j2, int j3,
		int i1, int i2, int i3,
		int k1, int k2, int k3) {
  return (Triangle(j1, j2, j3) &&
	  Triangle(i1, i2, i3) &&
	  Triangle(k1, k2, k3) &&
	  Triangle(j1, i1, k1) &&
	  Triangle(j2, i2, k2) &&
	  Triangle(j3, i3, k3));
}

/* 
** FUNCTION:    WignerEckartFactor.
** PURPOSE:     calculate the geometric prefactor in 
**              Wigner Eckart theorem, 
**              (-1)^{jf-mf}sqrt(2*jf+1)W3j(jf, k, ji, -mf, q, mi)
** INPUT:       {int jf},
**              angular momentum.
**              {int k },
**              angular momentum.
**              {int ji},
**              angular momentum.
**              {int mf},
**              projection of jf.
**              {int q },
**              projection of k.
**              {int mi},
**              projection of mi.
** RETURN:      {double},
**              prefactor.
** SIDE EFFECT: 
** NOTE:        
*/
double WignerEckartFactor(int jf, int k, int ji, int mf, int q, int mi) {
  double r;

  if (!Triangle(jf, k, ji)) return 0.0;
  if (mi + q - mf) return 0.0;

  r = sqrt(jf + 1.0);
  if (IsOdd((jf-mf)/2)) r = -r;
  r *= W3j(jf, k, ji, -mf, q, mi);
  return r;
}

/* 
** FUNCTION:    ClebschGordan.
** PURPOSE:     claculate the Clebsch Gordan coeff.
** INPUT:       {int j1},
**              angular momentum.
**              {int m1},
**              projection of j1.
**              {int j2},
**              angular momentum.
**              {int m2},
**              projection of j2.
**              {int jf},
**              angular momentum, final result 
**              of the coupling of j1 and j2.
**              {int mf},
**              projection of jf.
** RETURN:      {double},
**              CG coefficients.
** SIDE EFFECT: 
** NOTE:        
*/
double ClebschGordan(int j1, int m1, int j2, int m2, int jf, int mf) {
  double r;
  r = sqrt(jf+1.0);
  r *= W3j(j1, j2, jf, m1, m2, -mf);

  if (IsOdd((j1-j2+mf)/2)) r = -r;
  return r;
}

/* 
** FUNCTION:    ReducedCL.
** PURPOSE:     calculate the reduced matrix element
**              of the normalized spherical harmonics <ja||C^L||jb>
** INPUT:       {int ja},
**              angular momentum.
**              {int k },
**              rank of the spherical harmonics.
**              {int jb},
**              angular momentum.
** RETURN:      {double},
**              reduced matrix element.
** SIDE EFFECT: 
** NOTE:        it does not check for the triangular delta 
**              involving the orbital angular momenta.
*/
double ReducedCL(int ja, int k, int jb) {
  double r;

  r = sqrt((ja+1.0)*(jb+1.0))*W3j(ja, k, jb, 1, 0, -1);
  if (IsOdd((ja+1)/2)) r = -r;
  return r;
}

/*
** Wigner d-matrix <jm|exp(-iJ_y*a)|jn>
** j2 = j*2, m2 = m*2, n2 = n*2
*/
double WignerDMatrix(double a, int j2, int m2, int n2) {
  double b, c, ca, sa, x;
  int k, kmin, kmax;

  a *= 0.5;
  kmin = Max(0, (m2+n2)/2);
  kmax = Min((j2+m2)/2, (j2+n2)/2);
  ca = cos(a);
  sa = sin(a);
  x = 0.0;
  for (k = kmin; k <= kmax; k++) {
    b = pow(ca, (2*k-(m2+n2)/2));
    b *= pow(sa, (j2+(m2+n2)/2-2*k));
    c = LnFactorial(k);    
    c += LnFactorial((j2+m2)/2-k);
    c += LnFactorial((j2+n2)/2-k);
    c += LnFactorial(k-(m2+n2)/2);
    b /= exp(c);
    if (IsOdd(k)) b = -b;
    x += b;
  }
  c = LnFactorial((j2+m2)/2);
  c += LnFactorial((j2-m2)/2);
  c += LnFactorial((j2+n2)/2);
  c += LnFactorial((j2-n2)/2);
  c = exp(0.5*c);
  if (IsOdd((j2+m2)/2)) c = -c;
  x *= c;

  return x;
}
