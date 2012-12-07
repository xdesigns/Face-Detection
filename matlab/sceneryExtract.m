function data=sceneryExtract(file,num)
    constants;
    img=imread(file);
    dims=size(img);
    maxScale=floor((log(min([dims(2) dims(1)]))-log(WINDOW_WIDTH))/log(SCALING_FACTOR));
    imgPerScale=ceil(num/(maxScale+1));
    count=0;
    data=[];
    for i=0:maxScale
        scale=1/(SCALING_FACTOR^i);
        for j=1:imgPerScale
            count=count+1;
            if (count>num)
                break;
            else
                img2=imresize(img,scale);
                dims=size(img2);
                randX=randint(1,1,dims(2)-CROPPED_WIDTH+1)+1;
                randY=randint(1,1,dims(1)-CROPPED_HEIGHT+1)+1;
                img2=imcrop(img2,[randX randY CROPPED_WIDTH-1 CROPPED_HEIGHT-1]);
                img2=lightingCorrection(img2,[]);
                data=[data extractData(img2)];
            end
        end
    end
        
    
end
    
    
        
        
        
    
    