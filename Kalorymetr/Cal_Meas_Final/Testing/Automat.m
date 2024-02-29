## Copyright (C) 2019 ECieslar

## Author: ECieslar <ECieslar@MK-ECIESLAR>
## Created: 2019-04-23

#Writing directory
##cd D:\ECieslar\Documents\Temp;
## Calibration
# Clearing workspace
pkg load statistics
clearvars

## Printed to command line to show progress
progress = 0;

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
##out_file = fopen("01_calorific.csv", "w");
##out_file = fopen("C_approx.txt","w");
##out_file = fopen("D_approx.txt","w");
out_file = fopen("00_output_results.txt","w");
fprintf(out_file, "OCENA DANYCH POMIAROWYCH\n");  
fprintf(out_file, "(dok³adnoœæ regresji p³aszczyznowej)\n\n"); 

# Result values
limit = 0.00001;

# Calorific values
Gas_calor = [19.33, 11.03, 0];

############################################################################################################
#### Obliczenie wartoœci kalorycznoœci dla wszystkich punktów odchylenie i wartoœæ œrednia (plik z wynikami)
############################################################################################################
display("01 - Obliczenie wartoœci kalorycznoœci dla wszystkich punktów odchylenie i wartoœæ œrednia (plik z wynikami)");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "01 - Obliczenie wartoœci kalorycznoœci dla wszystkich punktów odchylenie i wartoœæ œrednia (plik z wynikami)\n"); 
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Podstawowy test  wartoœci oczekiwanej i odchylenia standardowego b³edu pomiaru kalorycznoœci\n");
fprintf(out_file, "## Dla wszystkich temperatur z zastosowaniem regresji liniowej do obliczenia wartoœci parametru C i D dla ka¿dego sk³adu\n");
fprintf(out_file, "## Wartoœæ oczekiwana powinna wynosiæ 0 (poprawnoœæ regresji), a odchylenie powinno byæ jak najmniejsze(minimalizacja b³edu pomiaru).\n\n");

## Calculating the calibration file (linear regresion for each concentration C=f(T) and D=g(T)
[Cmx, Dmx] = Calibration(Temperatures, Cmeas, Dmeas);

## Saving to the calibration file for C parameter
csvwrite("C2reglin.csv", Cmx);
C_reglin = csvread("C2reglin.csv");

## Saving to the calibration file for D parameter
csvwrite("D2reglin.csv", Dmx);
D_reglin = csvread("D2reglin.csv");


## Generating results for each cencentration
number = 1;
for j=1:columns(Temperatures)
  for i=1:rows(Cmeas)
     progress += 1
     Final(number,1) = Cmeas(i,1);
     Final(number,2) = Cmeas(i,2);
     Final(number,3) = Temperatures(j);
     [xresult, yresult] = Measurement_plain(Temperatures(j), Cmeas(i,2+j), Dmeas(i,2+j), C_reglin, D_reglin);   
     Final(number,4) = xresult;
     Final(number,5) = yresult; 
     Final(number,6) = 100 - xresult - yresult;
     Final(number,7) = ((100-Final(number,1)-Final(number,2))*Gas_calor(1) + Final(number,1)*Gas_calor(2))/100;
     Final(number,8) = ((100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2))/100;
     Final(number,9) = (Final(number,8) - Final(number,7))/Final(number,7); 
 
     [xresult, yresult] = Measurement_2row(Temperatures(j), Cmeas(i,2+j), Dmeas(i,2+j), C_reglin, D_reglin, limit);   
     Final(number,10) = xresult;
     Final(number,11) = yresult;
     Final(number,12) = 100 - xresult - yresult;
     Final(number,13) = ((100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2))/100;
     Final(number,14) = (Final(number,13) - Final(number,7))/Final(number,7); 
     
     number += 1;
  endfor
endfor
  
csvwrite("01_calorific.csv", Final);

## calculating the mean and standard deviations of regresion errors
  Par_Mean = mean(Final(:,9));
  Par_Relative = std(Final(:,9));
  fprintf(out_file, "Œrednia b³êdu wzglêdnego pomiaru kalorycznoœci (p³aszczyzna): %3.3f %% (~0%%)\n", Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego pomiaru kalorycznoœci (p³aszczyzna): %3.3f %%\n", Par_Relative*100);  
  
  Par_Mean = mean(Final(:,14));
  Par_Relative = std(Final(:,14));
  fprintf(out_file, "Œrednia b³êdu wzglêdnego pomiaru kalorycznoœci (paraboloida): %3.3f %%(~0%%)\n", Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego pomiaru kalorycznoœci (paraboloida): %3.3f %%\n\n", Par_Relative*100);   
  

############################################################################################################
#### Obliczenie wartoœci kalorycznoœci dla oddzielnie dla ka¿dej temperatury
############################################################################################################
display("02 - Obliczenie wartoœci odchyleñ parametrów kalorycznoœci dla oddzielnie dla ka¿dej temperatury");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "02 - Obliczenie wartoœci odchyleñ parametrów kalorycznoœci dla oddzielnie dla ka¿dej temperatury\n");  
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Pobobnie jak wy¿ej tylko oddzielnie dla ka¿dej temperatury - bez regresji liniowej\n");
fprintf(out_file, "## Wartoœæ oczekiwana powinna wynosiæ 0 (poprawnoœæ regresji), a odchylenie powinno byæ jak najmniejsze (minimalizacja b³edu pomiaru).\n");
fprintf(out_file, "## Wartoœci spodziewane podane w nawiasach\n\n");

clear Final;
for k = 1:columns(Temperatures)
  
  C_table = [Cmeas(:,1:2),Cmeas(:,k+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,k+2)];
  
  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Cc = Row2_reg(C_table);
  Dc = Row2_reg(D_table);

  ## Checking if the concentrations for C and D are the same

  for i=1:rows(C_table)

     progress += 1
     Final(i,1) = i;
     Final(i,2) = Temperatures(k);
     Final(i,3) = C_table(i,1);
     Final(i,4) = C_table(i,2);
     Final(i,5) = C_table(i,3);
     Final(i,6) = D_table(i,3);  
     
     Final(i,7) = Cs(1)*Final(i,3) +Cs(2)*Final(i,4) + Cs(3);
     Final(i,8) = Ds(1)*Final(i,3) +Ds(2)*Final(i,4) + Ds(3);
     Final(i,9) = (Final(i,7) - Final(i,5))/Final(i,5);
     Final(i,10) = (Final(i,8) - Final(i,6))/Final(i,6);
 
     [xresult,yresult] = Surfaces (C_table(i,3), D_table(i,3), Cs, Ds); 
     Final(i,11) = xresult;
     Final(i,12) = yresult;  
     Final(i,13) = 100 - xresult - yresult;
     Final(i,14) = ((100 - Final(i,3)- Final(i,4))*Gas_calor(1) + Final(i,3)*Gas_calor(2))/100;
     Final(i,15) = ((100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2))/100;
     Final(i,16) = (Final(i,15) - Final(i,14))/Final(i,14);      

     Final(i,17) = Cc(1)*Final(i,3)^2 + Cc(2)*Final(i,4)^2 + Cc(3)*Final(i,3)*Final(i,4) + Cc(4)*Final(i,3) + Cc(5)*Final(i,4) + Cc(6);
     Final(i,18) = Dc(1)*Final(i,3)^2 + Dc(2)*Final(i,4)^2 + Dc(3)*Final(i,3)*Final(i,4) + Dc(4)*Final(i,3) + Dc(5)*Final(i,4) + Dc(6);    
     [xresult,yresult] = Newton_Raphson2 (xresult, yresult, C_table(i,3), D_table(i,3), Cc, Dc, limit);
     Final(i,19) = (Final(i,17) - Final(i,5))/Final(i,5);
     Final(i,20) = (Final(i,18) - Final(i,6))/Final(i,6);
     Final(i,21) = xresult;
     Final(i,22) = yresult;
     Final(i,23) = 100 - xresult - yresult;
     Final(i,24) = ((100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2))/100;
     Final(i,25) = (Final(i,24) - Final(i,14))/Final(i,14);  
     
  endfor

## calculating the mean and standard deviations of regresion errors
  Par_Mean = mean(Final(:,9).*Final(:,5));
  Par_Relative = std(Final(:,9));
  fprintf(out_file, "Œrednia b³êdu bezwzglêdnego parametru C (p³aszczyzna) dla temperatury %i: %3.3f (=0.0)\n", Temperatures(k), Par_Mean);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego parametru C (p³aszczyzna) dla temperatury %i: %3.3f (<0.170)%%\n", Temperatures(k), Par_Relative*100);  
  
  Par_Mean = mean(Final(:,10).*Final(:,6));
  Par_Relative = std(Final(:,10));
  fprintf(out_file, "Œrednia b³êdu bezwzglêdnego parametru D (p³aszczyzna) dla temperatury %i: %3.3f (=0.0)\n", Temperatures(k), Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³êdu wzglêdnego parametru D (p³aszczyzna) dla temperatury %i: %3.3f (<0.150)%% \n\n", Temperatures(k), Par_Relative*100);  
  
  Par_Mean = mean(Final(:,19).*Final(:,5));
  Par_Relative = std(Final(:,19));
  fprintf(out_file, "Œrednia b³êdu bezwzglêdnego parametru C (paraboloida) dla temperatury %i: %3.3f (=0.0)\n", Temperatures(k), Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego parametru C (paraboloida) dla temperatury %i: %3.3f (<0.110)%% \n", Temperatures(k), Par_Relative*100);  
  
  Par_Mean = mean(Final(:,20).*Final(:,6));
  Par_Relative = std(Final(:,20));
  fprintf(out_file, "Œrednia b³êdu bezwzglêdnego parametru D (paraboloida) dla temperatury %i: %3.3f (=0.0)\n", Temperatures(k), Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³êdu wzglêdnego parametru D (paraboloida) dla temperatury %i: %3.3f (<0.050)%% \n\n\n", Temperatures(k), Par_Relative*100);  

  Par_Mean = mean(Final(:,16));
  Par_Relative = std(Final(:,16));
  fprintf(out_file, "Œrednia b³êdu wzglêdnego kalorycznosci (p³aszczyzna) dla temperatury %i (kalibracja i obliczanie): %3.3f (=0.0)\n", Temperatures(k), Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego kalorycznosci (p³aszczyzna) dla temperatury %i (kalibracja i obliczanie): %3.3f (<1.5)%% \n\n", Temperatures(k), Par_Relative*100);  
  
  Par_Mean = mean(Final(:,25));
  Par_Relative = std(Final(:,25));
  fprintf(out_file, "Œrednia b³êdu wzglêdnego kalorycznosci (paraboloida) dla temperatury %i (kalibracja i obliczanie): %3.3f (=0.0)\n", Temperatures(k), Par_Mean*100);  
  fprintf(out_file, "Odchylenie b³edu wzglêdnego kalorycznosci (paraboloida) dla temperatury %i (kalibracja i obliczanie): %3.3f (<1.5)%% \n\n\n", Temperatures(k), Par_Relative*100); 
  

  if (k == 1)
    Result = Final;
  else
    Result = [Result;Final];
  endif  

endfor

csvwrite("02_calorific_separate.csv", Result); 

clear Final;

## calculating the regresion coefficients for each conentracion and separately for C and D parameters
display("03 - Obliczenie wartoœci œredniej i odchylenia prostej regresji obliczanej dla poszczególnych sk³adów (przy zmianie temperatury)");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "03 - Obliczenie wartoœci œredniej i odchylenia prostej regresji obliczanej dla poszczególnych sk³adów (przy zmianie temperatury)\n");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Sprawdzenie na ile poprawne jest zastosowanie regresji liniowej do aproksymacji parametrów C i D w ró¿nych temp. dla tego samego sk³adu.\n");
fprintf(out_file, "## Wartoœci spodziewane podane w nawiasach\n\n");

 
Tnum = columns(Temperatures);
## calculation C parameter regresion parameters (a1, b) for each row of concentrations
for i = 1:rows(Cmeas)
    [AB(i,:)] = [transpose(Lin_T_reg(Temperatures, Cmeas(i,3:Tnum+2))),corr(Temperatures, Cmeas(i,3:Tnum+2))];
endfor

## adding the concentration columns and assigning to the matrix for C parameter
Cmx = [Cmeas(:,1:2), AB];

csvwrite("03_C_approx.csv", Cmx); 

## calculation mean and standard deviation of the korelation coefficient between temperature and C parameter values
Par_Mean = mean(Cmx(:,5));
Par_Relative = std(Cmx(:,5));
fprintf(out_file, "Œrednia wspó³czynnika korelacji C dla wszystkich temperatur: %3.5f (ok. -1)\n", Par_Mean);  
fprintf(out_file, "Odchylenie wspó³czynnika korelacji C dla wszystkich temperatur: %3.7f (ok. 0.005)\n\n\n", Par_Relative);  


## calculation D parameter regresion parameters (a1, b) for each row of concentrations
for i = 1:rows(Dmeas)
    [AB(i,:)] = [transpose(Lin_T_reg(Temperatures, Dmeas(i,3:Tnum+2))),corr(Temperatures, Dmeas(i,3:Tnum+2))];
endfor

## adding the concentration columns and assigning to the matrix  for D parameter
Dmx =[Dmeas(:,1:2), AB];
csvwrite("03_D_approx.csv", Dmx); 

## calculation mean and standard deviation of the korelation coefficient between temperature and D parameter values
Par_Mean = mean(Dmx(:,5));
Par_Relative = std(Dmx(:,5));
fprintf(out_file, "Œrednia wspó³czynnika korelacji D dla wszystkich temperatur: %3.5f (ok. 1)\n", Par_Mean);  
fprintf(out_file, "Odchylenie wspó³czynnika korelacji D dla wszystkich temperatur: %3.7f (ok. 0.001)\n\n\n", Par_Relative);  

## checking the influence on calorific value of constant errors added to C i D parameters 
display("04 - Obliczenie wp³ywu sta³ych b³êdów dodanych do parametrów C i D na wartoœci b³êdów kalorycznoœci");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "04 - Obliczenie wp³ywu sta³ych b³êdów dodanych do parametrów C i D na wartoœci b³êdów kalorycznoœci\n");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Sprawdzenie wp³ywu sta³ego b³êdu o okreœlonej w skrypcie wartoœci (0.1%%) na b³¹d pomiaru kalorycznoœci \n");
fprintf(out_file, "## Oblicznono dla 4 przypadków ró¿niacych siê znakiem b³êdu +/- C i +/- D \n");
fprintf(out_file, "## Im mniejsza waroœæ bezwzglêdna œredniej, minimalnej i maksymalnej tym lepiej. \n\n");

# constatnt error value 
error = 0.001;

error1 = [error, error, -error, -error];
error2 = [error, -error, error, -error];

## Checking if the concentrations for C and D are the same

clear Final_draw;
clear Final;
clear Result;

for m = 1:columns(Temperatures)
  C_table = [Cmeas(:,1:2),Cmeas(:,m+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,m+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Cc = Row2_reg(C_table);
  Dc = Row2_reg(D_table); 
  for k=1:columns(error1)
   for i=1:rows(C_table)
     progress += 1
     Final(i,1) = m;
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
  Final_temp(:,k) = Final(:,26).*100;


## calculating the mean and standard deviations of regresion errors
  Par_Relative = mean(Final(:,16));
  fprintf(out_file, "B³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja p³aszczyzn¹) T = %3.1f, C = %3.2f%%, D = %3.2f%%: œredni = %3.3f%%, maks = %3.3f%%, min = %3.3f%%\n", Temperatures(m), error1(k)*100, error2(k)*100, Par_Relative*100, max(Final(:,16))*100, min(Final(:,16))*100);  
   
  Par_Relative = mean(Final(:,26));
  fprintf(out_file, "B³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja paraboloid¹) T = %3.1f, C = %3.2f%%, D = %3.2f%%: œredni = %3.3f%%, maks = %3.3f%%, min = %3.3f%%\n\n", Temperatures(m), error1(k)*100, error2(k)*100, Par_Relative*100, max(Final(:,26))*100, min(Final(:,26))*100);  

  endfor 
  
  if (m == 1)
    Result = [Final(:,1:4), Final_temp(:,1:4)];
  else
    Result = [Result; Final(:,1:4), Final_temp(:,1:4)];
  endif  
  fprintf(out_file, "\n");
  
endfor
  
csvwrite("04_constant_error.csv", Result);  

## checking the influence on calorific value of random (normal) errors added to C i D parameters 
display("05 - Obliczenie wp³ywu losowych b³edów o rozk³adzie normalnym dodanych do parametrów C i D na wartoœci b³êdów kalorycznoœci");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "05 - Obliczenie wp³ywu losowych b³edów o rozk³adzie normalnym dodanych do parametrów C i D na wartoœci b³êdów kalorycznoœci\n");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Sprawdzenie wp³ywu sta³ego b³êdu o rozk³adzie normalnym o odchyleniu (0.1%%) \n");
fprintf(out_file, "## B³êdy dla parametrów C i D losowane niezale¿nie \n");
fprintf(out_file, "## Im mniejsza waroœæ bezwzglêdna œredniej, minimalnej i maksymalnej tym lepiej. \n\n");

# constatnt error value 
error = 0.001;
rand_num = 200;

## Checking if the concentrations for C and D are the same

clear Final_draw;
clear Final;
clear Result;

for m = 1:columns(Temperatures)

  C_table = [Cmeas(:,1:2),Cmeas(:,m+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,m+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Cc = Row2_reg(C_table);
  Dc = Row2_reg(D_table); 
  for k=1:rand_num
   progress += 1
   for i=1:rows(C_table)
     error1 = error*randn;
     error2 = error*randn;
     Final(i,1) = m;
     Final(i,2) = i;
     Final(i,3) = C_table(i,1);
     Final(i,4) = C_table(i,2);
     Final(i,5) = C_table(i,3);
     Final(i,6) = D_table(i,3);     
     Final(i,7) = Cs(1)*Final(i,3) +Cs(2)*Final(i,4) + Cs(3);
     Final(i,8) = Ds(1)*Final(i,3) +Ds(2)*Final(i,4) + Ds(3);
     
     Final(i,9) = Final(i,7)*(1+error1);
     Final(i,10) = Final(i,8)*(1+error2);
     [xresult,yresult] = Surfaces (Final(i,9), Final(i,10), Cs, Ds);
     Final(i,11) = xresult;
     Final(i,12) = yresult;  
     Final(i,13) = 100 - xresult - yresult;
     Final(i,14) = (100-Final(i,3)-Final(i,4))*Gas_calor(1) + Final(i,3)*Gas_calor(2);
     Final(i,15) = (100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2);
     Final(i,16) = (Final(i,15) - Final(i,14))/Final(i,14);
     
     Final(i,17) = Cc(1)*Final(i,3)^2 + Cc(2)*Final(i,4)^2 + Cc(3)*Final(i,3)*Final(i,4) + Cc(4)*Final(i,3) + Cc(5)*Final(i,4) + Cc(6);
     Final(i,18) = Dc(1)*Final(i,3)^2 + Dc(2)*Final(i,4)^2 + Dc(3)*Final(i,3)*Final(i,4) + Dc(4)*Final(i,3) + Dc(5)*Final(i,4) + Dc(6);    
     Final(i,19) = Final(i,17)*(1+error1);
     Final(i,20) = Final(i,18)*(1+error2);
     [xresult,yresult] = Newton_Raphson2 (xresult, yresult, Final(i,19), Final(i,20), Cc, Dc, limit);
     Final(i,21) = xresult;
     Final(i,22) = yresult;  
     Final(i,23) = 100 - xresult - yresult;
     Final(i,24) = (100-Final(i,3)-Final(i,4))*Gas_calor(1) + Final(i,3)*Gas_calor(2);
     Final(i,25) = (100 - xresult - yresult)*Gas_calor(1) + xresult*Gas_calor(2);
     Final(i,26) = (Final(i,25) - Final(i,24))/Final(i,24);

   endfor

   Final_temp1(:,k) = Final(:,16).*100;
   Final_temp2(:,k) = Final(:,26).*100;  

  endfor 


  ## calculating the mean and standard deviations of regresion errors
  std1 = 0; mean1 = 0;
  for n=1:rows(C_table)
    std1 += (std(Final_temp1(n,:)))^2; 
    mean1 += mean(Final_temp1(n,:)); 
  endfor
  std1 = sqrt(std1/rows(C_table));
  mean1 = mean1/rows(C_table);
  max1 = max(max(Final_temp1)); 
  min1 = min(min(Final_temp1));
  fprintf(out_file, "B³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja p³aszczyzn¹) T = %3.1f: odchylenie = %3.3f%%, œredni = %3.3f%%, maks = %3.3f%%, min = %3.3f%%\n", Temperatures(m), std1, mean1, max1, min1);  
  std1 = 0; mean1 = 0;
  for n=1:rows(C_table)
    std1 += (std(Final_temp2(n,:)))^2; 
    mean1 += mean(Final_temp2(n,:)); 
  endfor
  std1 = sqrt(std1/rows(C_table));
  mean1 = mean1/rows(C_table);
  max1 = max(max(Final_temp2)); 
  min1 = min(min(Final_temp2)); 
  fprintf(out_file, "B³¹d wzglêdny wyznacznia kalorycznoœci (aproksymacja paraboloid¹) T = %3.1f: odchylenie = %3.3f%%, œredni = %3.3f%%, maks = %3.3f%%, min = %3.3f%%\n", Temperatures(m), std1, mean1, max1, min1);  
  
  if (m == 1)
    Result1 = Final_temp1;
    Result2 = Final_temp2;
  else
    Result1 = [Result1; Final_temp1];
    Result2 = [Result2; Final_temp2];
  endif  
  fprintf(out_file, "\n");
  
endfor
  
csvwrite("05_norm_distr_error_plain.csv", Result1);
csvwrite("05_norm_distr_error_2row.csv", Result2);

## checking the checking the normality of residual errors for C i D parameters 
display("06 - Sprawdzenie normalnoœci resztkowych b³edów regresji dla parametrów C i D za pomoc¹ testu Chi2");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "06 - Sprawdzenie normalnoœci resztkowych b³edów regresji dla parametrów C i D za pomoc¹ testu Chi2\n");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## Zastosowanie testu Chi2 do weryfikacji normalnoœci rozk³adu b³êdów resztkowych regresji powierzchniowej \n");
fprintf(out_file, "## Dodatkowo sprawdzono wspó³czynnik skoœnoœci i wspó³czynnik p³askoœci (kurtozê), \n");
fprintf(out_file, "## oraz sprawdzono za pomoca testu Z ile potencjalnych obserwacji odstaj¹cych (outlier)\n");
fprintf(out_file, "## Im mniejsza waroœæ statystyki Chi2 i iloœæ pomiarów odstaj¹cych tym lepiej\n");
fprintf(out_file, "## Waroœci wspó³czynników skoœnoœci i p³askoœci powinny byæ jak najbli¿ej 0.\n\n");

clear Final;

for k=1:columns(Temperatures)
  
    C_table = [Cmeas(:,1:2),Cmeas(:,k+2)];
    D_table = [Dmeas(:,1:2),Dmeas(:,k+2)];

    Cs = Sur_reg(C_table);
    Ds = Sur_reg(D_table);

    Cc = Row2_reg(C_table);
    Dc = Row2_reg(D_table);

## Checking if the concentrations for C and D are the same

  for i=1:rows(C_table)
     progress += 1    
     Final(i,1) = k;
     Final(i,2) = i;
     Final(i,3) = C_table(i,1);
     Final(i,4) = C_table(i,2);
     Final(i,5) = C_table(i,3);
     Final(i,6) = D_table(i,3);     
     Final(i,7) = Cs(1)*Final(i,3) +Cs(2)*Final(i,4) + Cs(3);
     Final(i,8) = Ds(1)*Final(i,3) +Ds(2)*Final(i,4) + Ds(3);
     
     Final(i,9) = Final(i,5) - Final(i,7);
     Final(i,10) = Final(i,6) - Final(i,8);
     
     Final(i,11) = Cc(1)*Final(i,3)^2 + Cc(2)*Final(i,4)^2 + Cc(3)*Final(i,3)*Final(i,4) + Cc(4)*Final(i,3) + Cc(5)*Final(i,4) + Cc(6);
     Final(i,12) = Dc(1)*Final(i,3)^2 + Dc(2)*Final(i,4)^2 + Dc(3)*Final(i,3)*Final(i,4) + Dc(4)*Final(i,3) + Dc(5)*Final(i,4) + Dc(6);      

     Final(i,13) = Final(i,5) - Final(i,11);
     Final(i,14) = Final(i,6) - Final(i,12);    
  endfor
 

 fprintf(out_file, "Dla temperatury %d \n", Temperatures(k));

 ## Key parameters of Chi2 test 
  alfa = 0.05;  # statistical significance
  bins = 10;    # number of bins in empirical histogram - test Chi2 works well if bins have enough elements
  lim_sig = 3;  # number of sigmas from the mean that are used in the histogram (+/-3 sigma is typical

 ## Clearing variable in case the nest run of the script would be for different sizes of vectors
  clear Xns;
  clear Xvect;  

 
  meanX = mean(Final(:,9));
  stdX = std(Final(:,9));
  skewX = skewness(Final(:,9));
  kurtX = kurtosis(Final(:,9),1) - kurtosis(Final(:,9),0);  
  num = rows(Final(:,9));
  
# Conting number of measurement errors ouside +/- 3 sigma range
  ztest = 0;
  for i=1:num
    zX(i) = (Final(i,9) - meanX)/stdX;
    if (zX(i) > 3) 
      ztest += 1;
    endif
  endfor  

 ## Calculation of Chi2 statistics for C parameter in case of plain approximation   
  binX = 2*lim_sig*stdX/bins;
  Xvect(1) = meanX - lim_sig*stdX;

  for i =2:(bins+1)
    Xvect(i) = Xvect(1) + binX*(i-1);
  endfor
  Xvect;
  Xns = histc(Final(:,9),Xvect);

  Chi = 0;
  for i =1:(bins-1)
    Chi += (Xns(i) - num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)))^2/...
           (num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)));
  endfor

  Chi_crit = chi2inv (1-alfa, bins-3);

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif

  fprintf(out_file, "Wartoœci statystyki Chi2 %3.3f (Chi kryt = %3.3f) \n", Chi, Chi_crit);
  fprintf(out_file, "Czy rozk³ad normalny dla C (p³aszczyzna) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  fprintf(out_file, "Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  fprintf(out_file, "Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  fprintf(out_file, "Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest); 

  clear Xns;
  clear Xvect;

  meanX = mean(Final(:,10));;
  stdX = std(Final(:,10));
  skewX = skewness(Final(:,10));
  kurtX = kurtosis(Final(:,10),1) - kurtosis(Final(:,10),0);  
  num = rows(Final(:,10));

# Conting number of measurement errors ouside +/- 3 sigma range
  ztest = 0;
  for i=1:num
    zX(i) = (Final(i,10) - meanX)/stdX;
    if (zX(i) > 3) 
      ztest += 1;
    endif
  endfor  

 ## Calculation of Chi2 statistics for D parameter in case of plain approximation   
  binX = 2*lim_sig*stdX/bins;
  Xvect(1) = meanX - lim_sig*stdX;

  for i =2:(bins+1)
    Xvect(i) = Xvect(1) + binX*(i-1);
  endfor
  Xvect;
  Xns = histc(Final(:,10),Xvect);

  Chi = 0;

  for i =1:(bins-1)
    Chi += (Xns(i) - num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)))^2/...
           (num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)));
  endfor

  Chi_crit = chi2inv (1-alfa, bins-3);

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif

  fprintf(out_file, "Wartoœci statystyki Chi2 %3.3f (Chi kryt = %3.3f) \n", Chi, Chi_crit);
  fprintf(out_file, "Czy rozk³ad normalny dla D (p³aszczyzna) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  fprintf(out_file, "Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  fprintf(out_file, "Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  fprintf(out_file, "Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest); 
  
  clear Xns;
  clear Xvect;

  meanX = mean(Final(:,13));
  stdX = std(Final(:,13));
  skewX = skewness(Final(:,13));
  kurtX = kurtosis(Final(:,13),1) - kurtosis(Final(:,13),0);  
  num = rows(Final(:,13));

# Conting number of measurement errors ouside +/- 3 sigma range
  ztest = 0;
  for i=1:num
    zX(i) = (Final(i,13) - meanX)/stdX;
    if (zX(i) > 3) 
      ztest += 1;
    endif
  endfor  

## Calculation of Chi2 statistics for D parameter in case of plain approximation  
  binX = 2*lim_sig*stdX/bins;
  Xvect(1) = meanX - lim_sig*stdX;

  for i =2:(bins+1)
    Xvect(i) = Xvect(1) + binX*(i-1);
  endfor
  Xvect;
  Xns = histc(Final(:,13),Xvect);

  Chi = 0;
  for i =1:(bins-1)
    Chi += (Xns(i) - num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)))^2/...
           (num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)));
  endfor

  Chi_crit = chi2inv (1-alfa, bins-3);

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif

  fprintf(out_file, "Wartoœci statystyki Chi2 %3.3f (Chi kryt = %3.3f) \n", Chi, Chi_crit);
  fprintf(out_file, "Czy rozk³ad normalny dla C (paraboloida) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  fprintf(out_file, "Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  fprintf(out_file, "Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  fprintf(out_file, "Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest); 

 ## Calculation of Chi2 statistics for D parameter in case of pparaboloid approximation  
  clear Xns;
  clear Xvect;

  meanX = mean(Final(:,14));
  stdX = std(Final(:,14));
  skewX = skewness(Final(:,14));
  kurtX = kurtosis(Final(:,14),1) - kurtosis(Final(:,14),0);  
  num = rows(Final(:,14));

# Conting number of measurement errors ouside +/- 3 sigma range
  ztest = 0;
  for i=1:num
    zX(i) = (Final(i,14) - meanX)/stdX;
    if (zX(i) > 3) 
      ztest += 1;
    endif
  endfor  

## Calculation of Chi2 statistics for D parameter in case of plain approximation  
  binX = 2*lim_sig*stdX/bins;
  Xvect(1) = meanX - lim_sig*stdX;

  for i =2:(bins+1)
    Xvect(i) = Xvect(1) + binX*(i-1);
  endfor
  Xvect;
  Xns = histc(Final(:,14),Xvect);

  Chi = 0;

  for i =1:(bins-1)
    Chi += (Xns(i) - num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)))^2/...
           (num*(stdnormal_cdf((Xvect(i+1)-meanX)/stdX) - stdnormal_cdf((Xvect(i)-meanX)/stdX)));
  endfor

  Chi_crit = chi2inv (1-alfa, bins-3);

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif

  fprintf(out_file, "Wartoœci statystyki Chi2 %3.3f (Chi kryt = %3.3f) \n", Chi, Chi_crit);
  fprintf(out_file, "Czy rozk³ad normalny dla D (paraboloida) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  fprintf(out_file, "Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  fprintf(out_file, "Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  fprintf(out_file, "Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest); 
  
endfor

## checking the checking the normality of residual errors for C i D parameters 
display("07 - Wyznaczenie k¹ta miêdzy prostymi uzyskanymi dla sta³ej wartoœci C i D (regresja p³aszczyznowa)");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "07 - Wyznaczenie k¹ta miêdzy prostymi uzyskanymi dla sta³ej wartoœci C i D (regresja p³aszczyznowa)\n");
fprintf(out_file, "###############################################################################################################################################\n");
fprintf(out_file, "## K¹t miedzy prostymi i p³aszczyznami okreœla niezale¿noœæ parametrów C i D.\n");
fprintf(out_file, "## Im wiêksza wartoœæ bezwzglêdna tym lepiej.\n\n");

for k=1:columns(Temperatures)
  progress += 1
  fprintf(out_file, "Dla temperatury %d \n", Temperatures(k));

  C_table = [Cmeas(:,1:2),Cmeas(:,k+2)];
  D_table = [Dmeas(:,1:2),Dmeas(:,k+2)];

  Cs = Sur_reg(C_table);
  Ds = Sur_reg(D_table);

  Csa = Cs.*(100/Cs(3));
  Dsa = Ds.*(100/Ds(3));

## Surfaces  
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

  fprintf(out_file, "COS k¹ta nachylenia p³aszczyzn wzglêdem siebie: %3.5f\n", cos_fi); 
  fprintf(out_file, "K¹t nachylenia p³aszczyzn wzglêdem siebie: %3.3f\n", _fi); 

## Lines
  A1 = A1;
  B1 = B1;
  C1 = 0;

  A2 = A2;
  B2 = B2;
  C2 = 0;

  tan_fi_plain = (A1*B2-A2*B1)/(A1*A2+B1*B2);
  _fi_plain = atand(tan_fi_plain);

  fprintf(out_file, "TAN k¹ta nachylenia prostych dla konkretnych wartosci parametru wzglêdem siebie: %3.5f\n", tan_fi_plain); 
  fprintf(out_file, "K¹t nachylenia prostych dla konkretnych wartosci parametru wzglêdem siebie: %3.3f\n\n", _fi_plain); 

endfor
  
fclose("all");