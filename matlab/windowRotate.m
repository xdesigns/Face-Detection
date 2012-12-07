%Function to rotate grayscale image using symmetry argument
function img=windowRotate(imgSrc,crop,mask);
    constants;
    img=double(imgSrc);  
    dims=size(img);
    data=[];        
    if (isempty(crop))
        crop=[CROPPED_WIDTH CROPPED_HEIGHT];
    end
    if (isempty(mask))
        mask=maskGenerator(dims(1),dims(2));
    end
    avgVal=0;
    count=0;
    for i=1:dims(1)
        for j=1:dims(2)
            if (mask(i,j)~=0)
                avgVal=avgVal+img(i,j);
                count=count+1;
            end
        end
    end
    avgVal=avgVal/count;
    count=0;  
    avgXY=[0 0];
    for i=1:dims(1)
        for j=1:dims(2)
            if (img(i,j)>=avgVal & mask(i,j)~=0)
                data=[data; j-1 i-1];
                avgXY(1)=avgXY(1)+j-1;
                avgXY(2)=avgXY(2)+i-1;
                count=count+1;
            end
        end
    end
    avgXY=avgXY/count;
    if (((2*avgXY(1))<=(dims(2)*(1+ROT_CENTER_X_FACTOR))) & ((2*avgXY(1))>=(dims(2)*(1-ROT_CENTER_X_FACTOR))) & ((2*avgXY(2))<=(dims(1)*(1+ROT_CENTER_Y_FACTOR))) & ((2*avgXY(2))>=(dims(1)*(1-ROT_CENTER_Y_FACTOR))))
        covar=cov(data);
        [V D]=eig(covar);
        if (abs(V(2,1))<=abs(V(1,1)))
            opp=V(2,1);
            adj=V(1,1);
        else
            opp=V(2,2);
            adj=V(1,2);
        end
        hyp=sqrt(opp*opp+adj*adj);
        sin=abs(opp/hyp);
        cos=abs(adj/hyp);
        if (opp/adj>=0)
            sin=-sin;
        end
        img=transformRotate(img,sin,cos,avgXY,avgVal);
        img=imcrop(img,[avgXY(1)-crop(1)/2 avgXY(2)-crop(2)/2 crop(1)-1 crop(2)-1]); 
    else
        img=imcrop(img,[(dims(2)-crop(1))/2 (dims(1)--crop(2))/2 crop(1)-1 crop(2)-1]); 
    end
end

function tImg=transformRotate(img,sin,cos,cen,avgVal)
    dims=size(img);
    tImg=avgVal*ones(dims);
    for i=1:dims(1)
        for j=1:dims(2)
            x=j*cos+i*sin+(1-cos)*cen(1)-sin*cen(2);
            y=-j*sin+i*cos+(1-cos)*cen(2)+sin*cen(1);
            if (x<=dims(2) && x>=1 && y>=1 && y<=dims(1))
                tImg(i,j)=img(round(y),round(x));
            end
        end
    end
end
        
    
                