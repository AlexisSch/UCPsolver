#!/bin/sh


# keys
# data = 10-24

echo Welcome, lets solve the ucp
cd ..

# We initialize the exit_data.csv
cd exit
> exit_data.csv
# echo 'N, T, cost_compact, time_compact, cost_linear, time_linear, cost_column2, time_column2, number_column2,  cost_column3, time_column3, number_column3' >> exit_data.csv
echo 'N, T, cost_linear, time_linear,  cost_column3, time_column3, number_column3, cost_time_decompo, solve_time_decompo, number_columns_time_decompo' >> exit_data.csv
echo >> exit_data.csv
cd ..



# We go over the instances and execute the solving

#first with soplex
make
echo Solving with soplex
cd bin
for i in 1 2 3 4 5 6 7 8 9 10
do
    echo "Starting solving of instance $i ..."
    ./ucp.exe   ../data/10-24/instance_$i.txt   ../exit/exit_data.csv   1000   0
done

echo script finished... byebye
