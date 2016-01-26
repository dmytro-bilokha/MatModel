# MatModel

In the repository I want to share program to model and analyze electrical
circuits with thyristor valves. The model was developed by me in 2007
during work on PhD thesis. It consists of the main MATLAB script, some
MATLAB functions and a little of modules written in C for better
performance.

The main goal of the model is to account current and voltage values in the
given circuit. Model is very flexible, because it is possible to set any
algorithms of driving thyristors via editing sources and writing
additional modules.

The model takes topological matrix of the circuit and form the signal
graph. It determines dependent, independent and inertial nodes, forms
transfer matrix from independent and inertial nodes to dependent nodes.
Program accounts current and voltage values on the each step of modeling.
Thyristor valves state is changed from closed to opened in given moments of
time. On current zero-cross state of the valve is automatically changed to
closed.

The model was tested on some well-discovered circuits (rectifiers and
invertors).

## Model Description

Model consists of following files:

 * `FindDInodes.m` - the function to group circuit nodes by types;
 * `MakeSigGraph.m` - the function to make signal graph;
 * `MakeTree.m` - the function to make a tree from the topology matrix;
 * `MarkCycle.m` - finds all branches forming loop with the connection;
 * `SimulateSh.m` - the main MATLAB macros to lunch the model;
 * `computeej.c` - the subroutine to compute voltages and currents of sources;
 * `formmat.c` - the subroutine to compute transfer matrix;
 * `formzero.m` - calculates transfer matrix with zero time step;
 * `havecycle.c` - determines does the graph have a loop or no;
 * `make3mat.c` - the subroutine to form graph's transfer matrix.


## Input Data Description

The model needs some data to be present in the MATLAB workspace.

### Constants

 * _Tanalys_ - duration of modeling, seconds;
 * _dt_ - model time step, seconds;
 * _d0_ - tolerance of determining the valves closing moment, seconds;
 * _Nd_ - quantity of time steps in the thyristors driving period;
 * _S_ - stage of decoupling;

### Model parameters

 * _Kvs_ - vector. Each element of it determines step on each thyristor
should be opened;

 * _EJm_ - voltage and current sources data matrix. Each row corresponds
to one source. If the first element of the row equals zero, source is
constant with value from the second element of the row. If the first
element is not zero, the source is sine with the magnitude from the second
element, angle frequency from the third element and phase from the 4th
element of the row;

 * _IN1_ - vector of starting values for inertial variables (voltage of capacities
and current of inductors);

 * _Topology_ - circuit topology matrix. Each row of the matrix corresponds to
the circuit element. The first element of the row determines type of circuit
element: 1 for voltage source; 2 for capacitor; 3 for open thyristor; 4 for resistance;
5 for closed thyristor; 6 for inductor; 7 for current source. The second column of the
matrix determines number of the circuit element. 3th and 4ch columns mean connection
nodes. 5th element is value (resistance for resistor, capacity for capacitor, etc).
6th element determines which data to store for the circuit element (0=no data, 1=current,
2=voltage). Inertial, independent values are stored for calculation even if zero set.
Also, thyristors voltages and currents are stored anyway.

 * _kmc_ -  matrix of magnetic coupling for inductors. Each row corresponds to one
connection. The first and second elements of the row contains inductors numbers.
The third column contains coupling value.

## How to Use

1. Compile all C modules according to MATLAB requirements (it is OS dependent).

2. Copy all compiled C modules and MATLAB m-files to working directory.

3. Lunch MATLAB and create input data in the workspace. Some examples are included in _/Test_ directory.

4. Start modeling via lunching SimulateSh.m from the MATLAB's console.

5. Use all the power of MATLAB tools to analyze the modeling result.
