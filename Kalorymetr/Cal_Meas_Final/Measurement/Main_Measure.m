################################################################################
##                                                                            ##
##                                MEASUREMENT                                 ##
##                                                                            ##
################################################################################

## Measured parameters (for Ta = 660, CH4 = 32, N2 = 35, C2H6 = 33)
Ta = 660;
CH4real = 32;
N2real = 35;
printf ("Methane real: %d \n", CH4real); 
printf ("Nitrogen real: %d \n", N2real); 
C2H6real = 100 - CH4real - N2real;
printf ("Ethane real: %d \n\n", C2H6real);

## Test parameters - from the C/D_values_var_T files
Ctest = 1.582292599;
Dtest = 20931.94184;

## Initial Newton-Raphson solution error value
limit = 0.00001;

## Reading the calibration tables for C and D parameters
C_reglin = csvread("C2reglin.csv");
D_reglin = csvread("D2reglin.csv");

## Calculating the results
[CH4result, N2result] = Measurement(Ta, Ctest, Dtest, C_reglin, D_reglin, limit);

## Printing the results
printf ("Methane calculated: %d \n", CH4result); 
printf ("Nitrogen calculated: %d \n", N2result); 
C2H6result = 100 - CH4result - N2result;
printf ("Ethane calculated: %d \n", C2H6result); 

fclose("all");

