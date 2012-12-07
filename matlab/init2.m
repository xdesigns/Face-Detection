    constants;
%Initializing Network Functions
    net.initFcn='initlay';
    net.performFcn='msereg';
    net.trainFcn='trainscg';
    net=init(net);
    
%Defining Network Training Parameters
    disp('Defining Network Training Parameters...');
    net.trainParam.epochs=NETWORK_EPOCHS;
    net.trainParam.show=NETWORK_SHOW;
    net.trainParam.goal=NETWORK_GOAL;
    net.trainParam.lr=NETWORK_LR;
    net.trainParam.max_fail=NETWORK_MAX_FAIL;
  %  net.trainParam.lr_dec=NETWORK_LR_DEC;
  %  net.trainParam.lr_inc=NETWORK_LR_INC;
  %  net.trainParam.max_perf_inc=NETWORK_MAX_PERF_INC;
  %  net.trainParam.mc=NETWORK_MC;
    net.trainParam.min_grad=NETWORK_MIN_GRAD;
    net.trainParam.time=NETWORK_TIME;
    
    net.performParam.testratio=NETWORK_RATIO;
    

 %Initializing Training Set
    noNonFacesAvail=0;
    cnt=0;
  