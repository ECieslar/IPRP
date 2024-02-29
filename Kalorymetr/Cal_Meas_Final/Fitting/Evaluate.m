## Reading the measured values file
tx0 = csvread("wzor_15-29-02_128mean.csv");
idata = tx0(251:1250,:);
minimum = min(idata(:,2));

for i = 1:rows(idata)
  pdata(i,1) = idata(i,1) - idata(1,1);
  pdata(i,2) = idata(i,2) - minimum;
endfor

#calculation of least square regresion of data in the first file
## Initial value of coeficients matrix to calculate difference
Init = [1; 1; 45000; 1];

## Limiting value of the approximation
limit = 1.0e-15

Axyz = fitting4(pdata, Init, limit);

#printing the formula for the result surface regresion of the first file
Axyz

## Test in case C and G have some error
Aijk = fitting2(pdata, [Axyz(2);Axyz(3)+0.01], [Axyz(1);Axyz(4)-0.01], limit);
Aijk

csvwrite("approx_exp.csv", Axyz);