function mask=maskGenerator(height,width);
    constants;
    a=width*MASK_PARAM_X/2;
    b=height*MASK_PARAM_Y/2;
    mask=[];
    for i=0:height-1
        for j=0:width-1
            if (((j-(width/2)-0.5)/a)^2+((i-(height/2)-0.5)/b)^2>=1 | ((j-(width/2)+0.5)/a)^2+((i-(height/2)+0.5)/b)^2>=1 |...
                ((j-(width/2)+0.5)/a)^2+((i-(height/2)-0.5)/b)^2>=1 | ((j-(width/2)-0.5)/a)^2+((i-(height/2)+0.5)/b)^2>=1)
                mask(i+1,j+1)=0;
            else
                mask(i+1,j+1)=255;
            end
        end
    end
end


    
