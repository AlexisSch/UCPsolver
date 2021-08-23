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



You could create classes for every block of a decomposition ?


# How do i create a new decomposition ?

Don't forget to add the corresponding folder for the objects !
Change the 3 classes :
- master : change the vectors of contraints and of variables ; change add_column, change the constraints definition
- pricer : same, change all the variables and constriant



errors in make : 
expected identifier before ‘)’ token : manque une virgule
 expected unqualified-id before ‘int’ : virgule à la place du point virg




a few rules :
- in pricer, only put things usefull for the pricer. Don't send the full reduced costs matrix, for example.



trucs classiques :

int number_of_units( m_instance_ucp->get_units_number() );
int number_of_time_steps( m_instance_ucp->get_time_steps_number() );
for( int i_unit = 0; i_unit < number_of_units; i_unit ++)
{
    for(int i_time_step = 0; i_time_step < number_of_time_steps; i_time_step ++)
    {
        // write here
    }
}