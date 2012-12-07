%constants;
trainDB=[];
targetOutput=[];
disp(sprintf('Initializing Face Set with %i images...',TRAINING_FRACTION_FACES*TRAINING_SET));
for i=1:TRAINING_FRACTION_FACES*TRAINING_SET
    faceSelected=randint(1,1,totalNoFaceFiles)+1;
    facesDB{faceSelected,2}=facesDB{faceSelected,2}+1;
    img=imread(facesDB{faceSelected,1});
    img=double(img);
    trainDB=[trainDB extractData(img)];   
    targetOutput=[targetOutput 1];  
end

if (noNonFacesAvail<=TRAINING_SET*TRAINING_FRACTION_NON_FACES)
    noNonFaces=noNonFacesAvail;
else
    noNonFaces=TRAINING_SET*TRAINING_FRACTION_NON_FACES;
end

noScenery=(1-TRAINING_FRACTION_FACES)*TRAINING_SET-noNonFaces;
disp(sprintf('Initializing Scenery Set with %i images...',noScenery));
for i=TRAINING_FRACTION_FACES*TRAINING_SET+1:TRAINING_FRACTION_FACES*TRAINING_SET+noScenery
    scenerySelected=randint(1,1,NO_SCENE_FILES)+1;
    sceneDB{scenerySelected,2}=sceneDB{scenerySelected,2}+1;
    img=sceneryWindowExtract(sceneDB{scenerySelected,1});
    trainDB=[trainDB extractData(img)];   
    targetOutput=[targetOutput -1];  
end    

disp(sprintf('Initializing BootStrap Set with %i images...',noNonFaces));
for i=TRAINING_FRACTION_FACES*TRAINING_SET+noScenery+1:TRAINING_FRACTION_FACES*TRAINING_SET+noScenery+noNonFaces
    bootstrapSelected=randint(1,1,noNonFacesAvail)+1;
    img=imread([NON_FACE_DIR{1} sprintf('%i.jpg',bootstrapSelected)]);
    img=double(img);
    trainDB=[trainDB extractData(img)];   
    targetOutput=[targetOutput -1];  
end   
inputPerm=randperm(TRAINING_SET);
trainDB=trainDB(:,inputPerm);
targetOutput=targetOutput(:,inputPerm);

%Commencing Training Process
netTemp=net;
disp('Commencing Training Process...');
if (DO_VALIDATION)
    minErr=1;
    prevErr=1;
    fails=0;
    err=0;
    for i=1:NO_VALIDATION_CHECKS
        flag=1;
        [net2 tr Y E]=train(netTemp,trainDB,targetOutput);
        %msError=msereg(E,net2,net2.performParam);
        msError=sse(E,net2);
        if (msError<NETWORK_GOAL)
            disp(sprintf('Performance Goal Met...MSE(REG) %f',msError));
            netTemp=net2;
            break;
        end
        err=0;
        out=sim(net2,validation.P(:,1:NO_VALIDATION_FACES));
        for j=1:NO_VALIDATION_FACES
            err=err+(((1-out(1,j))^2)/4);
        end
        
        out=sim(net2,validation.P(:,NO_VALIDATION_FACES+1:NO_VALIDATION_FACES+NO_VALIDATION_NON_FACES));
        
        for j=1:NO_VALIDATION_NON_FACES
            err=err+(((1+out(1,j))^2)/4);
        end
        
        err=err/(NO_VALIDATION_FACES+NO_VALIDATION_NON_FACES);
        if (err<=prevErr)
            fails=0;
        else
            fails=fails+1;
            if (fails>NETWORK_MAX_FAIL)
                flag=0;
                disp('Fail limit exceeded');
                break;
            end
        end
        if (err<minErr)
            minErr=err;
        end
        if (err<=minErr+VALIDATION_MARGIN)
            netTemp=net2;
            disp(sprintf('---Check %i | MSE(REG) %f | Err Margin %f | Fails %f | Min Err %f | Err %f',i,msError,VALIDATION_MARGIN,fails,minErr,err));
            prevErr=err;
        else
            disp(sprintf('Validation Error - Error Margin %f Min Error=%f Error=%f',VALIDATION_MARGIN,minErr,err));
            break;
        end
    end
    if (err<=minErr+VALIDATION_MARGIN & flag==1)
        disp('Global error minimized..Saving changes');
        net=netTemp;
        iter=iter+1;
        bootStrapIterate;
    else
        disp('Possible case of overfitting...Discarding Training');
    end    
else
    net=train(net,trainDB,targetOutput);
    iter=iter+1;
    bootStrapIterate;
end


