function A=extractWeights(A,x1,y1,x2,y2,width,height,weights)
    count=1;
    for i=y1:y2
        for j=x1:x2
            if (isempty(weights))
                A((i-1)*width+j)=1;
            else
                A((i-1)*width+j)=weights(1,count);
                count=count+1;
            end
        end
    end
end