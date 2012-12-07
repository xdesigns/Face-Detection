noFiles=0;
for k=1:MAX_BOOTSTRAP_FILES
    bootStrap;
    noFiles=noFiles+noNonFacesAvail-prevAvail;
    if (noFiles>=MIN_BOOTSTRAP_WINDOWS & k>=MIN_BOOTSTRAP_FILES)
        disp('Minimum Windows Condition met...');
        break;
    end
end