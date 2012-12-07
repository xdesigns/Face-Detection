    constants;
    maskGenCorr;
    mask=uint8(mask);
%Initializing Network Functions
    net.initFcn='initlay';
    net.performFcn='msereg';
    net.trainFcn='traingdx';
    net=init(net);
    
%Initializing Face Database
    disp('Initializing Face Database...');
    totalNoFaceFiles=sum(NO_FACE_FILES');
    facesDB=cell(totalNoFaceFiles,2);
    for i=1:NO_FOLDERS
        for j=1:NO_FACE_FILES(i)
            file=sprintf('%04d.jpg',j);
            facesDB{sum(NO_FACE_FILES(1,1:(i-1))')+j,1}=[FACE_DIR{i} file];
            facesDB{sum(NO_FACE_FILES(1,1:(i-1))')+j,2}=0;
        end
    end

%Initializing Scenery Database
    disp('Initializing Scenery Database...');
    sceneDB=cell(NO_SCENE_FILES,2);
    for i=1:NO_SCENE_FILES
        file=sprintf('%03d.jpg',i);
        sceneDB{i,1}=[SCENE_DIR{1} file];    
        sceneDB{i,2}=0;
    end
 
%Defining Network Training Parameters
    disp('Defining Network Training Parameters...');
    net.trainParam.epochs=NETWORK_EPOCHS;
    net.trainParam.show=NETWORK_SHOW;
    net.trainParam.goal=NETWORK_GOAL;
    net.trainParam.lr=NETWORK_LR;
    net.trainParam.max_fail=NETWORK_MAX_FAIL;
    net.trainParam.lr_dec=NETWORK_LR_DEC;
    net.trainParam.lr_inc=NETWORK_LR_INC;
    net.trainParam.max_perf_inc=NETWORK_MAX_PERF_INC;
    net.trainParam.mc=NETWORK_MC;
    net.trainParam.min_grad=NETWORK_MIN_GRAD;
    net.trainParam.time=NETWORK_TIME;
    
    net.performParam.ratio=NETWORK_RATIO;
    

 %Initializing Training Set
    noNonFacesAvail=0;
    cnt=0;
  