#The files contain the values of the chosen parameter for each concentration
#The concentrations and the results are placed in the separate raws
#The names of each raws are plased in variables sxy below
#It is only necessary to change values between the lines below to calculate the results

#reading the data files
function Calibration(tmp)
  # Reading data files
  Ccal = csvread("cdata.csv");
  Dcal = csvread("ddata.csv");
  
  tx0tmp = [Ccal(:,1:2),Ccal(:,tmp+2)];
  tx1tmp = [Dcal(:,1:2),Dcal(:,tmp+2)];

  #calculation of surface regresion of data in the first file
  Asur = Sur_reg(tx0tmp);

  #printing the formula for the result surface regresion of the first file
  printf ("Surface regresion: %s = (%d)*x +(%d)*y + (%d) \n", "Surface D", Asur(1), Asur(2), Asur(3)); 
  A1s = Asur(1)
  B1s = Asur(2)
  C1s = Asur(3)

  csvwrite("d_sur_coef.csv", Asur);

  #calculation of surface regresion of data in the first file
  Asur = Sur_reg(tx1tmp);

  #printing the formula for the result surface regresion of the first file
  printf ("Surface regresion: %s = (%d)*x +(%d)*y + (%d) \n", "Surface C", Asur(1), Asur(2), Asur(3)); 
  A2s = Asur(1)
  B2s = Asur(2)
  C2s = Asur(3)

  csvwrite("c_sur_coef.csv", Asur);

  #calculation of surface regresion of data in the first file
  Axyz = Row2_reg(tx0tmp);

  #printing the formula for the result surface regresion of the first file
  printf ("Surface regresion: %s = (%d)*x^2 +(%d)*y^2 +(%d)*x*y +(%d)*x + (%d)*y + (%d) \n", "2 row D", Axyz(1), Axyz(2), Axyz(3), Axyz(4), Axyz(5), Axyz(6)); 
  A1 = Axyz(1)
  B1 = Axyz(2)
  C1 = Axyz(3)
  D1 = Axyz(4)
  E1 = Axyz(5)
  F1 = Axyz(6)

  csvwrite("d_2row_coef.csv", Axyz);

  #calculation of surface regresion of data in the second file
  Axyz = Row2_reg(tx1tmp);

  #printing the formula for the result surface regresion of the first file
  printf ("Surface regresion: %s = (%d)*x^2 +(%d)*y^2 +(%d)*x*y +(%d)*x + (%d)*y + (%d) \n", "2 row C", Axyz(1), Axyz(2), Axyz(3), Axyz(4), Axyz(5), Axyz(6)); 
  A2 = Axyz(1)
  B2 = Axyz(2)
  C2 = Axyz(3)
  D2 = Axyz(4)
  E2 = Axyz(5)
  F2 = Axyz(6)

  csvwrite("c_2row_coef.csv", Axyz);


  fclose("all");
endfunction

