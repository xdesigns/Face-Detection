/*******************************************************************************
 ******************** NEURAL NETWORK BASED FACE DETECTION **********************
 ****************** Copyright (C) 2010  All Rights Reserved ********************
 ************************* Created By: Nijith Jacob ****************************
 ************************ Email : nijith89@yahoo.com ***************************
 *******************************************************************************
 **   This program is free software: you can redistribute it and/or modify   ***
 **   it under the terms of the GNU General Public LiavgXYse as published by ***
 **   the Free Software Foundation, either version 3 of the LiavgXYse, or    ***
 **   (at your option) any later version.                                    ***
 **                                                                          ***
 **   This program is distributed in the hope that it will be useful,        ***
 **   but WITHOUT ANY WARRANTY; without even the implied warranty of         ***
 **   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          ***
 **   GNU General Public LiavgXYse for more details.                         ***
 *******************************************************************************
 *******************************************************************************
 **   This header file contains all supporting function used in the          ***
 **   program.                                                               ***
 *******************************************************************************
 ******************************************************************************/

#include "mathFunc.h"

#define minAngleInc                         ROTATION_MIN_ANGLE_INC*M_PI/180
#define maxAngleInc                         ROTATION_MAX_ANGLE_INC*M_PI/180


/**Function to generate elliptical mask**/
inline void maskGenerator(IplImage* mask,const double paramX=MASK_PARAM_X,
                                             const double paramY=MASK_PARAM_Y){
  /**Setting all pixels to black**/
  cvSet(mask,cvScalar(0));

  /**Fill the ellipse with white**/
  cvEllipse(mask,cvPoint(mask->width/2,mask->height/2),
                    cvSize(cvRound(mask->width*paramX/2),
                    cvRound(mask->height*paramY/2)),0,0,360,cvScalar(255),-1);
}

/**Function thar returns skin Fraction in the current skin Window**/
double getSkinAmount(IplImage* img){
  int i,j;
  int width                             =   img->width;
  int height                            =   img->height;
  int widthStep                         =   img->widthStep;
  double skinAmount                     =   0;
  unsigned char* imgData                =   (unsigned char*)img->imageData;
  unsigned char* ptr;

  for (i=0;i<height;++i){
    ptr                                 =   imgData+i*widthStep;
    for (j=0;j<width;++j){
      /**If pixel is not black**/
      if (*(ptr+j)>0u){
        ++skinAmount;
      }
    }
  }
  return skinAmount/(width*height);
}


/**Function to draw the face rectangle in the final image for a given angle**/
void drawRectangle(IplImage* img,ROI* ROIPtr,double maxWeight){
  double XCen                           =   ROIPtr->x;
  double YCen                           =   ROIPtr->y;
  double width                          =   ROIPtr->width;
  double height                         =   ROIPtr->height;
  double weight                         =   ROIPtr->weight;
  double angle                          =   ROIPtr->angle;

  double xDest[4]                       =   {0};
  double yDest[4]                       =   {0};

  CvPoint2D64f center                   =   cvPoint2D64f(XCen,YCen);
  /**Transfrom each corners based on the angle**/
  transformPoint(XCen-width/2,YCen-height/2,&xDest[0],&yDest[0],&center,angle);
  transformPoint(XCen+width/2,YCen-height/2,&xDest[1],&yDest[1],&center,angle);
  transformPoint(XCen+width/2,YCen+height/2,&xDest[2],&yDest[2],&center,angle);
  transformPoint(XCen-width/2,YCen+height/2,&xDest[3],&yDest[3],&center,angle);

  double colorIntensity                 =   pow(weight/maxWeight,
                                                        COLOR_DETERMINE_FACTOR);

  CvScalar squareColor                  =   {
                                             FACE_COLOR.val[0]*colorIntensity,
                                             FACE_COLOR.val[1]*colorIntensity,
                                             FACE_COLOR.val[2]*colorIntensity
  };


  /**Draw each of the edges**/
  cvLine  (img,cvPoint ((int)xDest[0],(int)yDest[0]),
                            cvPoint((int)xDest[1],(int)yDest[1]),
                                                squareColor,LINE_THICKESS);

  cvLine  (img,cvPoint ((int)xDest[1],(int)yDest[1]),
                            cvPoint((int)xDest[2],(int)yDest[2]),
                                                squareColor,LINE_THICKESS);

  cvLine  (img,cvPoint ((int)xDest[2],(int)yDest[2]),
                            cvPoint((int)xDest[3],(int)yDest[3]),
                                                squareColor,LINE_THICKESS);

  cvLine  (img,cvPoint ((int)xDest[3],(int)yDest[3]),
                            cvPoint((int)xDest[0],(int)yDest[0]),
                                                squareColor,LINE_THICKESS);
}


/**Function for color correction of the input image. Correction is performed
 by incrementing or decrementing the RGB values of each pixel if the value is
 greater than the average for each channels**/
void imgColorCorrection(IplImage* img){
  int i,j,k;
  int width                             =   img->width;
  int height                            =   img->height;
  int widthStep                         =   img->widthStep;
  int nChannels                         =   img->nChannels;
  unsigned char* imgData                =   (unsigned char*)(img->imageData);

  /**Computing the average of each channel**/
  CvScalar avgColor                     =   cvAvg(img);

  unsigned char *pixelPtr;
  unsigned char pixelData;

  for (i=0;i<width;++i){
    for (j=0;j<height;++j){
      pixelPtr                          =   imgData+j*widthStep+i*nChannels;
      for (k=0;k<nChannels;++k){
        pixelData                       =   *pixelPtr;
        if (pixelData>=(unsigned char)avgColor.val[k]){
          /**Incrementing if greater than average value**/
          *pixelPtr                     +=  pixelData<=255-COLOR_FACTOR?
                                               COLOR_FACTOR:255-COLOR_FACTOR;
        }
        else{
          /**Decrementing if less than average value**/
          *pixelPtr                     -=  pixelData>=COLOR_FACTOR?
                                                      COLOR_FACTOR:pixelData;
        }
        ++pixelPtr;
      }
    }
  }
}


/**Function for cropping a rectangular region from an image**/
IplImage* cropImage(IplImage* srcImage, const CvRect* cropArea){

  int x                                 =   cropArea->x;
  int y                                 =   cropArea->y;
  int cropWidth                         =   cropArea->width;
  int cropHeight                        =   cropArea->height;
  int width                             =   srcImage->width;
  int height                            =   srcImage->height;
  /**The top left corner of the crop area that lies within the image**/
  int availX                            =   x>=0?x:0;
  int availY                            =   y>=0?y:0;

  /**The available dimensions within the image**/
  int availWidth                        =   0;
  int availHeight                       =   0;
  if (x>=0){
    availWidth                          =   x+cropWidth>=width?
                                                              width-x:cropWidth;
  }
  else{
    availWidth                          =   x+cropWidth>=width?
                                                              width:x+cropWidth;
  }
  if (y>=0){
    availHeight                         =   y+cropHeight>=height?
                                                            height-y:cropHeight;
  }
  else{
    availHeight                         =   y+cropHeight>=height?
                                                            height:y+cropHeight;
  }

  if (availWidth<=0 || availHeight<=0){
    printf("Crop Image Error: No Area To Crop\n");
    exit(EXIT_FAILURE);
  }

  IplImage* destImage                   =   cvCreateImage(cvSize(availWidth,
                              availHeight),srcImage->depth,srcImage->nChannels);
  /**Set the crop area**/
  cvSetImageROI(srcImage,cvRect(availX,availY,availWidth,availHeight));
  cvCopy(srcImage,destImage);
  cvResetImageROI(srcImage);

  return destImage;
}


/**Function to check whether two rectangular regions overlap**/
bool isOverlapping(double XCen1, double YCen1, double width1, double height1 ,
                        double angle1,double XCen2, double YCen2, double width2,
                                                 double height2, double angle2){
  int i,j,k,l;
  /**Store the transformed corners of both rectangular regions**/
  double pts1[4][2];
  double pts2[4][2];
  CvPoint2D64f center1                  =   {XCen1,YCen1};
  CvPoint2D64f center2                  =   {XCen2,YCen2};


  /**Transform region 1**/
  transformPoint(XCen1-width1/2,YCen1-height1/2,&pts1[0][0],&pts1[0][1],
                                                                &center1,angle1);
  transformPoint(XCen1+width1/2,YCen1-height1/2,&pts1[1][0],&pts1[1][1],
                                                                &center1,angle1);
  transformPoint(XCen1+width1/2,YCen1+height1/2,&pts1[2][0],&pts1[2][1],
                                                                &center1,angle1);
  transformPoint(XCen1-width1/2,YCen1+height1/2,&pts1[3][0],&pts1[3][1],
                                                                &center1,angle1);

  /**Transform region 2**/
  transformPoint(XCen2-width2/2,YCen2-height2/2,&pts2[0][0],&pts2[0][1],
                                                                &center2,angle2);
  transformPoint(XCen2+width2/2,YCen2-height2/2,&pts2[1][0],&pts2[1][1],
                                                                &center2,angle2);
  transformPoint(XCen2+width2/2,YCen2+height2/2,&pts2[2][0],&pts2[2][1],
                                                                &center2,angle2);
  transformPoint(XCen2-width2/2,YCen2+height2/2,&pts2[3][0],&pts2[3][1],
                                                                &center2,angle2);

  bool flag;
  double a1,b1,c1,a2,b2,c2;
  double x,y;
  int sgn;


  for (i=0;i<4;++i){
    /**Get the adjacent corner for rectangle 2**/
    k                                   =   (i+1)%4;

    /**Get the coefficients of the edge joining the two corners
     a=y2-y1, b=x1-x2 and c=x2y1-x1y2 **/
    a2                                  =   pts2[k][1]-pts2[i][1];
    b2                                  =   pts2[i][0]-pts2[k][0];
    c2                                  =   pts2[k][0]*pts2[i][1]-
                                                          pts2[i][0]*pts2[k][1];
    flag                                =   true;

    /**Check for intersection of the edge with the edges of rectangle 1**/
    for (j=0;j<4;++j){
      /**Get the adjacent corner for rectangle 1**/
      l                                 =   (j+1)%4;

      /**Get the coefficients of the edge joining the two corners**/
      a1                                =   pts1[l][1]-pts1[j][1];
      b1                                =   pts1[j][0]-pts1[l][0];
      c1                                =   pts1[l][0]*pts1[j][1]-
                                                          pts1[j][0]*pts1[l][1];
      /**Sign of inside of rectangle 1**/
      sgn                               =   ((a1*XCen1+b1*YCen1+c1>=0)?1:-1);

      if (((a1*pts2[i][0]+b1*pts2[i][1]+c1>=0)?1:-1)!=sgn){
        flag                            =   false;
      }

      /**Find intersection point of both the edges**/
      if (a1*b2-a2*b1!=0){
        x                               =   (b1*c2-b2*c1)/(a1*b2-a2*b1);
        y                               =   (a2*c1-a1*c2)/(a1*b2-a2*b1);
        /**Check whether intersection point lies on the edges**/
        if (((x<=pts1[l][0]&&x>=pts1[j][0])||(x>=pts1[l][0]&&x<=pts1[j][0]))
            &&((x<=pts2[k][0]&&x>=pts2[i][0])||(x>=pts2[k][0]&&x<=pts2[i][0]))
            &&((y<=pts1[l][1]&&y>=pts1[j][1])||(y>=pts1[l][1]&&y<=pts1[j][1]))
            &&((y<=pts2[k][1]&&y>=pts2[i][1])||(y>=pts2[k][1]&&y<=pts2[i][1])))
          return true;
      }
    }
    /**Check whether any corner of rectangle 2 lies within rectangle 1**/
    if (flag)
      return true;
  }
  return false;
}


/**Function to rotate an image by a certain angle
 Note: This function requires images having same number of channels**/
void transformRotate(IplImage* imgSrc,IplImage* imgDst,
                               const CvPoint2D64f* center,const double angle){

  IplImage*  img                        =   cvCreateImage(cvGetSize(imgSrc),
                                               IPL_DEPTH_64F,imgSrc->nChannels);
  cvConvertScale(imgSrc,img);

  IplImage* rotateImg                   =   cvCreateImage(cvGetSize(imgDst),
                                               IPL_DEPTH_64F,imgDst->nChannels);
  cvConvertScale(imgDst,rotateImg);

  int i,j;
  double XSrc                           =   0;
  double YSrc                           =   0;
  double XDest                          =   0;
  double YDest                          =   0;
  int srcWidth                          =   img->width;
  int srcHeight                         =   img->height;
  int destWidth                         =   rotateImg->width;
  int destHeight                        =   rotateImg->height;
  int nChannels                         =   img->nChannels;

  double XCen                           =   center->x;
  double YCen                           =   center->y;

  for (i=0;i<destHeight*destWidth;++i){
    XDest                               =   i%destWidth;
    YDest                               =   i/destWidth;
    transformPoint(XDest,YDest,&XSrc,&YSrc,center,-angle);
    if ((int)XSrc<srcWidth&&(int)XSrc>=0&&(int)YSrc>=0&&(int)YSrc<srcHeight){
      for (j=0;j<nChannels;++j){
        *((double*)(rotateImg->imageData)+(int)YDest*rotateImg->widthStep/
            sizeof(double)+(int)XDest*nChannels+j)=*((double*)(img->imageData)+
                 (int)YSrc*img->widthStep/sizeof(double)+(int)XSrc*nChannels+j);
      }
    }
  }
  cvConvertScale(rotateImg,imgDst);
  cvReleaseImage(&img);
  cvReleaseImage(&rotateImg);
}


/**Function to rotate a window based on its symmetry. Principal Component
 Analysis(PCA) is empoyed to find the eigenvectors along the symmetry axes**/
IplImage* windowRotate(IplImage* img,double* angle,
                                               CvSize* cropArea,IplImage* mask){

  IplImage* img64F                      =   cvCreateImage(cvGetSize(img),
                                                               IPL_DEPTH_64F,1);
  cvConvertScale(img,img64F);

  int i;
  int x,y;
  /**Number of pixels considered for symmetry calculation**/
  int count                             =   0;

  int width                             =   img->width;
  int height                            =   img->height;

  double *dataArr                       =   NULL;
  /**Temporary variable to hold a pixel value**/
  double pixel                          =   0;

  /**Sum of the pixels considered for symmetry calculation**/
  double pixelSum                       =   0;

  double avgXY[2]                       =   {0};

  /**Matrix array to store the different pixels coordinates**/
  CvMat** arrMat                        =   (CvMat**)malloc(width*height*
                                                              sizeof(CvMat**));

  for (i=0;i<height*width;++i){
    x                                   =   i%width;
    y                                   =   i/width;
    /**If pixel lies within the mask**/
    if (*((unsigned char*)(mask->imageData)+y*mask->widthStep+x)>0u){
      arrMat[count]                     =   cvCreateMat(1,2,CV_64FC1);
      pixel                             =   *((double*)(img64F->imageData)+y*
                                             img64F->widthStep/sizeof(double)+x);
      pixelSum                          +=  pixel;

      dataArr                           =   (double*)malloc(sizeof(double)*2);
      dataArr[0]                        =   (x-width/2)*pixel;
      dataArr[1]                        =   (y-height/2)*pixel;

      arrMat[count]->data.db            =   dataArr;

      avgXY[0]                          +=  dataArr[0];
      avgXY[1]                          +=  dataArr[1];
      ++count;
    }
  }

  if (count){
    avgXY[0]                            /=  count;
    avgXY[1]                            /=  count;
  }

  double  covarData[4],
          eigenVecData[4],
          eigenValData[2];

  IplImage *finalImage8U                =   NULL;
  IplImage *finalImage64F               =   NULL;

  /**Covariance Matrix**/
  CvMat* covar                          =   cvCreateMat(2,2,CV_64FC1);
  covar->data.db                        =   covarData;
  /**Eigen Vector Matrix**/
  CvMat* eigenVec                       =   cvCreateMat(2,2,CV_64FC1);
  eigenVec->data.db                     =   eigenVecData;
  /**Eigen Value Matrix**/
  CvMat* eigenVal                       =   cvCreateMat(2,1,CV_64FC1);
  eigenVal->data.db                     =   eigenValData;
  /**Average Matrix**/
  CvMat* avg                            =   cvCreateMat(1,2,CV_64FC1);
  avg->data.db                          =   avgXY;


  cvCalcCovarMatrix((const CvArr**)arrMat,count,covar,avg,CV_COVAR_NORMAL
                                                            |CV_COVAR_USE_AVG);

  cvEigenVV(covar,eigenVec,eigenVal,10e-15);

  double angle1                         =   tanh(eigenVec->data.db[1]/
                                                    eigenVec->data.db[0]);
  double angle2                         =   tanh(eigenVec->data.db[3]/
                                                    eigenVec->data.db[2]);

  if (absVal(angle1)<absVal(angle2)){
    *angle                              =   -angle1;
  }
  else{
    *angle                              =   -angle2;
  }
  IplImage* rotateImg                   =   cvCreateImage(cvGetSize(img64F),
                                                               IPL_DEPTH_64F,1);

  /**Set all pixels to average value in the final image**/
  cvSet(rotateImg,cvScalar(pixelSum/count));

  CvPoint2D64f center;
  center.x                              =   width/2;
  center.y                              =   height/2;
  transformRotate(img64F,rotateImg,&center,-*angle);

  CvRect cropRect;
  cropRect.x                            =   (int)((width-cropArea->width)/2);
  cropRect.y                            =   (int)((height-cropArea->height)/2);
  cropRect.width                        =   cropArea->width;
  cropRect.height                       =   cropArea->height;

  finalImage64F                         =   cropImage(rotateImg,&cropRect);

  finalImage8U                          =   cvCreateImage(cvGetSize
                                            (finalImage64F),IPL_DEPTH_8U,1);
  cvConvertScale(finalImage64F,finalImage8U);

  cvReleaseImage(&finalImage64F);
  cvReleaseImage(&rotateImg);
  cvReleaseMat(&covar);
  cvReleaseMat(&avg);
  cvReleaseMat(&eigenVec);
  cvReleaseMat(&eigenVal);

  for (i=0;i<count;++i){
    free(arrMat[i]->data.db);
    cvReleaseMat(&arrMat[i]);
  }

  cvReleaseImage(&img64F);
  free(arrMat);

  return finalImage8U;
}


/**Function that returns the amount of symmetry along the vertical axis after
 rotation about the center**/
double getSymmetry(IplImage* img, const double *XCen, const double *YCen,
                                        const double *angle,IplImage* mask){
  int i,j;
  int width                             =   img->width;
  int height                            =   img->height;
  int count                             =   0;
  CvPoint2D64f center                   =   {*XCen,*YCen};

  /**Check whether center is outside the image**/
  if (*XCen>=width-1 || *XCen<=0)
    return 0;

  double symmetryAmount                 =   0;

  IplImage* rotateImg                   =   cvCreateImage(cvGetSize(img),
                                                               IPL_DEPTH_64F,1);

  IplImage* rotateMask                  =   cvCreateImage(cvGetSize(mask),
                                                               IPL_DEPTH_64F,1);
  cvSet(rotateMask,cvScalar(0,0,0));

  double imgPixel1                      =   0;
  double imgPixel2                      =   0;
  double *maskPixel                     =   NULL;

  transformRotate(img, rotateImg,&center,*angle);
  transformRotate(mask, rotateMask,&center,*angle);

  double *imgData                       =   NULL;
  double *maskData                      =   NULL;

  /**Region of scanning for symmetry**/
  int startX                            =   0;
  int endX                              =   (int)*XCen;
  int effWidth                          =   0;
  int incAmount                         =   0;

  if (endX>width/2){
    startX                              =   2*endX-width;
    effWidth                            =   2*(width-endX);
  }
  else{
    startX                              =   0;
    effWidth                            =   2*endX;
  }

  for (i=0;i<height;i+=SYMMETRY_INC_Y){
    imgData                             =   (double*)rotateImg->imageData+i*
                                            rotateImg->widthStep/sizeof(double);

    maskData                            =   (double*)rotateMask->imageData+i*
                                            rotateMask->widthStep/sizeof(double);
    for (j=startX;j<endX;j+=SYMMETRY_INC_X){

      maskPixel                         =   maskData+j;
      incAmount                         =   effWidth-1-2*j+2*startX;
      if ((unsigned char)(*maskPixel) > 0u){
        if ((unsigned char)(*(maskPixel+incAmount)) > 0u){
            ++count;
            imgPixel1                   =   *(imgData+j);
            imgPixel2                   =   *(imgData+j+incAmount);
            symmetryAmount              +=  absVal(imgPixel1-imgPixel2);

        }
      }
    }
  }
  symmetryAmount                        /=  255*count;
  cvReleaseImage(&rotateImg);
  cvReleaseImage(&rotateMask);

  return 1-symmetryAmount;
}

/**Function that returns the amount of correlation along the vertical axis after
 rotation about the center**/
double getCorrelation(IplImage* img, const double *XCen, const double *YCen,
                                            const double *angle,IplImage* mask){
  int i,j;
  int width                             =   img->width;
  int height                            =   img->height;
  int count                             =   0;
  CvPoint2D64f center                   =   {*XCen,*YCen};

  /**Check whether center is outside the image**/
  if (*XCen>=width-1 || *XCen<=0)
    return 0;

  double correlationAmount              =   0;

  IplImage* rotateImg                   =   cvCreateImage(cvGetSize(img),
                                                               IPL_DEPTH_64F,1);

  IplImage* rotateMask                  =   cvCreateImage(cvGetSize(mask),
                                                               IPL_DEPTH_64F,1);
  cvSet(rotateMask,cvScalar(0));

  double imgPixel1                      =   0;
  double imgPixel2                      =   0;

  double *maskPixel                     =   NULL;


  transformRotate(img, rotateImg,&center,*angle);
  transformRotate(mask, rotateMask,&center,*angle);

  double *imgData                       =   NULL;
  double *maskData                      =   NULL;

  /**Region of scanning for correlation**/
  int startX                            =   0;
  int endX                              =   (int)*XCen;
  int effWidth                          =   0;

  int incAmount                         =   0;

  if (endX>width/2){
    startX                              =   2*endX-width;
    effWidth                            =   2*(width-endX);
  }
  else{
    startX                              =   0;
    effWidth                            =   2*endX;
  }

  double sumSquares1                    =   0;
  double sumSquares2                    =   0;
  double sum1                           =   0;
  double sum2                           =   0;
  double sumProduct                     =   0;

  for (i=0;i<height;i+=CORRELATION_INC_Y){
    imgData                             =   (double*)rotateImg->imageData+i*
                                            rotateImg->widthStep/sizeof(double);

    maskData                            =   (double*)rotateMask->imageData+i*
                                            rotateMask->widthStep/sizeof(double);
    for (j=startX;j<endX;j+=CORRELATION_INC_X){
      maskPixel                         =   maskData+j;
      incAmount                         =   effWidth-1-2*j+2*startX;

      if ((unsigned char)(*maskPixel) > 0u){
        if ((unsigned char)(*(maskPixel+incAmount)) > 0u){
            ++count;
            imgPixel1                   =  *(imgData+j);
            imgPixel2                   =   *(imgData+j+incAmount);
            sumSquares1                 +=  imgPixel1 * imgPixel1;
            sumSquares2                 +=  imgPixel2 * imgPixel2;
            sum1                        +=  imgPixel1;
            sum2                        +=  imgPixel2;
            sumProduct                  +=  imgPixel1 * imgPixel2;
        }
      }
    }
  }
  /**Calculate the correlation**/
  correlationAmount                      =  absVal((sumProduct-(sum1*sum1+sum2*
                                             sum2-sum1*sum2)/count)/(sqrt(
                                             sumSquares1-sum1*sum1/count)*sqrt(
                                             sumSquares2-sum2*sum2/count)));

  cvReleaseImage(&rotateImg);
  cvReleaseImage(&rotateMask);

  return correlationAmount;
}


/**Function to compute the angle of rotation through correlation / symmetry
 argument**/
IplImage* windowRotate2(IplImage* img,double* angle,CvSize* cropArea,
                                                                IplImage* mask){
  double i;
  double curFactor                      =   0;
  double prevFactor                     =   0;
  double maxFactor                      =   0;

  double curAngleInc                    =   minAngleInc;
  double XCen                           =   img->width/2;
  double YCen                           =   img->height/2;
  CvPoint2D64f center                   =   {XCen,YCen};
  IplImage* rotateImg                   =   cvCreateImage(cvGetSize(img),
                                                               IPL_DEPTH_8U,1);

  /**Indicates whether the correlation/symmetry is increasing or decreasing**/
  bool isIncreasing                     =   true;
  /**Rotate through the sweep angle**/
  for (i=ROTATION_START_ANGLE;i<=ROTATION_START_ANGLE+
                                       ROTATION_SWEEP_ANGLE;i+=curAngleInc){
    if (USE_SYMMETRY){
      curFactor                         =   getSymmetry(img,
                                                &XCen,&YCen,&i,mask);
    }
    else{
      curFactor                         =   getCorrelation(img,
                                                &XCen,&YCen,&i,mask);
    }

    /**If correlation/symmetry is decreasing increase the rate otherwise
     decrease**/
    if (isIncreasing){
      if (curFactor<prevFactor){
        isIncreasing                    =   false;
      }
      else if(curAngleInc>minAngleInc){
        curAngleInc                     =     curAngleInc<=minAngleInc*
                                              ROTATION_MUL_FACTOR?minAngleInc:
                                              curAngleInc/ROTATION_MUL_FACTOR;
      }
    }
    else{
      if (curFactor>prevFactor){
        isIncreasing                    =   true;
      }
      else if (curAngleInc<maxAngleInc){
        curAngleInc                     =   ROTATION_MUL_FACTOR*curAngleInc>=
                                            maxAngleInc?maxAngleInc:
                                            ROTATION_MUL_FACTOR*curAngleInc;
      }
    }
    /**If a maxima is found**/
    if (curFactor>maxFactor){
        maxFactor                       =   curFactor;
        *angle=-i;
    }
    prevFactor                          =   curFactor;

  }
  transformRotate(img, rotateImg,&center,-*angle);

  CvRect cropRect;
  cropRect.x                            =   (int)(XCen-cropArea->width/2);
  cropRect.y                            =   (int)(YCen-cropArea->height/2);
  cropRect.width                        =   cropArea->width;
  cropRect.height                       =   cropArea->height;

  IplImage* finalImage                  =   cropImage(rotateImg,&cropRect);
  cvReleaseImage(&rotateImg);
  return finalImage;
}

double getCorrelationRatio(IplImage* img,const double *XCen,const double *YCen,
                                            const double angle,IplImage* mask){

  double curAngle                       =   angle;

  double correlationVerAxis             =   getCorrelation(img,XCen,YCen,
                                                           &curAngle,mask);
  curAngle                              =   angle+M_PI/2;

  double correlationHorAxis             =   getCorrelation(img,XCen,YCen,
                                                           &curAngle,mask);
  return correlationHorAxis/correlationVerAxis;

}



/**Function for performing linear gradient lighting correction
 Note: Image should be grayscale**/
void lightingCorrection(IplImage* window,IplImage* mask){

  IplImage* window64F                   =   cvCreateImage(cvGetSize(window),
                                                              IPL_DEPTH_64F,1);
  cvConvertScale(window,window64F);

  int i;
  int x,y;
  int width                             =   window->width;
  int height                            =   window->height;
  int count                             =   0;

  double* matData                       =   (double*) malloc(sizeof(double)*3*
                                                                  width*height);
  double *bData                         =   (double*) malloc(sizeof(double)*
                                                                  width*height);

  for (i=0;i<width*height;++i){
    x                                   =   i%width;
    y                                   =   i/width;
    /**If pixel lies within the mask**/
    if (*((unsigned char*)(mask->imageData)+y*mask->widthStep+x)>0u){
      /**Store the values x,y,1.0 in A**/
      matData[3*count]                  =   x;
      matData[3*count+1]                =   y;
      matData[3*count+2]                =   1.0;
      /**Store Pixel Values in b**/
      bData[count++]                    =   *((double*)(window64F->imageData)+
                                                        y*window64F->widthStep/
                                                             sizeof(double)+x);
    }
  }

  CvMat* mat                            =   cvCreateMat(count,3,CV_64FC1);
  mat->data.db                          =   matData;

  CvMat* b                              =   cvCreateMat(count,1,CV_64FC1);
  b->data.db                            =   bData;

  /**The final matrix obtained**/
  CvMat* temp2                          =   cvCreateMat(3,1,CV_64FC1);
  double* temp2Data                     =   (double*)malloc(sizeof(double)*3);
  temp2->data.db                        =   temp2Data;

  double *srcPixel                      =   NULL;
  double factor                         =   0;

  pseudoInverse(mat,b,temp2);

  for (i=0;i<height*width;++i){
    x                                   =   i%width;
    y                                   =   i/width;
    srcPixel                            =   ((double*)(window64F->imageData)+i);

    factor                              =   LIGHTING_CORRECTION_FACTOR*
                                                      (temp2->data.db[0]*x+
                                                           temp2->data.db[1]*y);
    if (factor>=0)
      /**Decrement the pixel value by factor**/
      *srcPixel                         -=  absVal(factor)<=*srcPixel?
                                                      absVal(factor):*srcPixel;
    else
      /**Increment the pixel value by factor**/
      *srcPixel                         +=  absVal(factor)<=255-*srcPixel?
                                                  absVal(factor):255-*srcPixel;
  }

  IplImage* window8U                    =   cvCreateImage(cvGetSize(window64F),
                                                               IPL_DEPTH_8U,1);

  cvConvertScale(window64F,window8U);
  cvReleaseImage(&window64F);

  cvEqualizeHist(window8U,window8U);

  cvConvertScale(window8U,window);
  cvReleaseImage(&window8U);

  free(matData);
  free(bData);
  free(temp2Data);

  cvReleaseMat(&mat);
  cvReleaseMat(&b);
  cvReleaseMat(&temp2);
}


/**Function to validate incorrect paths. All instances of '/' is replaced by
 '\' and if the path is a directory then '\' is appended at the end**/
void validatePath(char *path, bool isDir){
  int i;
  const char delim                          =   '\\';
  int lastIndex                             =   strlen(path)-1;
  for (i=0;i<=lastIndex;++i){
    if (path[i] == '/'){
      path[i]                               =   delim;
    }
  }
  if (isDir && path[lastIndex]!=delim){
    path[lastIndex+1]                       =   delim;
    path[lastIndex+2]                       =   '\0';
  }

}



/**Function to get the full path name of a file**/
bool getPath(char *filePath,char *fileName, char* dest){
  int i;
  validatePath(filePath,false);
  /**Pointer to extension of the file name**/
  char *extIndex;

  /**Boolean indicating whether any valid image extension is found**/
  bool flag                                 =   false;

  /**Check whether extension is valid**/
  for (i=0;i<NO_VALID_EXT;++i){
    if (extIndex=strchr(fileName,'.')){

      if (stricmp(extIndex,VALID_EXT[i])==0){
        /**If valid extension is found**/
        flag                                =   true;
        break;
      }
    }
  }
  /**Delimiters used in file names**/
  const char delim                          =   '\\';

  int index;
  char* ptr;

  if (ptr=strrchr(filePath,delim))
    index                                   =   ptr-filePath+1;
  else
    index                                   =   0;

  /**Create the full path name**/
  strncpy(dest,filePath,index);
  strcpy(dest+index,fileName);
  return flag;
}

/**Function to recursively create a directory path**/
void makeDir(char* dir){
  validatePath(dir,true);

  /**Pointer to remaining part of directory to be created**/
  char* dirRem                              =   dir;

  char currentDir[FILENAME_MAX];

  int index;
  char* ptr;

  char delim                                =   '\\';

  while(true){
    if (ptr=strchr(dirRem,delim))
      index                                 =   ptr-dir+1;
    else
      index                                 =   0;

    if (index<=0){
      break;
    }

    dirRem                                  =   dir+index;
    strncpy(currentDir,dir,index);
    currentDir[index]                       =   '\0';
    mkdir(currentDir);
  }
}

