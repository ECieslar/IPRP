################################################################################
##                                                                            ##
##                                CALIBRATION                                 ##
##                                                                            ##
################################################################################

## INPUT: Files containing C and D parameter values for each concentration for each temperature
## INPUT: Any number of rows is possible, but each row have to be complete (as stated below)
## INPUT: Number of rows and parameter value columns may be different for each parameter (C or D)

## OUTPUT: File containing coeficients of parameter approximation (par = a1*T + b) using regresion
## OUTPUT: Separately for each concentration
## OUTPUT: It is used in measurement to calculate C and D parameter value for actual temperature 

## Temperatures for C_raw and D_raw columns 3:7
## T1, T2, T3, T4 ...
Temperatures = csvread("temperatures.csv");

## Reading the INPUT file for C parameter
## File columns are:
## CH4% / N2% / C_T1 / C_T2 / ....
## Number of parameter columns can be any number higher then 2 (here 5), 
## But has to fit [Teperatures] matrix stated above
C_raw = csvread("cdata.csv");

## Reading the INPUT file for D parameter
## File columns are:
## CH4% / N2% / D_T1 / D_T2 / ....
## Number of parameter columns can be any number higher then 2 (here 5), 
## But has to fit [Teperatures] matrix stated above
D_raw = csvread("ddata.csv");

## Calculating the calibration file (linear regresion for each concentration C=f(T) and D=g(T)
[Cmx, Dmx] = Calibration(Temperatures, C_raw, D_raw);

## Saving to the calibration file for C parameter
csvwrite("C2reglin.csv", Cmx);

## Saving to the calibration file for D parameter
csvwrite("D2reglin.csv", Dmx);

fclose("all");

