    constants;
    noTrueDetect=0;
    noFalseDetect=0;
    disp('Checking Faces...');
    out=sim(net, trainDB(:,1:TRAINING_FRACTION_FACES*TRAINING_SET));
    for j=1:TRAINING_FRACTION_FACES*TRAINING_SET
        if (out(1,j)>VALIDATION_THRESHOLD)
            noTrueDetect=noTrueDetect+1;
        else
            noFalseDetect=noFalseDetect+1;
        end
    end
    disp(sprintf('True Face Detections=%f',100*noTrueDetect/(TRAINING_FRACTION_FACES*TRAINING_SET)));
    disp(sprintf('False Face Detections=%f',100*noFalseDetect/(TRAINING_FRACTION_FACES*TRAINING_SET)));
    noTrueDetect=0;
    noFalseDetect=0;
    disp('Checking Non-Faces...');
    out=sim(net,trainDB(:,TRAINING_FRACTION_FACES*TRAINING_SET + 1:TRAINING_SET));
    for j=1:(1-TRAINING_FRACTION_FACES) * TRAINING_SET
        if (out(1,j)<VALIDATION_THRESHOLD)
            noTrueDetect=noTrueDetect+1;
        else
            noFalseDetect=noFalseDetect+1;
        end
    end
    
    disp(sprintf('True Non-Face Detections=%f',100*noTrueDetect/((1-TRAINING_FRACTION_FACES) * TRAINING_SET)));
    disp(sprintf('False Non-Face Detections=%f',100*noFalseDetect/((1-TRAINING_FRACTION_FACES) * TRAINING_SET)));
            