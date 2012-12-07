%Defining network architecture and training functions
    constants;
    disp('Creating Network architecture...');
    net=network;
    net.numInputs=26;
    net.numLayers=27;
    net.biasConnect=[ones(27,1)];
    net.inputConnect=[eye(26,26);zeros(1,26)];
    net.layerConnect=[zeros(26,27);ones(1,26) 0];
    net.outputConnect=[zeros(1,26) 1];
    net.targetConnect=[zeros(1,26) 1];
    %------------Initialising 5x5 input ranges-----------------%
    for i=1:16
        net.inputs{i}.range=[zeros(25,1) 255*ones(25,1)];
    end
     %------------Initialising 10x10 input ranges-----------------%
    for i=17:20
        net.inputs{i}.range=[zeros(100,1) 255*ones(100,1)];
    end
    %------------Initialising 5x20 input ranges-----------------%
    for i=21:26
        net.inputs{i}.range=[zeros(100,1) 255*ones(100,1)];
    end

    for i=1:27
        net.layers{i}.size=1;
        net.layers{i}.transferFcn = 'tansig';
        net.layers{i}.initFcn = 'initnw';
    end
