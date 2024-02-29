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


# Reading C and D matrix files
Cmeas = csvread("cdata.csv");
Dmeas = csvread("ddata.csv");
Temperatures = csvread("temperatures.csv");

# Output files
out_file = fopen("00_output_results.txt","w");
fprintf(out_file, "WP£YW B£EDÓW POMIARU C i D NA KALORYCZNOŒÆ\n\n");  

# Result values
limit = 0.0000001;
k = 1;
error = 0.001;
error1 = [error, error, -error, -error];
error2 = [error, -error, error, -error];

# Calorific values
Gas_calor = [19.33, 11.03, 0];

for m=1:columns(Temperatures)
  fprintf(out_file, "\nDla temperatury: T=%d \n", Temperatures(m));  
  C_table = [Cmeas(:,1:2),Cmeas(:,m+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,m+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Cc = Row2_reg(C_table);
  Dc = Row2_reg(D_table);

  ## Checking if the concentrations for C and D are the same

  clear Final_draw;

  for k=1:columns(error1)
    for i=1:rows(C_table)
       i
       Final(i,1) = k;
       Final(i,2) = i;
       Final(i,3) = C_table(i,1);
       Final(i,4) = C_table(i,2);
       Final(i,5) = C_table(i,3);
       Final(i,6) = D_table(i,3);     
       Final(i,7) = Cs(1)*Final(i,3) +Cs(2)*Final(i,4) + Cs(3);
       Final(i,8) = Ds(1)*Final(i,3) +Ds(2)*Final(i,4) + Ds(3);
       
       Final(i,9) = Final(i,7)*(1+error1(k));
       Final(i,10) = Final(i,8)*(1+error2(k));
       [xresult,yresult] = Surfaces (Final(i,9), Final(i,10), Cs, Ds);
       Final(i,11) = xresult;
       Final(i,12) = yresult;  
       Final(i,13) = 100 - xresult - yresult;
       Final(i,14) = (100-Final(i,3)-Final(i,4))*Gas_calor(1) + Final(i,3)*Gas_calor(2);
       Final(i,15) = (100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2);
       Final(i,16) = (Final(i,15) - Final(i,14))/Final(i,14);
       
       Final(i,17) = Cc(1)*Final(i,3)^2 + Cc(2)*Final(i,4)^2 + Cc(3)*Final(i,3)*Final(i,4) + Cc(4)*Final(i,3) + Cc(5)*Final(i,4) + Cc(6);
       Final(i,18) = Dc(1)*Final(i,3)^2 + Dc(2)*Final(i,4)^2 + Dc(3)*Final(i,3)*Final(i,4) + Dc(4)*Final(i,3) + Dc(5)*Final(i,4) + Dc(6);    
       Final(i,19) = Final(i,17)*(1+error1(k));
       Final(i,20) = Final(i,18)*(1+error2(k));
       [xresult,yresult] = Newton_Raphson2 (xresult, yresult, Final(i,19), Final(i,20), Cc, Dc, limit);
       Final(i,21) = xresult;
       Final(i,22) = yresult;  
       Final(i,23) = 100 - xresult - yresult;
       Final(i,24) = (100-Final(i,3)-Final(i,4))*Gas_calor(1) + Final(i,3)*Gas_calor(2);
       Final(i,25) = (100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2);
       Final(i,26) = (Final(i,25) - Final(i,24))/Final(i,24);

    endfor
    Final_draw(:,k) = Final(:,26).*100;


  ## calculating maximum and of regresion errors
    Par_Relative = max(Final(:,16));
    Par_Relative1 = min(Final(:,16));
    if (abs(Par_Relative) < abs(Par_Relative1)) Par_Relative = Par_Relative1;
    endif

    fprintf(out_file, "Maksymalny b³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja p³aszczyzn¹): C=%3.3f, D=%3.3f: %3.3f %%\n", error1(k), error2(k), Par_Relative*100);  
    Par_Relative = mean(Final(:,16));
    fprintf(out_file, "Œredni b³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja p³aszczyzn¹): C=%3.3f, D=%3.3f: %3.3f %%\n", error1(k), error2(k), Par_Relative*100);  
    
    Par_Relative = max(Final(:,26));
    Par_Relative1 = min(Final(:,26));
    if (abs(Par_Relative) < abs(Par_Relative1)) Par_Relative = Par_Relative1;
    endif
    fprintf(out_file, "Maksymalny b³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja paraboloid¹): C=%3.3f, D=%3.3f: %3.3f %%\n", error1(k), error2(k), Par_Relative*100);  

    Par_Relative = mean(Final(:,16));
    fprintf(out_file, "Œredni b³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja p³aszczyzn¹): C=%3.3f, D=%3.3f: %3.3f %%\n", error1(k), error2(k), Par_Relative*100);  

    
  endfor  

## Drawing the 3D display of error values  - separate drawing for each temperature
  csvwrite("Final.csv", Final_draw);  

  Final_draw = [C_table(:,1:2),Final_draw];

  hf = figure;
  colors = ["r", "g", "b", "y"];
    
  for k=1:(columns(Final_draw)-2)

    scatter3 (Final_draw(:,1), Final_draw(:,2), Final_draw(:,k+2),80, colors(k), "filled");
    hold on

  endfor

  xlabel ("CH4");
  ylabel ("N2");
  zlabel ("Error");
  legend("Error C+,D+ - RED", "Error C+,D- - GREEN", "Error C-,D+ - BLUE", "Error C-,D- - YELLOW");
  title ("Calorific value errors in % (for +/-0,1% C,D parameter errors)");
  hold off
  
endfor
  
fclose("all");