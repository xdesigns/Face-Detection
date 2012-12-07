function [layer1 layer2]=saveNetwork(net,file1,file2);
    constants;
    b=net.b;
    bias=zeros(27,1);
    for i=1:27
        if (~isempty(net.b{i,1}))
            bias(i,1)=b{i,1};
        end
    end     
   
    layer1=zeros(26,401);
    if (NETWORK_TYPE==1)
        count=1;
        %-------------------extracting 5x5 weights---------------------------------------%
        for i=1:5:16
            for j=1:5:16
                layer1(count,1:400)=extractWeights(layer1(count,1:400),j,i,j+4,i+4,CROPPED_WIDTH,CROPPED_HEIGHT,net.IW{count,count});
                count=count+1;
            end
        end
        %-------------------extracting 10x10 weights---------------------------------------%
        for i=1:10:11
            for j=1:10:11
                layer1(count,:)=extractWeights(layer1(count,:),j,i,j+9,i+9,CROPPED_WIDTH,CROPPED_HEIGHT,net.IW{count,count});
                count=count+1;
            end
        end
        %-------------------extracting 5x20 weights---------------------------------------%
        for i=1:3:16
            layer1(count,:)=extractWeights(layer1(count,:),1,i,CROPPED_WIDTH,i+4,CROPPED_WIDTH,CROPPED_HEIGHT,net.IW{count,count});
            count=count+1;
        end
    else
        for i=1:26
            for j=1:400
                if (~isempty(net.IW{i,j}))
                    layer1(i,j)=net.IW{i,j};
                end
            end
        end
    end
    
    layer1(:,401)=bias(1:26,1);
    save (file1,'layer1','-ascii');
    
    
    
    layer2=zeros(1,27);
    for i=1:26
        if (~isempty(net.LW{end,i}))
            layer2(1,i)=net.LW{end,i};
        else
            layer2(1,i)=0;
        end
    end
    layer2(1,27)=bias(27,1);
    
    save (file2,'layer2','-ascii');
end
            
            