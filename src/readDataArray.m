function [timestamp, temp, hmdty, co2, tvoc] = readDataArray(file)
    % open file pointer
    fd = fopen(file, 'r');
    % set data shape
    data_shape = [5 Inf];
    % scan all lines possible (there's other stuff at the end of the file)
    [data, n] = fscanf(fd, "%d,%f,%f,%d,%d\n", data_shape);
    fprintf("%d data lines parsed.\n", n);
    % close file pointer
    fclose(fd);
    timestamp = data(1,:);
    temp = data(2,:);
    hmdty = data(3,:);
    co2 = data(4,:);
    tvoc = data(5,:);
end
% written by Jakob Weiß

% example: [wavelength, baseline, coefficient] = readDataArray_BioPhys("Gruppe1_messung1_1zu3.csv")
