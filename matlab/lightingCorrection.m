function image=lightingCorrection(image,mask);
    image=double(image);
    constants;
    dims=size(image);
    rows=dims(1);
    cols=dims(2);
    if (isempty(mask))
        mask=maskGenerator(rows,cols);
    end
    A=[];
    B=[];
    for i=0:(rows-1)
        for j=0:(cols-1)
            if (mask(i+1,j+1)~=0)
                A=[A;j i 1];
                B=[B;image(i+1,j+1)];
            end
        end
    end
    result=inv(A'*A)*A'*B;
    for i=1:rows
        for j=1:cols
            factor=LIGHTING_CORRECTION_FACTOR*(result(1)*(j-1)+result(2)*(i-1));
            if (factor>=0)
                if (abs(factor)<=image(i,j))
                    image(i,j)=image(i,j)-abs(factor);
                else
                    image(i,j)=0;
                end
            else
                if (abs(factor)<=255-image(i,j))
                    image(i,j)=image(i,j)+abs(factor);
                else
                    image(i,j)=255;
                end
            end
        end
    end
    image=uint8(image);
    image=histeq(image);
    image=double(image);
end
     