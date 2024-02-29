## The least square approximation of the multiparametric (2 parameters), one variable function using Gauss-Newton Method
## INPUT: Arg - results of the measurements (time domain readings of measurement voltage), 
##        Ax - rough approximation of expected coefficients C and D
##        Ai - average of the B & G parameter
##        limit - precision parameter for the algorithm
## OUTPUT: val - matrix containing the coefficients of the time domain regresion

function val = fitting2 (Arg, Ax, Ai, limit)
## Maximum number of approximations
protect = 50;
## Initial value of delta for each parameter of the function
delta = 0.000001*Ax;

while(1)

    ## Calculation of residuals
    for i = 1:rows(Arg)
      r(i) = Arg(i,2) - approx([Ai(1);Ax(1);Ax(2);Ai(2)], Arg(i,1));
    endfor
    
    ## Calculation of Jacobian function specific matrix and its transpose
    for i = 1:rows(Arg)
      ## Derivatives of approx(An,t) function
      
      Jf(i,1) = -(approx([Ai(1);Ax(1)+delta(1);Ax(2);Ai(2)],Arg(i,1)) - approx([Ai(1);Ax(1)-delta(1);Ax(2);Ai(2)],Arg(i,1)))/(2*delta(1));
      Jf(i,2) = -(approx([Ai(1);Ax(1);Ax(2)+delta(2);Ai(2)],Arg(i,1)) - approx([Ai(1);Ax(1);Ax(2)-delta(2);Ai(2)],Arg(i,1)))/(2*delta(2));
      
      Jft(1,i) = Jf(i,1);
      Jft(2,i) = Jf(i,2);
    endfor

    ## Calculation of new coefficients matrix  
    Ax_new = Ax - inv(Jft*Jf)*Jft*transpose(r);
    
    ## Calculation of approximation evaluation quality parameter
    Ax_eval = 0;
    temp = Ax_new .- Ax;
    for i = 1:columns(temp)
      Ax_eval = Ax_eval + temp(i)^2;
    endfor
    #Ax_eval

    if (Ax_eval <= limit)
    ## Assigning the final value of function parameters
      val = Ax_new;
      break;
    else
    ## Assigning values of parameters in the next approximation cycle
      Ax = Ax_new
      for i = 1:rows(Ax)
        if abs(Ax(i)) > 0.001 
          delta(i) = 0.00001*Ax(i);
        else
          delta(i) = 0.00001;
        endif
      endfor
      ## Calculating number of approximations
      ## to protect hanging of the algorithm - maximum number of approximations
      protect = protect - 1;
      if (protect == 0)
        val = zeros(columns(Ax));
        break;
      endif
   endif 
endwhile
  
  
endfunction

