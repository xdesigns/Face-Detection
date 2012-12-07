    constants;
    noTrueDetect=0;
    noFalseDetect=0;
    disp('Checking Faces...');
    out=sim(net,validation.P(:,1:NO_VALIDATION_FACES));
    for j=1:NO_VALIDATION_FACES
        if (out(1,j)>VALIDATION_THRESHOLD)
            noTrueDetect=noTrueDetect+1;
        else
            noFalseDetect=noFalseDetect+1;
        end
    end
    disp(sprintf('True Face Detections=%f',100*noTrueDetect/NO_VALIDATION_FACES));
    disp(sprintf('False Face Detections=%f',100*noFalseDetect/NO_VALIDATION_FACES));
    noTrueDetect=0;
    noFalseDetect=0;
    disp('Checking Non-Faces...');
    out=sim(net,validation.P(:,NO_VALIDATION_FACES+1:NO_VALIDATION_FACES+NO_VALIDATION_NON_FACES));
    for j=1:NO_VALIDATION_NON_FACES
        if (out(1,j)<VALIDATION_THRESHOLD)
            noTrueDetect=noTrueDetect+1;
        else
            noFalseDetect=noFalseDetect+1;
        end
    end
    
    disp(sprintf('True Non-Face Detections=%f',100*noTrueDetect/NO_VALIDATION_NON_FACES));
    disp(sprintf('False Non-Face Detections=%f',100*noFalseDetect/NO_VALIDATION_NON_FACES));
            