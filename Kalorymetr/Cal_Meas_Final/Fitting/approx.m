function val = approx (An, t)
  val = An(3) - An(3)/power(1 + power(t/An(2),An(1)), An(4));
endfunction
