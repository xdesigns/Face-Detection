constants
data = [];
count = 0;
for i=-1:0.02:1
    count=count+1;
    VALIDATION_THRESHOLD=i;
    noTrueDetect=0;
    noFalseDetect=0;
    disp(sprintf('Checking Faces...threshold %i',i));
    out=sim(net,validation.P(:,1:NO_VALIDATION_FACES));
    for j=1:NO_VALIDATION_FACES
        if (out(1,j)>VALIDATION_THRESHOLD)
            noTrueDetect=noTrueDetect+1;
        end
    end
    disp(sprintf('True Face Detections=%f',100*noTrueDetect/NO_VALIDATION_FACES));
  
    data(count,1) = noTrueDetect/NO_VALIDATION_FACES;
    
    disp('Checking Non-Faces...');
    out=sim(net,validation.P(:,NO_VALIDATION_FACES+1:NO_VALIDATION_FACES+NO_VALIDATION_NON_FACES));
    for j=1:NO_VALIDATION_NON_FACES
        if (out(1,j)>=VALIDATION_THRESHOLD)
            noFalseDetect=noFalseDetect+1;
        end
    end
    disp(sprintf('False Non-Face Detections=%f',100*noFalseDetect/NO_VALIDATION_NON_FACES));
    
    data(count,2) = noFalseDetect/(NO_VALIDATION_FACES + NO_VALIDATION_NON_FACES);
end
            