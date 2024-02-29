## Copyright (C) 2019 ECieslar

# Reading C and D matrix files
##########################################################################
##                               IMPORTANT                              ##
## Date in the input files
## Decimal separator ".", Number separator ","
## In cdata and ddata number of rows have to be the same
## Numebr of columns in Temperatures should be the same 
## as columns with parameter data in cdata i ddata (here 4)
## Order of columns:
## CH4, N2, CT1, CT2, CT3, CT4 ...
Cmeas = csvread("cdata.csv");
## CH4, N2, DT1, DT2, DT3, DT4 ...
Dmeas = csvread("ddata.csv");
## T1, T2, T3, T4 ...
Temperatures = csvread("temperatures.csv");
###########################################################################

for m=1:columns(Temperatures)
  C_table = [Cmeas(:,1:2),Cmeas(:,m+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,m+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Cc = Row2_reg(C_table);
  Dc = Row2_reg(D_table);

  #ploting the surface of the D file 
  hf = figure;
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);
  z = Ds(1)*xx + Ds(2)*yy + Ds(3);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("D");
  view([1 -1 1]);
  hold on
  scatter3 (Dmeas(:,1), Dmeas(:,2), Dmeas(:,m+2),40, "r");
  title ("D surface regresion");
  hold off

  #ploting the surface of the C file 
  hf = figure; "b", "filled"
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);
  z = Cs(1)*xx + Cs(2)*yy + Cs(3);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("C");
  view([1 -1 1]);
  hold on
  scatter3 (Cmeas(:,1), Cmeas(:,2), Cmeas(:,m+2),40, "r");
  title ("C surface regresion");
  hold off

  #ploting the regresion 2 row of the D file 
  hf = figure;
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);
  z = Dc(1)*(xx.^2) + Dc(2)*(yy.^2) + Dc(3)*xx.*yy + Dc(4)*xx + Dc(5)*yy + Dc(6);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("D");
  view([1 -1 1]);
  hold on
  scatter3 (Dmeas(:,1), Dmeas(:,2), Dmeas(:,m+2),40, "r");
  title ("D paraboloidal regresion");
  hold off

  #ploting the regresion 2 row of the C file 
  hf = figure;
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);
  z = Cc(1)*(xx.^2) + Cc(2)*(yy.^2) + Cc(3)*xx.*yy + Cc(4)*xx + Cc(5)*yy + Cc(6);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("C");
  view([1 -1 1]);
  hold on
  scatter3 (Cmeas(:,1), Cmeas(:,2), Cmeas(:,m+2),40, "r");
  title ("C paraboloidal regresion");
  hold off

  #ploting the regresion 2 row of the D file 
  hf = figure;
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);

  z = Ds(1)*xx + Ds(2)*yy + Ds(3);
  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("D");
  view([1 -1 1]);
  hold on

  z = Dc(1)*(xx.^2) + Dc(2)*(yy.^2) + Dc(3)*xx.*yy + Dc(4)*xx + Dc(5)*yy + Dc(6);
  mesh (xx, yy, z);

  scatter3 (Dmeas(:,1), Dmeas(:,2), Dmeas(:,m+2), 60, "r");
  title ("D paraboloid and surface regresion");
  hold off

  #ploting the regresion 2 row of the C file 
  hf = figure;
  x = linspace (0, 110, 45)';
  y = linspace (-10, 50, 25)';
  [xx, yy] = meshgrid (x, y);

  z = Cs(1)*xx + Cs(2)*yy + Cs(3);
  mesh (xx, yy, z);
  view([1 -1 1]);
  hold on

  z = Cc(1)*(xx.^2) + Cc(2)*(yy.^2) + Cc(3)*xx.*yy + Cc(4)*xx + Cc(5)*yy + Cc(6);
  mesh (xx, yy, z);

  scatter3 (Cmeas(:,1), Cmeas(:,2), Cmeas(:,m+2), 60, "r");
  title ("C paraboloid and surface regresion");
  hold off

endfor  
  
fclose("all");