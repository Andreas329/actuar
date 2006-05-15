#include "R.h"
#include "Rmath.h"

void rpareto(int *n, double *alpha, double *lambda, double *y)
{
    int i;

    if (R_IsNaN(*alpha) || R_IsNaN(*lambda)) Rf_error ("alpha et lambda doivent �tre des nombres");
    if (*alpha <= 0)  Rf_error ("alpha et lambda doivent �tre positifs");
    if (*lambda <= 0) Rf_error ("alpha et lambda doivent �tre positifs");
    
    for (i = 0; i <= *n; i++)
    {
	GetRNGstate();
	y[i] = *lambda * pow(unif_rand(), -1.0 / *alpha) - *lambda;
	PutRNGstate();
    }
}
