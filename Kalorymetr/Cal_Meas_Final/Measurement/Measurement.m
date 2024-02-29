#Meaurement with the TCD sensor electronic circuit

#INPUT: Matrixes containing C and D regresion line parameters for each concentration 
#INPUT: Any number of rows is possible, but each row have to be complete (as stated below)
#INPUT: Number of rows and parameter value columns may be different for each parameter (C or D)

#INPUT: Ta - Measured value of current gas temperature
#INPUT: C_val - Measured value of parameter C
#INPUT: D_val - Measured value of parameter D
#INPUT: C_reglin - regresion line for C parameters for each concentration (lines defined for concentrations)
#INPUT: D_reglin - regresion line for D parameters for each concentration (lines defined for concentrations)
#INPUT: limit - stopping error value for Newton-Raphson algorithm

## OUTPUT: Value of CH4 (xresult) and N2 (yresult) - refers to first and second column in C_reglin and D_reglin matrixes

## [C_reglin] matrix - INPUT matrix for C parameter
## Columns are 
## CH4% / N2% / a1_C / b_C

## [D_reglin] matrix - INPUT matrix for D parameter
## Columns are 
## CH4% / N2% / a1_D / b_D

function [xresult, yresult] = Measurement(Ta, C_val, D_val, C_reglin, D_reglin, limit)
  
  ## Calculation C parameter values in each concentration in current Ta temperature
  for i = 1:rows(C_reglin)
    tx0(i,1:2) = C_reglin(i,1:2);
    tx0(i,3) = C_reglin(i,3)*Ta + C_reglin(i,4);
  endfor

  ## Calculation D parameter values in each concentrationin current Ta temperature
  for i = 1:rows(D_reglin)
    tx1(i,1:2) = D_reglin(i,1:2);
    tx1(i,3) = D_reglin(i,3)*Ta + D_reglin(i,4);
  endfor

  ## Calculation of surface parameters for the actual temperature

  ## Calculation of surface regresion of data in the first file
  Cs = Sur_reg(tx0);

  ## Calculation of surface regresion of data in the second file
  Ds = Sur_reg(tx1);

  ## Calculation of surface regresion of data in the first file
  Cr = Row2_reg(tx0);

  ## Calculation of surface regresion of data in the second file
  Dr = Row2_reg(tx1);

  ## First approximation of concentration values - using plane surface approximation
  [xi, yi] = Surfaces (C_val, D_val, Cs, Ds);

  ## Second, more precise approximation using paraboloid surface approximation and the results from plane approximation
  [xresult,yresult] = Newton_Raphson2 (xi, yi, C_val, D_val, Cr, Dr, limit);

endfunction