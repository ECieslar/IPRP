## Copyright (C) 2019 ECieslar

# Reading C and D matrix files
##########################################################################
##                               IMPORTANT                              ##
## Data in the input files
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

pkg load statistics;

# Result values
limit = 0.0000001;

clear Final_draw;

for k=1:columns(Temperatures)
  
    C_table = [Cmeas(:,1:2),Cmeas(:,k+2)];
    D_table = [Dmeas(:,1:2),Dmeas(:,k+2)];

    Cs = Sur_reg(C_table);
    Ds = Sur_reg(D_table);

    Cc = Row2_reg(C_table);
    Dc = Row2_reg(D_table);

## Checking if the concentrations for C and D are the same

  for i=1:rows(C_table)
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

  if (k==1) 
    Final_draw = Final;
  else
    Final_draw = [Final_draw;Final];
  endif
 
  h1 = figure;
  hist(Final(:,9));
  colormap (summer ()); 
  xlabel ("Error");
  ylabel ("Empirical probability");
  title ("Histogram of proksimation C parameter error - plain surface");

  h1 = figure;
  hist(Final(:,10));
  colormap (autumn ()); 
  xlabel ("Error");
  ylabel ("Empirical probability");
  title ("Histogram of proksimation D parameter error - plain surface");

  h1 = figure;
  hist(Final(:,13));
  colormap (winter ()); 
  xlabel ("Error");
  ylabel ("Empirical probability");
  title ("Histogram of proksimation C parameter error - paraboloid surface");

  h1 = figure;
  hist(Final(:,14));
  colormap (spring ()); 
  xlabel ("Error");
  ylabel ("Empirical probability");
  title ("Histogram of proksimation D parameter error - paraboloid surface");
    
  printf("##############################################################################\n");
  printf("Dla temperatury %d \n", Temperatures(k));
  printf("##############################################################################\n\n");

  clear Xns;
  clear Xvect;

  alfa = 0.05
  bins = 10;
  lim_sig = 3;

# Basic probability parametres  
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

# Chi2 test of normality  
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

  Chi
  Chi_crit = chi2inv (1-alfa, bins-3)

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif
  printf("Czy rozk³ad normalny dla C (p³aszczyzna) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  printf("Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  printf("Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  printf("Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest);  
  
  clear Xns;
  clear Xvect;

# Basic probability parametres    
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

# Chi2 test of normality 
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

  Chi
  Chi_crit = chi2inv (1-alfa, bins-3)

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif
  printf("Czy rozk³ad normalny dla D (p³aszczyzna) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  printf("Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  printf("Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  printf("Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest);  
  
  clear Xns;
  clear Xvect;

# Basic probability parametres    
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

# Chi2 test of normality  
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

  Chi
  Chi_crit = chi2inv (1-alfa, bins-3)

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif
  printf("Czy rozk³ad normalny dla C (paraboloida) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  printf("Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  printf("Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  printf("Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest);  
  
  clear Xns;
  clear Xvect;

# Basic probability parametres    
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

# Chi2 test of normality  
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

  Chi
  Chi_crit = chi2inv (1-alfa, bins-3)

  if Chi < Chi_crit 
    s = "TAK";
  else
    s = "NIE";
  endif
  printf("Czy rozk³ad normalny dla D (paraboloida) przy z poziomem istotnoœci %1.3f przy %d przedzia³ach: %s\n", alfa, bins, s);
  printf("Wspó³czynnik skoœnoœci (poprawna wartoœæ ~0): %1.3f \n", skewX);
  printf("Wspó³czynnik p³askoœci - kurtoza (poprawna wartoœæ ~0): %1.3f\n", kurtX);  
  printf("Liczba b³êdów poza obszaerem +/-3 sigma: %d \n\n", ztest);  
  
endfor

csvwrite("Final.csv", Final_draw); 

fclose("all");