function Regresion(Tx, C_data, D_data)
  
  ## Calculation of surface parameters for temperature column

  ## Calculation of surface regresion of data in the first file
  Cs = Sur_reg([C_data(:,1:2),C_data(:,Tx+2)]);
  csvwrite("c_sur_coef.csv", Cs);

  ## Calculation of surface regresion of data in the second file
  Ds = Sur_reg([D_data(:,1:2),D_data(:,Tx+2)]);
  csvwrite("d_sur_coef.csv", Ds);

  ## Calculation of surface regresion of data in the first file
  Cr = Row2_reg([C_data(:,1:2),C_data(:,Tx+2)]);
  csvwrite("c_2row_coef.csv", Cr);
  
  ## Calculation of surface regresion of data in the second file
  Dr = Row2_reg([D_data(:,1:2),D_data(:,Tx+2)]);
  csvwrite("d_2row_coef.csv", Dr);
  
  fclose("all");
endfunction