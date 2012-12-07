%Defining network architecture and training functions
    disp('Creating Network architecture...');
    net=network;
    net.numInputs=400;
    net.numLayers=27;
    net.biasConnect=[ones(27,1)];

    disp('Initialising Input Connections...');
    count=1;
    net.inputConnect=zeros(27,400);
    
    %---------------------------------------------5x5 inputs Specification---------------------------------------------------------%
    for i=1:5:16
        for j=1:5:16
            net.inputConnect(count,:)=extractWeights(net.inputConnect(count,:),j,i,j+4,i+4,CROPPED_WIDTH,CROPPED_HEIGHT,[]);
            count=count+1;
        end
    end
    
    %---------------------------------------------10x10 inputs Specification---------------------------------------------------------%
    for i=1:10:11
        for j=1:10:11
            net.inputConnect(count,:)=extractWeights(net.inputConnect(count,:),j,i,j+9,i+9,CROPPED_WIDTH,CROPPED_HEIGHT,[]);
            count=count+1;
        end
    end
    
    %---------------------------------------------5x20 inputs Specification---------------------------------------------------------%
    for i=1:3:16
        net.inputConnect(count,:)=extractWeights(net.inputConnect(count,:),1,i,20,i+4,CROPPED_WIDTH,CROPPED_HEIGHT,[]);
        count=count+1;
    end
    

    net.layerConnect=[zeros(26,27);ones(1,26) 0];
    net.outputConnect=[zeros(1,26) 1];
    net.targetConnect=[zeros(1,26) 1];
    
    disp('Initialising Input Ranges...');
    for i=1:400
        i
        disp(sprintf('Setting Input Range For Input %d',i));
        net.inputs{i}.range=[0 255];
    end
    
    disp('Initialising Layer and Bias Parameters...');
    for i=1:27
        net.layers{i}.size=1;
        net.layers{i}.transferFcn = 'tansig';
        net.layers{i}.initFcn = 'initnw';
        net.biases{i}.initFcn='initnw';
       % net.biases{i}.learnFcn='learngdm';
       % if (i<=26)
        %    net.layerWeights{27,i}.learnFcn='learngdm';
         %   net.layerWeights{27,i}.initFcn='initnw';
        %end
    end
    
    %disp('Initialising Input Parameters...');
    
    %for i=1:27
     %   for j=1:400
      %      if (~isempty(net.inputWeights{i,j}))
       %         net.inputWeights{i,j}.initFcn='initnw';
        %        net.inputWeights{i,j}.learnFcn='learngdm';
         %   end
%        end
 %   end
    
    