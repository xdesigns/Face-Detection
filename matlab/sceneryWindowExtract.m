function img=sceneryExtract(file)
    constants;
    img=imread(file);
    dims=size(img);
    maxScale=floor((log(min([dims(2) dims(1)]))-log(WINDOW_WIDTH))/log(SCALING_FACTOR));
    randScale=randint(1,1,maxScale+1);
    scale=1/(SCALING_FACTOR^randScale);
    
    img=imresize(img,scale);
    dims=size(img);
    randX=randint(1,1,dims(2)-CROPPED_WIDTH+1)+1;
    randY=randint(1,1,dims(1)-CROPPED_HEIGHT+1)+1;
    img=imcrop(img,[randX randY CROPPED_WIDTH-1 CROPPED_HEIGHT-1]);
    img=lightingCorrection(img,[]);
end
    
    
        
        
        
    
    