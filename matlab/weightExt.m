    weights = [];
    count = 1;
    for i=1:5:16
        for j=1:5:16
            indi = (i-1)/5 + 1;
            indj = (j-1)/5 + 1;
            weights{indi,indj}=zeros(5,5);
            for k=1:5
                for l=1:5
                    weights{indi,indj}(k,l)=cell2mat(net.IW(count,20*(i+k-2)+j+l-1));
                end
            end
            count=count+1;
        end
    end
    
    data = zeros(4,4);
    for i=1:4
        for j=1:4
            data(i,j)=sum(sum(cell2mat(weights(i,j)).*cell2mat(weights(i,j))));
        end
    end
        