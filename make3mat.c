//////////////////////////////////////////////////////////////////////
//Subroutine to form graph's transfer matrix
//Call from MATLAB:
//[Num,Den]= make3mat(graph,dnodes,inodes)
//IN:
//graph - signal graph matrix
//dnodes - vector of dependent nodes
//inodes - vector of independent nodes
//OUT:
//Num - 3d numerator transfers matrix
//Den - 3d denumerator transfers matrix
//Author: Bilokha D.O
//////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include "mex.h"
#include <stdlib.h>
#include <string.h>

#define GRAPH_HEIGHT 4

void AddToCur(unsigned int);
void AddCycle(unsigned int);

//Struct used to find elementary cycles definition
typedef struct graph_eltag
{
    unsigned int EdgeNum;	//Numder of  edge
    unsigned int StartNode;	//Start node's number
    unsigned int EndNode;	//End node's number
    double Transfer;		//Edge's transfer
    unsigned int Plevel;	//Level of dt
} graph_el;

graph_el *CurrentEN;		//Edges' collector array
unsigned int rsz_cur, lsz_cur, sz_graph, last_cur, cur_len;
double *graph, *Trans;
unsigned int *HcyclArray, *Levs;
unsigned int num_hcyc, Ss_len, Sz_len;
double *Ss, *Sz, *NumData, *DenData;
mxArray *NumPoly, *DenPoly;
char *havelast;

void
mexFunction(int nlhs, mxArray *plhs[],
				int nrhs, const mxArray *prhs[])
{
    unsigned int i, k, m, j, cycle, found, addr, addrh;
    char *ConMatrix;		//Incedency matrix of cycles
    unsigned int *dst, *src;
    unsigned int cur_hlen, number, sum;
    unsigned int FindFrom, num_cyc, dinx, iinx;
    unsigned int *Nodes, *len;
    double *dnodes, *inodes;
    unsigned int num_dn, num_in;
    double prod;
    double *gr_arg;

    //get pointer to graph data and number of edges
    gr_arg= mxGetPr(prhs[0]);	//Get pointer to graph
    sz_graph= mxGetN(prhs[0]);	//Get number of edges in graph
    graph= mxCalloc((sz_graph+1)*GRAPH_HEIGHT,sizeof(double));	//Allocate memory to copy graph
    memcpy(graph, gr_arg, sz_graph*GRAPH_HEIGHT*sizeof(double));//Copy graph from gr_arg to graph
    sz_graph++;
    //get pointer to dependent nodes array and it length
    dnodes= mxGetPr(prhs[1]);
    num_dn= mxGetN(prhs[1]);
    //get pointer to independent nodes array and it length
    inodes= mxGetPr(prhs[2]);
    num_in= mxGetM(prhs[2]);
    //Allocate memory for collector massive
    CurrentEN= mxCalloc(1,1);
    //Allocate memory for outputs
    plhs[0]= mxCreateCellMatrix(num_dn, num_in);	//Numerator's cell matrix
    plhs[1]= mxCreateCellMatrix(num_dn, num_in);	//Denumerator's cell matrix
    Ss= mxCalloc(1, sizeof(double)*(sz_graph+2));	
    Sz= mxCalloc(1, sizeof(double)*(sz_graph+2));
    //Allocate memory for elementary cycles' arrays
    len= mxCalloc(1,1);		//Number of edges in cycle array
    havelast= mxCalloc(1,1);	//
    Nodes= mxCalloc(1,1);	//Cycle start nodes array
    Trans= mxCalloc(1,1);	//Cycles' transfers
    Levs= mxCalloc(1,1);	//P-level of cycle
    HcyclArray= mxCalloc(1,1);	//
    ConMatrix= mxCalloc(1,1);	//Incendency matrix
    //Set system variables
    rsz_cur=0;

    for(dinx=0; dinx<num_dn; dinx++)	//repeat for each dependent node
    {
	for(iinx=0; iinx<num_in; iinx++)	//repeat for each independent node
	{
	    num_cyc= 0; lsz_cur= 0;
	    Ss_len= 1; Sz_len=1;
	    Ss[0]= 0; Sz[0]= 1;
	    addr= (sz_graph-1)*GRAPH_HEIGHT;
	    graph[addr++]=dnodes[dinx];		//Add closing edge to
	    graph[addr++]=inodes[iinx];		//graph to calculate transfer
	    graph[addr++]=1;			//from node inodes[iinx]
	    graph[addr]=0;			//to node dnodes[dinx]
	    //Main loop i is number of edge
	    for(i=0; i<sz_graph; i++)	//repeat for each graph edge
	    {
		//Add edge i to collector massive
		AddToCur(i);	//add edge to array CurrentEN
		FindFrom= i+1;
		//Loop for finding cycles with edge i
		while(lsz_cur)	//while(sizeof_collector_massive !=0)
		{
		    found= false;
		    cycle= false;
		    for(k=FindFrom; k<sz_graph; k++)	//Loop for finding edge next to i
		    {
			//if(edge_k_is_next_to_edge_i)
			if(CurrentEN[last_cur].EndNode==graph[k*GRAPH_HEIGHT])
			{
			    found= true;	//set flag
			    AddToCur(k);	//add edge k to collector massive
			    //if(now_we_have_cycle)
			    if(CurrentEN[last_cur].EndNode==CurrentEN[0].StartNode)
			    {
				sum=0; prod=1;
				cycle= true;
				num_cyc++;
				addr=sz_graph*num_cyc;
				//Resize massives
				Nodes= mxRealloc(Nodes, addr*sizeof(unsigned int));
				Trans= mxRealloc(Trans, num_cyc*sizeof(double));
				Levs= mxRealloc(Levs, num_cyc*sizeof(unsigned int));
				addr -= sz_graph;
				//add new elementary cycle
				len= mxRealloc(len, num_cyc*sizeof(unsigned int));
				havelast= mxRealloc(havelast, num_cyc*sizeof(char));
				len[num_cyc-1]= lsz_cur;
				havelast[num_cyc-1]= false;
				for(m=0; m<lsz_cur; m++)
				{
				    if(CurrentEN[m].EdgeNum+1==sz_graph)
					havelast[num_cyc-1]= true;
				    Nodes[addr++]= CurrentEN[m].StartNode;
				    sum += CurrentEN[m].Plevel;
				    prod *= CurrentEN[m].Transfer;
				}
				Trans[num_cyc-1]= -prod;
				Levs[num_cyc-1]= sum;
			    }
			    else
			    {
				for(m=1; m<lsz_cur; m++)	//do we have not normal cycle?
				{
				    if(CurrentEN[m].StartNode
					    ==CurrentEN[last_cur].EndNode)
				    {
					cycle= true;
					break;
				    }
				}
			    }
			    break;
			}
		    }
		    if(cycle | (!found))	//if continue not found or cycle found
		    {
			//remove last edge from collector massive
			last_cur--;
			FindFrom= CurrentEN[--lsz_cur].EdgeNum+1;
		    }
		    else
			FindFrom= i+1;
		}
	    }
	    //Now in arrays we have data on elementary cycles
	    //Reallocate memory for high cycles
	    addr= num_cyc*num_cyc*sizeof(char);
	    ConMatrix= mxRealloc(ConMatrix, addr);
	    memset(ConMatrix, false, addr);
	    HcyclArray= mxRealloc(HcyclArray, num_cyc*sizeof(unsigned int));
	    num_hcyc= 0;
	    number= num_cyc-1;
	    //Loop to form incedency cycles matrix
	    //i is number of elementary cycle
	    for(i=0; i<number; i++)
	    {
		addr= i*sz_graph;
		cur_len= len[i];
		for(k=i+1; k<num_cyc; k++) 	//test cycle i cycles from i+1 
		{
		    addrh= k*sz_graph;
		    cur_hlen= len[k];
		    found= false;
		    for(m=0; m<cur_len; m++)	//test are cycles i and k independent? 
		    {
			for(j=0; j<cur_hlen; j++)
			{
			    if(Nodes[addrh+j]==Nodes[addr+m])
			    {
				found= true;
				goto exit_loops;
			    }
			}
		    }
exit_loops:
		    if(!found)
			//If cycles i and k are independent, set true in incedency matrix
			ConMatrix[i*num_cyc+k]=true;
		}
	    }
	    //Now we have incedency matrix and hcycles==cycles
	    //The loop is to find all high-level cycles
	    cur_len=0;
	    for(i=0; i<number; i++)
	    {
		AddCycle(i);
		FindFrom= i+1;
		while(cur_len)
		{
		    cycle= false;
		    for(k=FindFrom; k<num_cyc; k++)
		    {
			cycle= true;
			for(m=0; m<cur_len; m++)
			{
			    if(!ConMatrix[HcyclArray[m]*num_cyc+k])
			    {
				cycle= false;
				break;
			    }
			}
			if(cycle)
			{
			    //Here we have high level cycle
			    //We are to add it to collector massive
			    FindFrom= k+1;
			    AddCycle(k);
			}
		    }
		    if(!cycle)
		    {
			//Cycle not found. We are to delete last cycle
			cur_len--;
			FindFrom= HcyclArray[cur_len]+1;
		    }
		}
	    }

	    cur_len= 0;
	    AddCycle(num_cyc-1);
	    //Set proper matrix cell to polynom found
	    NumPoly= mxCreateDoubleMatrix(1, Ss_len, mxREAL);	//Create numerator polynom
	    NumData= mxGetPr(NumPoly);				//Get numerator's data pointer
	    DenPoly= mxCreateDoubleMatrix(1, Sz_len, mxREAL);	//Create denumerator polynom
	    DenData= mxGetPr(DenPoly);				//Get denumerator's data pointer
	    for(m=0; m < Ss_len; m++)				//Copy numerators coefficients with revers
		NumData[Ss_len-m-1]= Ss[m];
	    for(m=0; m < Sz_len; m++)				//Copy denumerators' coefficients with revers
		DenData[Sz_len-m-1]= Sz[m];
	    mxSetCell(plhs[0], num_dn*(iinx)+dinx, NumPoly);	//Set numerator cell to polynom
	    mxSetCell(plhs[1], num_dn*(iinx)+dinx, DenPoly);	//Set denumerator cell to polynom
	}
    }
    //Form output for MATLAB
}

//Subroutine to add to the collector massive edge inx
void 
AddToCur(unsigned int inx)
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
    inx *= GRAPH_HEIGHT;
    CurrentEN[last_cur].StartNode= (unsigned int)graph[inx++];
    CurrentEN[last_cur].EndNode= (unsigned int)graph[inx++];
    CurrentEN[last_cur].Transfer= graph[inx++];
    CurrentEN[last_cur].Plevel= (unsigned int)graph[inx];
}

//Subroutine to add to the collector massive cycle inx
void 
AddCycle(unsigned int inx)
{
    unsigned int pos, el_cyc, tpow=0;
    char flagl=false;
    double cyc_trans=1;

    num_hcyc++;
    HcyclArray[cur_len++]= inx;
    for(pos=0; pos<cur_len; pos++)
    {
	el_cyc= HcyclArray[pos];
	if(havelast[el_cyc])
	    flagl=true;
	cyc_trans *= Trans[el_cyc];
	tpow += Levs[el_cyc];
    }
    if(flagl)
    {
	//Ss += cyc_trans;
	if(tpow+1 > Ss_len) //if new polynom level > old
	{
	    for(pos= Ss_len; pos < tpow+1; pos++)	//Zero empty elements
		Ss[pos]= 0;
	    Ss_len= tpow+1;		//Set size of array
	}
	Ss[tpow] -= cyc_trans;		//Save polynom's coefficient
    }
    else
    {
	//Sz += cyc_trans;
	if(tpow+1 > Sz_len)
	{
	    for(pos= Sz_len; pos < tpow+1; pos++)
		Sz[pos]= 0;
	    Sz_len= tpow+1;
	}
	Sz[tpow] += cyc_trans;	
    }
}
