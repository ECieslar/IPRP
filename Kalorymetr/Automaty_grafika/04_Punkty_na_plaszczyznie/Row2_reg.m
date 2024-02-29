## Surface regresion of data from the imput matrixes
## INPUT: Arg[:,1], Arg[:,2] - x, y - argumrets, Arg[:,3] - z - function value
## OUTPUT: val - matrix containing the coefficients of the paraboloid regresion 

function val = Row2_reg (Arg)
  ## Iniciating variables
  xi = 0; yi = 0; 
  xiyi = 0; xi2 = 0; yi2 = 0; 
  xiyi2 = 0; xi2yi = 0; xi3 = 0; yi3 = 0; 
  xiyi3 = 0; xi2yi2 = 0; xi3yi = 0; xi4 = 0; yi4 = 0; 
  zi = 0; xizi = 0; yizi = 0;
  xiyizi = 0; xi2zi = 0; yi2zi = 0;
  ## Calculating A and b matrix elements
  for i = 1:rows(Arg)
    xi = xi + Arg(i,1);
    yi = yi + Arg(i,2);
    xiyi = xiyi + Arg(i,1)*Arg(i,2);
    xi2 = xi2 + Arg(i,1)^2;
    yi2 = yi2 + Arg(i,2)^2; 
    xiyi2 = xiyi2 + Arg(i,1)*Arg(i,2)^2;
    xi2yi = xi2yi + Arg(i,1)^2*Arg(i,2);
    xi3 = xi3 + Arg(i,1)^3;
    yi3 = yi3 + Arg(i,2)^3;
    xiyi3 = xiyi3 + Arg(i,1)*Arg(i,2)^3;
    xi2yi2 = xi2yi2 + Arg(i,1)^2*Arg(i,2)^2;
    xi3yi = xi3yi + Arg(i,1)^3*Arg(i,2); 
    xi4 = xi4 + Arg(i,1)^4;
    yi4 = yi4 + Arg(i,2)^4;
    zi = zi + Arg(i,3);
    xizi = xizi + Arg(i,1)*Arg(i,3);   
    yizi = yizi + Arg(i,2)*Arg(i,3);
    xiyizi = xiyizi + Arg(i,1)*Arg(i,2)*Arg(i,3);
    xi2zi = xi2zi + Arg(i,1)^2*Arg(i,3);
    yi2zi = yi2zi + Arg(i,2)^2*Arg(i,3); 
  endfor
  
  ## Entering calculated values into the matrixes A and b
  A = [xi4,     xi2yi2,   xi3yi,  xi3,    xi2yi,  xi2; ...
       xi2yi2,  yi4,      xiyi3,  xiyi2,  yi3,    yi2; ...
       xi3yi,   xiyi3,    xi2yi2, xi2yi,  xiyi2,  xiyi; ...
       xi3,     xiyi2,    xi2yi,  xi2,    xiyi,   xi; ...
       xi2yi,   yi3,      xiyi2,  xiyi,   yi2,    yi; ...
       xi2,     yi2,      xiyi,   xi,     yi,     rows(Arg)];
  b = [xi2zi; yi2zi; xiyizi; xizi; yizi; zi];
  
  ## Solving the linear equation
  val = A \ b;
  
endfunction

