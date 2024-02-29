## Function to solve nonlinear sets of equations describing the surfaces of parameters
## INPUT: x0, y0 - values near to the expected result to ensure the stability of the algorithm
## INPUT: C, D - measured values of the parameters
## INPUT: Cc, Dc - coefficients describing paraboloid surface of the parameters
## INPUT: limit - stopping error value for Newton-Raphson algorithm

## OUTPUT: x, y - calculated values of concentrations

function [x,y] = Newton_Raphson2 (x0, y0, C, D, Cc, Dc, limit)
## Init
 xi = x0; yi = y0; protect = 100; 
 
while(1)
## Calculating the function values
 C_temp = Cc(1)*xi^2 + Cc(2)*yi^2 + Cc(3)*xi*yi + Cc(4)*xi + Cc(5)*yi + Cc(6) - C;
 D_temp = Dc(1)*xi^2 + Dc(2)*yi^2 + Dc(3)*xi*yi + Dc(4)*xi + Dc(5)*yi + Dc(6) - D;

## Calculating the Jacobian values
 C_Jx_temp = 2*Cc(1)*xi + Cc(3)*yi + Cc(4);
 C_Jy_temp = 2*Cc(2)*yi + Cc(3)*xi + Cc(5);
 D_Jx_temp = 2*Dc(1)*xi + Dc(3)*yi + Dc(4);
 D_Jy_temp = 2*Dc(2)*yi + Dc(3)*xi + Dc(5); 

## Creating the matrixes 
 Fxy = [C_temp; D_temp];
 Jxy = [C_Jx_temp, C_Jy_temp; D_Jx_temp, D_Jy_temp];
 Zxy = - (Jxy \ Fxy);
  
## Calculating the measure of result precision
 mZxy = sqrt(Zxy(1)^2 + Zxy(2)^2);

## Checking achieved precision 
 if (mZxy <= limit)
    x = xi;
    y = yi;
    break;
 else
    xi = xi + Zxy(1);
    yi = yi + Zxy(2);
    ## To protect hanging of the algorithm - maximum number of approximations
    protect = protect - 1;
    if (protect == 0)
      x = 0;
      y = 0;
      break;
    endif
 endif 
 
endwhile
 
endfunction
