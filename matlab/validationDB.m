%Initialising Validation Database
    constants;
    disp('Initializing Validation Database...');
    disp(sprintf('No Validation Faces = %i',NO_VALIDATION_FACES));
    disp(sprintf('No Validation Non-Faces = %i',NO_VALIDATION_NON_FACES));
    validation.P=[];
    validation.T=[];
    for i=1:NO_VALIDATION_FACES
        if (RANDOMIZE_VALIDATION)
            faceSelected=randint(1,1,totalNoFaceFiles)+1;
        else
            faceSelected=i;
        end
        disp(sprintf('---Adding Face %i = %s',i,facesDB{faceSelected,1}));
        img=imread(facesDB{faceSelected,1});
        img=double(img);
        validation.P=[validation.P extractData(img)];   
        validation.T=[validation.T 1];  
    end
    
    for i=1:NO_VALIDATION_NON_FACES
        scenerySelected=randint(1,1,NO_SCENE_FILES)+1;
        disp(sprintf('---Adding Non-Face %i= %s',i,sceneDB{scenerySelected}));
        img=sceneryWindowExtract(sceneDB{scenerySelected,1});
        img=double(img);
        validation.P=[validation.P extractData(img)];   
        validation.T=[validation.T -1];  
    end
    
    save ('validation/validationDB','validation');