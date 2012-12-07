function data=extractData(img)
    constants;
    data=[];
    if (NETWORK_TYPE==1)
        for i=1:5:16
            for j=1:5:16
                data=[data; vectorize(img(i:i+4,j:j+4))'];
            end
        end

        for i=1:10:11
            for j=1:10:11
                data=[data; vectorize(img(i:i+9,j:j+9))'];
            end
        end

        for i=1:3:16
            data=[data; vectorize(img(i:i+4,:))'];
        end
    else
        data=vectorize(img)';
    end
end
    