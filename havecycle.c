//////////////////////////////////////////////////////////////////////
//Subroutine to answer have the graph cycle or not
//Call from MATLAB:
//output=havecycle(graph)
//IN:
//graph - graph matrix Mx2
//OUT:
//TRUE - if graph has cycle
//FALSE - if no
//Author: Bilokha D.O
//////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include "mex.h"
#include <stdlib.h>
#include <string.h>

void AddToCur(unsigned int);
void IAddToCur(unsigned int);

//Struct used to find elementary cycles definition
typedef struct graph_eltag
{
   unsigned int EdgeNum;
   unsigned int StartNode;
   unsigned int EndNode;
} graph_el;

graph_el *CurrentEN;
unsigned int rsz_cur, lsz_cur, sz_graph, last_cur;
double *graph;

void mexFunction(int nlhs, mxArray *plhs[],
						int nrhs, const mxArray *prhs[])
{
   unsigned int i, k, m, cycle, found;
   unsigned int FindFrom;
   unsigned int *Out;

   //get pointer to graph data and its height
   graph= mxGetPr(prhs[0]);
   sz_graph= mxGetM(prhs[0]);
   //Allocate memory for collector massive
   CurrentEN= mxCalloc(1,1);
   //Allocate memory for output
   plhs[0]= mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
   mxSetLogical(plhs[0]);
   Out= mxGetData(plhs[0]);
   //Zero variables
   cycle=false;
   rsz_cur=0; lsz_cur=0;

   //Main loop i is number of edge
   for(i=0; i<sz_graph; i++)
   {
	   //Add edge i to collector massive
	   AddToCur(i);
	   FindFrom= i+1;
	   //Loop for finding cycles with edge i
	   while(lsz_cur)       		//while(sizeof_collector_massive !=0)
	   {
		   found= false;
		   for(k=FindFrom; k<sz_graph; k++)	//Loop for finding edge next to i
		   {
			   if(k != CurrentEN[last_cur].EdgeNum)
			   {

				   if(CurrentEN[last_cur].EndNode==graph[k]) //if(edge_k_is_next_to_edge_i)
				   {
					   found= true;					//set flag
					   AddToCur(k);					//add edge k to collector massive
				   }
				   else
					   if(CurrentEN[last_cur].EndNode==graph[k+sz_graph]) //if(edge_k_is_next_to_edge_i)
					   {
						   found= true;					//set flag
						   IAddToCur(k);					//add edge k to collector massive
					   }
				   if(found)
				   {
					   for(m=0; m<last_cur; m++)	
					   {
						   if(CurrentEN[m].StartNode==CurrentEN[last_cur].EndNode)
						   {
							   cycle= true;
							   lsz_cur= 0;
							   break;
						   }
					   }
					   break;
				   } 
			   }
		   }
		   if(!found)		//if continue not found
		   {
			   //remove last edge from collector massive
			   last_cur--;
			   FindFrom= CurrentEN[--lsz_cur].EdgeNum+1;
		   }
		   else
		   {
			   FindFrom= i+1;
		   }
	   }
	   if(cycle)
		   break;
   }

   //Form output for MATLAB
   *Out= cycle;
}

//Subroutine to add to the collector massive edge inx
void AddToCur(unsigned int inx)
{
	if(rsz_cur<=lsz_cur)
   {
   	CurrentEN= mxRealloc(CurrentEN, sizeof(graph_el)*(++lsz_cur));
      rsz_cur= lsz_cur;
   }
   else
   	lsz_cur++;
   last_cur= lsz_cur-1;
   CurrentEN[last_cur].EdgeNum= inx;
   CurrentEN[last_cur].StartNode= (unsigned int)graph[inx];
   inx += sz_graph;
   CurrentEN[last_cur].EndNode= (unsigned int)graph[inx];
   inx += sz_graph;
}

void IAddToCur(unsigned int inx)
{
	if(rsz_cur<=lsz_cur)
   {
   	CurrentEN= mxRealloc(CurrentEN, sizeof(graph_el)*(++lsz_cur));
      rsz_cur= lsz_cur;
   }
   else
   	lsz_cur++;
   last_cur= lsz_cur-1;
   CurrentEN[last_cur].EdgeNum= inx;
   CurrentEN[last_cur].EndNode= (unsigned int)graph[inx];
   inx += sz_graph;
   CurrentEN[last_cur].StartNode= (unsigned int)graph[inx];
   inx += sz_graph;
}
