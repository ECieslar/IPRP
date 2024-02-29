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


# Output files
out_file = fopen("00_output_results.txt","w");
fprintf(out_file, "NACHYLENIE P£ASZCZYZN APROKSYMACJI PARAMETRÓW C i D WZGLÊDEM SIEBIE\n\n");  

# Result values
limit = 0.00001;
error = 0.001;

# Calculation of angles
for k=1:columns(Temperatures)
  printf("\nDla temperatury: T=%d\n",Temperatures(k));
  fprintf(out_file,"\nDla temperatury: T=%d\n",Temperatures(k));
  C_table = [Cmeas(:,1:2),Cmeas(:,k+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,k+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  # Surfaces
  Csa = Cs.*(100/Cs(3));
  Dsa = Ds.*(100/Ds(3));

  A1 = Csa(1);
  B1 = Csa(2);
  C1 = -1;
  D1 = Csa(3);

  A2 = Dsa(1);
  B2 = Dsa(2);
  C2 = -1;
  D2 = Dsa(3);

  cos_fi = (A1*A2 + B1*B2 + C1*C2)/(sqrt(A1^2 +B1^2 +C1^2)*sqrt(A2^2 +B2^2 +C2^2));
  _fi = acosd(cos_fi);
  cos_fi
  _fi
  fprintf(out_file, "COS k¹ta nahylenia p³aszczyzn wzglêdem siebie: %3.5f\n", cos_fi); 
  fprintf(out_file, "K¹t nahylenia p³aszczyzn wzglêdem siebie: %3.3f\n", _fi); 

  hf = figure;
  x = linspace (0, 100, 51)';
  y = linspace (0, 50, 26)';
  [xx, yy] = meshgrid (x, y);
  z = (100/D1)*(A1*xx + B1*yy + D1);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("C");
  view([1 -1 1]);
  hold on

  z = (100/D2)*(A2*xx + B2*yy + D2);

  mesh (xx, yy, z);
  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("C/D");
  view([1 -1 1]);
  hold off

  ## Lines
  A1 = A1;
  B1 = B1;
  C1 = 0;

  A2 = A2;
  B2 = B2;
  C2 = 0;

  tan_fi_plain = (A1*B2-A2*B1)/(A1*A2+B1*B2);
  _fi_plain = atand(tan_fi_plain);
  tan_fi_plain
  _fi_plain

  fprintf(out_file, "TAN k¹ta nahylenia prostych dla konkretnych wartosci parametru wzglêdem siebie: %3.5f\n", tan_fi_plain); 
  fprintf(out_file, "K¹t nahylenia prostych dla konkretnych wartosci parametru wzglêdem siebie: %3.3f\n", _fi_plain); 

  hf = figure;
  x = 0:1:100;
  y= x.*(-A1/B1);
  plot(x,y);
  hold on

  y= x.*(-A2/B2);
  plot(x,y);
  xlabel ("CH4");
  ylabel ("N2");
  hold off

endfor  
  
fclose("all");