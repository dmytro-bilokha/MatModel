//////////////////////////////////////////////////////////////////////
//Subroutine to compute transfer matrix
//Call from MATLAB:
//transm= formmat(Num, Den, dt)
//IN:
//Num - numerator matrix
//Den - denumerator matrix
//dt - time step
//OUT:
//transm - transfer matrix
//Author: Bilokha D.O
//////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include "mex.h"
#include <stdlib.h>
#include <string.h>


void
mexFunction(int nlhs, mxArray *plhs[],
				int nrhs, const mxArray *prhs[])
{
    double *data, *NumPr, *DenPr;
    double dt, powdt, numerator, denumerator;
    unsigned int num_cols, num_rows;
    int i, k, j;
    unsigned int addr=0;
    unsigned int Num_len, Den_len;
    mxArray *CellNum, *CellDen;

    //get number of columns in numerator matrix
    num_cols= mxGetN(prhs[0]);
    //get number of rows in numerator matrix
    num_rows= mxGetM(prhs[0]);
    //get dt
    dt= *mxGetPr(prhs[2]);
    //create transfers matrix
    plhs[0]= mxCreateDoubleMatrix(num_rows, num_cols, mxREAL);
    data= mxGetPr(plhs[0]);
    for(i=0; i < num_cols; i++)
    {
	for(k=0; k < num_rows; k++)
	{
	    //Get cells
	    CellNum= mxGetCell(prhs[0], addr+k);
	    CellDen= mxGetCell(prhs[1], addr+k);
	    //Get numerator and denumerator lengths
	    Num_len= mxGetN(CellNum);
	    Den_len= mxGetN(CellDen);
	    //Get numerator and denumerator pointers
	    NumPr= mxGetPr(CellNum);
	    DenPr= mxGetPr(CellDen);
	    //Calculate numerator
	    powdt= 1; numerator= 0;
	    for(j=Num_len-1; j >= 0; j--)
	    {
		numerator += NumPr[j]*powdt;
		powdt *= dt;
	    }
	    //Calculate denumerator
	    powdt= 1; denumerator= 0;
	    for(j=Den_len-1; j >= 0; j--)
	    {
		denumerator += DenPr[j]*powdt;
		powdt *= dt;
	    }
	    //Store data
	    data[addr+k]= numerator/denumerator;
	}
	addr += num_rows;
    }
}

