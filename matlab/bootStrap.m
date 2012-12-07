    scenerySelected=randint(1,1,NO_SCENE_FILES)+1;
    file=sceneDB{scenerySelected,1};
    prevAvail=noNonFacesAvail;
    disp(sprintf('BootStrapping on image %s',file));
    data=sceneryExtract(file,MAX_BOOTSTRAP_TRIALS);

    out=sim(net,data);
    disp(sprintf('Extracted %i images',MAX_BOOTSTRAP_TRIALS));
    for i=1:MAX_BOOTSTRAP_TRIALS
        if (out(1,i)>0)
            noNonFacesAvail=noNonFacesAvail+1;
            imgS=[];
            for j=1:CROPPED_HEIGHT;
                imgS=[imgS ;data((j-1)*CROPPED_WIDTH+1:j*CROPPED_WIDTH,i)'];
            end
            imgFile=[NON_FACE_DIR{1} sprintf('%i.jpg',noNonFacesAvail)];
            fclose(fopen(imgFile,'w'));
            imwrite(uint8(imgS),imgFile);
        end
    end
    disp(sprintf('Found %i New Non-Faces',noNonFacesAvail-prevAvail));            
        