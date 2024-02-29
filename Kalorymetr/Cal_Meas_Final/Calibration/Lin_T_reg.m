## Linear regresion of the data from input matrixes
## INPUT: X(i) - random values of argument X , Y(i) - random values of function Y
## OUTPUT: [coef] = [a1; b] matrix is a result - it contains y=a1*x+b coefficients

function coef = Lin_T_reg (X,Y)
  xi = 0; xi2 = 0; yi = 0; xiyi = 0; 

## Preparation of [A]  and  [b] matrixes values
  for i = 1:columns(X)
    xi = xi + X(i);
    xi2 = xi2 + X(i)^2;
    yi = yi + Y(i);
    xiyi = xiyi + X(i)*Y(i);
  endfor

## Preparation of [A]  and  [b] matrixes sets
  A = [xi2, xi; xi, columns(X)];
  b = [xiyi; yi];
  
## Calculation of linear equation system [A] * [coef] = [b] to get [coef] matrix    
  coef = A \ b;  
endfunction

