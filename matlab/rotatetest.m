function rotatetest(imgSrc, lcorr);
    img = imread(imgSrc);
    if lcorr
      img = lightingCorrection(img,[]);
    end
    windowRotate3(img,[]);
end