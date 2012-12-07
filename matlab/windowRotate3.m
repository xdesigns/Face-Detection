%Function to rotate grayscale image using symmetry argument
function windowRotate3(imgSrc,mask);
    constants;
    roi_factor=0.9;
    img=double(imgSrc);  
    dims=size(img);
    data=[];        
    if (isempty(mask))
        mask=maskGenerator(dims(1),dims(2));
    end
    avgVal=0;
    count=0;
    
    for i=1:dims(1)
        for j=1:dims(2)
            if (mask(i,j)~=0)
                pixel = img(i,j);
                avgVal=avgVal+pixel;
                count=count+1;
            end
        end
    end
    avgVal=avgVal/count;
    for i=1:dims(1)
        for j=1:dims(2)
            if (mask(i,j)~=0)
                pixel = img(i,j);
                dx = pixel * (j-(dims(2)/2));
                dy = pixel * (i-(dims(1)/2));
                data = [data; dx dy];
            end
        end
    end
    if (1)
        covar=cov(data);
        [V D]=eig(covar);
        slope1 = (V(2,1)/V(1,1));
        slope2 = (V(2,2)/V(1,2));
        
        
          x1 = dims(2)*roi_factor;
          y1 = slope1 * x1 + dims(1) / 2 -slope1 * (dims(2)/2);
          x2 = dims(2)-x1;
          y2 = slope1 * x2 + dims(1)/2 - slope1 * (dims(2)/2);
          
          x3 = dims(2)*roi_factor;
          y3 = slope2 * x3 + dims(1) / 2 -slope2 * (dims(2)/2);
          x4 = dims(2)-x1;
          y4 = slope2 * x4 + dims(1)/2 - slope2 * (dims(2)/2);
          
       
        imshow (imgSrc,[0 255]);
        hold on   
        plot ([x1,x2],[y1,y2], '--', 'Color' ,'g', 'LineWidth', 2);
        plot ([x3,x4],[y3,y4], '--', 'Color' ,'g', 'LineWidth', 2);
        hold off
               
    end
    
    
end

    
                