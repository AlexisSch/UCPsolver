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

To use CPLEX ?
make LPS=cpx ZIMPL=false


You could create classes for every block of a decomposition ?


# How do i create a new decomposition ?
In the src/TemplateDecomposition, you will find templates (not sure that it's the right name ?) for every classes that needs to be created.
You will need to create three inherited classes :
- FormulationMaster : for the master problem, from here you will manipulate constraints, columns and variables that are used by the Master Problem.
- FormulationPricer : same for the Pricer Problem
- ObjPricer : this class is needed by Scip, you will pilote the pricing problem from here, and see if columns need to be added. There are also a few functions (forgot the name) that needs to be called to setup the column generation.

The templates should tell you where to put the right things. Avoid to copy pasta as much as possible, as it is likely to create many errors. If you fill where it is needed step by step, it should work pretty well.
Some decomposition might recquire some specific stuff (for example if it is an overlapping decomposition, or if you want to add some functionnalities...), which the template do not cover.

# A few commentaries
As you might observe while navigating this code, a few things have changed in the way I code since the beggining of the project - hopefully for the better. The code might not be totally clean, but the last decompositions done should be pretty easy to use.











