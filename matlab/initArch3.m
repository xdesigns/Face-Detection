%Defining network architecture and training functions
    num_hidden = 10;
    disp('Creating Network architecture...');
    net=network;
    net.numInputs=400;
    net.numLayers=num_hidden+1;
    net.biasConnect=[ones(net.numLayers,1)];

    disp('Initialising Input Connections...');
    net.inputConnect=zeros(num_hidden+1,400);
    
    for i=1:1:num_hidden
            net.inputConnect(i,:)=ones (1,400);
    end
    
    net.layerConnect=[zeros(num_hidden,num_hidden+1);ones(1,num_hidden) 0];
    net.outputConnect=[zeros(1,num_hidden) 1];
    net.targetConnect=[zeros(1,num_hidden) 1];
    
    disp('Initialising Input Ranges...');
    for i=1:400
        i
        disp(sprintf('Setting Input Range For Input %d',i));
        net.inputs{i}.range=[0 255];
    end
    
    disp('Initialising Layer and Bias Parameters...');
    for i=1:num_hidden+1
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
    
    