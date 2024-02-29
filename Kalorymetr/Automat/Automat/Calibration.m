## CALIBRATION of the TCD sensor electronic circuit

## INPUT: Matrixes containing C and D parameter values for each concentration for each temperature
## INPUT: Any number of rows is possible, but each row have to be complete (as stated below)
## INPUT: Number of rows and parameter value columns may be different for each parameter (C or D)

## OUTPUT: Matrixes containing coeficients of parameter approximation (par = a1*T + b) using regresion (
## OUTPUT: Separately for each concentration
## OUTPUT: It is used in measuremet to calculate C and D parameter value for actual temperature 

## [Temperature] matrix - temperatures for C_raw and D_raw columns 3:6

## [C_raw] matrix - INPUT matrix for C parameter
## Columns are
## CH4% / N2% / C_T1 / C_T2 / ....
## Number of parameter columns can be any number higher then 2 (here 4), 
## But has to fit [Temperatures] matrix 

## [D_raw] matrix - INPUT matrix for D parameter
## Columns are
## CH4% / N2% / D_T1 / D_T2 / ....
## Number of parameter columns can be any number higher then 2 (here 4), 
## But has to fit [Temperatures] matrix 

function [Cmx , Dmx] = Calibration(Temperatures, C_raw, D_raw)
  
  ## calculation C parameter regresion parameters (a1, b) for each row of concentrations
  for i = 1:rows(C_raw)
    [AB(i,:)] = transpose(Lin_T_reg(Temperatures, C_raw(i,3:end)));
  endfor

  ## adding the concentration columns and assigning to the matrix for C parameter
  Cmx = [C_raw(:,1:2), AB];

  ## calculation D parameter regresion parameters (a1, b) for each row of concentrations
  for i = 1:rows(D_raw)
    [AB(i,:)] = transpose(Lin_T_reg(Temperatures, D_raw(i,3:end)));
  endfor

  ## adding the concentration columns and assigning to the matrix  for D parameter
  Dmx =[D_raw(:,1:2), AB];

endfunction

