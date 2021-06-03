% Altitude Compensation
sea_level_difference = linspace(0,316);

comp_value = 8192 * (1 + (sea_level_difference*0.14)/100);


figure(1); clf; hold all; ...
    plot(sea_level_difference,comp_value);


