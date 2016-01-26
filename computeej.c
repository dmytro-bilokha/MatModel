
//////////////////////////////////////////////////////////////////////
//Subroutine to compute E and J
//Call from MATLAB:
//ejv= computeej(EJm, t)
//IN:
//EJm - matrix with sourcers' data
//t - present time
//OUT:
//ejv - E and J current values vector
//Author: Bilokha D.O
//////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include "mex.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


void mexFunction(int nlhs, mxArray *plhs[],
				int nrhs, const mxArray *prhs[])
{
	double *EJm, *ejv;
	double t;
	unsigned int num_scs, i;

	//get pointer to EJm and number of sources
	EJm= mxGetPr(prhs[0]);
	num_scs= mxGetM(prhs[0]);
	//get present time
	t= *mxGetPr(prhs[1]);
	//Allocate memory for ej vector
	plhs[0]= mxCreateDoubleMatrix(num_scs, 1, mxREAL);
	ejv= mxGetPr(plhs[0]);
	for(i=0; i<num_scs; i++)
	{
		if(((unsigned int)EJm[i])==0)
			ejv[i]= EJm[i+num_scs];
		else
			ejv[i]= EJm[i+num_scs]*sin(EJm[i+2*num_scs]*t+EJm[i+3*num_scs]);
	}
}

