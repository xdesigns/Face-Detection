trainDB=[];
targetOutput=[];
disp(sprintf('Initializing Face Set with %i images...',TRAINING_FRACTION_FACES*TRAINING_SET));
for i=1:TRAINING_FRACTION_FACES*TRAINING_SET
    faceSelected=randint(1,1,totalNoFaceFiles)+1;
    facesDB{faceSelected,2}=facesDB{faceSelected,2}+1;
    img=imread(facesDB{faceSelected,1});
    img=uint8(img);
    img = bitand(img, mask);
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
    img=uint8(img);
    img = bitand(img, mask);
    img=double(img);
    trainDB=[trainDB extractData(img)];   
    targetOutput=[targetOutput -1];  
end    

disp(sprintf('Initializing BootStrap Set with %i images...',noNonFaces));
for i=TRAINING_FRACTION_FACES*TRAINING_SET+noScenery+1:TRAINING_FRACTION_FACES*TRAINING_SET+noScenery+noNonFaces
    bootstrapSelected=randint(1,1,noNonFacesAvail)+1;
    img=imread([NON_FACE_DIR{1} sprintf('%i.jpg',bootstrapSelected)]);
    img=uint8(img);
    img = bitand(img, mask);
    img=double(img);
    trainDB=[trainDB extractData(img)];   
    targetOutput=[targetOutput -1];  
end   
%inputPerm=randperm(TRAINING_SET);
%trainDB=trainDB(:,inputPerm);
%targetOutput=targetOutput(:,inputPerm);

%Commencing Training Process
disp('Commencing Training Process...');
if (DO_VALIDATION)
    net=train(net,trainDB,targetOutput,[],[],validation);
else
    net=train(net,trainDB,targetOutput);
end

iter=iter+1;
if (mod(iter,5)==0)
    noNonFacesAvail=0;
end
bootStrapIterate;


