## Surface regresion of data from the imput matrixes
## INPUT: Arg[:,1], Arg[:,2] - x, y - argumrets, Arg[:,3] - z - function value
## OUTPUT: val - matrix containing the coefficients of the surface regresion

function val = Sur_reg (Arg)
  xi = 0; yi = 0; zi = 0; xiyi = 0; xi2 = 0; yi2 = 0; xizi = 0; yizi = 0;
  for i = 1:rows(Arg)
    xi = xi + Arg(i,1);
    yi = yi + Arg(i,2);
    xiyi = xiyi + Arg(i,1)*Arg(i,2);
    xi2 = xi2 + Arg(i,1)^2;
    yi2 = yi2 + Arg(i,2)^2; 
    zi = zi + Arg(i,3);
    xizi = xizi + Arg(i,1)*Arg(i,3);   
    yizi = yizi + Arg(i,2)*Arg(i,3);  
  endfor
  
  A = [xi2, xiyi, xi; xiyi, yi2, yi; xi, yi, rows(Arg)];
  b = [xizi; yizi; zi];
  val = A \ b;
  
endfunction

