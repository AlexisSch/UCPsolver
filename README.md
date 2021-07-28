# Solving UCP with various decomposition and using Column Generation

Welcome to this code that allows, for now, to get a lower bound on the Unit Commitment Problem instances !



### How do I use this code ?
You must first "make" the executable. Update the makefile with your local scip directory, and then just type  the make command into your terminal.
If the compilation went alright, you can find the executable in the bin folder. Use it from the terminal. 
You must specify one parameter : the entry data file directory.
You can also add other parameters : in order, the exit data file directory, the time limit for resolution (yet to be implemented), the display verbosity (most of the time you should put a 0 here).




### How does the code work ?

In the src folder, you can find all the code in .cpp or .h files. It is organised the following way :

- the main.cpp file is where you make what you want from the code
- you can also find a bunch of other cpp and h files corresponding to things that are generic. They may be moved elsewhere later.
- In the /Decomposition folder, you can find a bunch of stuff needed by all decompositions. First, the VariableMaster, which have so far the same structure for every decomposition. Then FormulationMaster and FormulationPricer, which are abstract class. You will need to derive these classes in your decomposition, adding the correct constraints to get the column generation working
- For a decomposition, there should be a folder /MyDecomposition, where there should be 3 classes : FormulationMasterMyDecomposition and FormulationPricerMyDecomposition, derived from the classes from /decomposition, and ObjPricerUCPMyDecomposition, which is derived from ObjPricer, a class from SCIP. This class allows us to tell SCIP how to get the column generation rolling.
