## Function to solve linear sets of equations describing the surface of parameters
## INPUT: C, D - measured values of the parameters
## INPUT: Cval, Dval - coefficients describing paraboloid surface of the parameters

## OUTPUT: x, y - calculated values of concentrations

function [x,y] = Surfaces (C, D, Cval, Dval)
  
## Setting the matrixes
  A = [Cval(1), Cval(2); Dval(1), Dval(2)];
  b = [C-Cval(3); D-Dval(3)];

## Solving the set of equations
  Res = A \ b;
  
## Assigning the results
  x = Res(1);
  y = Res(2);

endfunction
