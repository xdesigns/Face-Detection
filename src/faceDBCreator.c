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
 **   This C file is used for for creating face DB.                          ***
 *******************************************************************************
 ******************************************************************************/

#include "auxilary.h"
#include <time.h>

/**Parameters for creating face variations**/

const double MAX_SCALE                  =   0.9545;
const double MIN_SCALE                  =   -0.8713;
const double MAX_ROTATE                 =   5;
const double MIN_ROTATE                 =   -5;
const double MAX_TRANSLATE              =   0.5;
const double MIN_TRANSLATE              =   -0.5;
const int NO_VARIATIONS                 =   10;


/**A function used to fill the matrix used for transformation in DB Creator**/
inline void faceDbCreatorFillData(double* tempMatData,const double* fillData,
                                                    const int noFeaturePoints){
  int i;
  for (i=0;i<noFeaturePoints;++i){
    tempMatData[i*8]                    =   fillData[i*2];
    tempMatData[i*8+1]                  =   -fillData[i*2+1];
    tempMatData[i*8+2]                  =   1.0;
    tempMatData[i*8+3]                  =   0;

    tempMatData[i*8+4]                  =   fillData[i*2+1];
    tempMatData[i*8+5]                  =   fillData[i*2];
    tempMatData[i*8+6]                  =   0;
    tempMatData[i*8+7]                  =   1.0;
  }
}



void faceDbCreator(const char filePath[50],const char coordsFilename[100],
                                      const int startFile,const int endFile,
                                      const int noIterations,const int border){
  /**Number of Feature Points used in aligning images.**/
  const int noFeaturePoints             =   4;

  const int initialSize                 =   38;

  int i,j,k,iteration;

  /**No of files from DB added for alignment**/
  int noFiles                           =   0;
  double xr                             =   0;
  double yr                             =   0;
  int x,y;
  char filePathCopy[100];
  /**Corrds of the standards face with respect to initialSize**/
  CvMat *stdCoords                      =   cvCreateMat(noFeaturePoints*2,1,
                                                                  CV_64FC1);
  double stdCoordsData[]                =   {5+border,6+border,32+border,
                                            6+border,18+border,15+border,
                                                    18+border,25+border};
  stdCoords->data.db                    =   stdCoordsData;

  /**Average Coords of the faces aligned so far**/
  double avgData[noFeaturePoints*2];
  CvMat *avgMat                         =   cvCreateMat(noFeaturePoints*2,1,
                                                                    CV_64FC1);
  avgMat->data.db                       =   avgData;
  /**Coords to which other coordinates are aligned to**/
  double testData[noFeaturePoints*2];
  CvMat *testMat                        =   cvCreateMat(noFeaturePoints*2,1,
                                                                    CV_64FC1);
  testMat->data.db                      =   testData;

  cvCopy(stdCoords,testMat);

  double tempCoords[noFeaturePoints*2];

  /**Coords of all the image in the database**/
  CvMat* coords[endFile-startFile+1];

  double coordsData[endFile-startFile+1][noFeaturePoints*8];

  /**Face DB image file names**/
  char fileNames[endFile-startFile+1][100];
  char tempFileName[100];
  char tempStr[50];

  IplImage *img                         =   NULL;
  IplImage *dst                         =   NULL;

  FILE* coordsFile                      =   fopen(coordsFilename,"r+");
  FILE* t                               =   NULL;

  if (coordsFile){
    for (i=-startFile+1;i<=endFile-startFile;++i){
      if(!feof(coordsFile)){
        fscanf(coordsFile,"%s %lf %lf %lf %lf %lf %lf %lf %lf",&tempStr,
                                &tempCoords[0],&tempCoords[1],&tempCoords[2],
                                &tempCoords[3],&tempCoords[4],&tempCoords[5],
                                                &tempCoords[6],&tempCoords[7]);
        /**Skip the coords upto startImage**/
        if (i>=0){
          strcpy(tempFileName,filePath);
          strcat(tempFileName,tempStr);
          /**Check whether the file exists**/
          if (t=fopen(tempFileName,"r")){
            fclose(t);
            strcpy(fileNames[noFiles],tempFileName);

            coords[noFiles]             =  cvCreateMat(noFeaturePoints*2,4,
                                                                    CV_64FC1);
            faceDbCreatorFillData(coordsData[noFiles],tempCoords,noFeaturePoints);

            coords[noFiles]->data.db    =   coordsData[noFiles];

            ++noFiles;
          }
        }
      }
      else{
        noFiles                         =   i-1;
        break;
      }
    }
    fclose(coordsFile);

    if (!noFiles){
      printf("Face DB Creator Error: No File To Process\n");
      exit(EXIT_FAILURE);
    }
  }
  else {
    printf("Face DB Creator Error: Could Not Open Coords File\n");
    exit(EXIT_FAILURE);
  }

  /**PsuedoInverse**/
  CvMat *temp2                          =   cvCreateMat(4,1,CV_64FC1);
  double tempData2[4];
  temp2->data.db                        =   tempData2;

  for (iteration=0;iteration<noIterations;++iteration){
    cvSetZero(avgMat);
    for (i=0;i<noFiles;++i){
      pseudoInverse(coords[i],testMat,temp2);
      for (j=0;j<noFeaturePoints;++j){
        xr                              =   coordsData[i][j*8]*temp2->data.db[0]
                                            -coordsData[i][j*8+4]*
                                            temp2->data.db[1]+temp2->data.db[2];

        yr                              =   coordsData[i][j*8]*temp2->data.db[1]
                                            +coordsData[i][j*8+4]*
                                            temp2->data.db[0]+temp2->data.db[3];
        coordsData[i][j*8]              =   xr;
        coordsData[i][j*8+5]            =   xr;
        coordsData[i][j*8+1]            =   -yr;
        coordsData[i][j*8+4]            =   yr;
        avgData[j*2]                    +=  xr;
        avgData[j*2+1]                  +=  yr;
      }

      img                               =   cvLoadImage(fileNames[i],
                                                      CV_LOAD_IMAGE_GRAYSCALE);

      dst                               =   cvCreateImage(cvSize(initialSize+
                                                2*border,initialSize+2*border),
                                                    img->depth,img->nChannels);
      cvSetZero(dst);

      double a                          =   temp2->data.db[0];
      double b                          =   temp2->data.db[1];
      double det                        =   a*a+b*b;
      double tx                         =   temp2->data.db[2];
      double ty                         =   temp2->data.db[3];

      /**Transform the image**/
      for (j=0;j<dst->height;++j){
        for (k=0;k<dst->width;++k){
          xr                            =   ((k-tx)*a+(j-ty)*b)/det;
          yr                            =   ((k-tx)*-b+(j-ty)*a)/det;
          if ((int)xr>=0 && (int)xr <img->width && (int)yr>=0
                                                 && (int)yr<img->height){
            *((unsigned char*)(dst->imageData)+j*dst->widthStep+k)=
                                    *((unsigned char*)(img->imageData)+
                                           (int)yr*img->widthStep+(int)xr);
          }

        }
      }
      cvSaveImage(fileNames[i],dst);
      cvReleaseImage(&img);
      cvReleaseImage(&dst);

    }

    /**Averge of the transformation performed so far**/
    for (j=0;j<noFeaturePoints*2;++j){
      avgData[j]                        /=  endFile-startFile+1;
    }
    /**Perform transformation on the average data**/
    CvMat* tempMat                      =   cvCreateMat(noFeaturePoints*2,4,
                                                                      CV_64FC1);
    double tempMatData[noFeaturePoints*8];
    tempMat->data.db                    =   tempMatData;
    faceDbCreatorFillData(tempMatData,avgData,noFeaturePoints);

    pseudoInverse(tempMat,stdCoords,temp2);

    for (j=0;j<noFeaturePoints;++j){
      testData[j*2]                     =   avgData[j*2]*temp2->data.db[0]-
                                            avgData[j*2+1]*temp2->data.db[1]+
                                                             temp2->data.db[2];
      testData[j*2+1]                   =   avgData[j*2]*temp2->data.db[1]+
                                            avgData[j*2+1]*temp2->data.db[0]+
                                                             temp2->data.db[3];
    }
    cvReleaseMat(&tempMat);
  }

  IplImage *img8U,*img64F;
  CvRect *cropArea;

  IplImage *finalImage32F               =   cvCreateImage(cvSize(CROPPED_WIDTH,
                                               CROPPED_HEIGHT),IPL_DEPTH_32F,1);
  IplImage *finalImage8U                =   cvCreateImage(cvSize(CROPPED_WIDTH,
                                                CROPPED_HEIGHT),IPL_DEPTH_8U,1);
  IplImage *transformImage64F;
  IplImage *transformImage32F;
  IplImage *croppedImage32F             =   cvCreateImage(cvSize(initialSize,
                                                  initialSize),IPL_DEPTH_32F,1);
  IplImage *croppedImage64F             =   cvCreateImage(cvSize(initialSize,
                                                  initialSize),IPL_DEPTH_64F,1);

  IplImage* mask                        =   cvCreateImage(cvGetSize
                                              (croppedImage64F),IPL_DEPTH_8U,1);
  maskGenerator(mask);

  /**Random transformations**/
  double scale                          =   0;
  double rotate                         =   0;
  double translateX                     =   0;
  double translateY                     =   0;

  tempStr[0]                            =   '_';
  tempStr[4]                            =   '.';
  tempStr[5]                            =   'j';
  tempStr[6]                            =   'p';
  tempStr[7]                            =   'g';
  tempStr[8]                            =   '\0';

  /**Random Number Generator**/
  CvRNG rg;

  for (i=0;i<noFiles;++i){
    img8U                               =   cvLoadImage(fileNames[i],
                                                       CV_LOAD_IMAGE_GRAYSCALE);
    img64F                              =   cvCreateImage(cvGetSize(img8U),
                                                               IPL_DEPTH_64F,1);
    cvConvertScale(img8U,img64F);
    cvReleaseImage(&img8U);

    remove(fileNames[i]);

    xr                                  =   coordsData[i][0]-stdCoordsData[0]+
                                                                         border;
    yr                                  =   coordsData[i][4]-stdCoordsData[1]+
                                                                         border;
    cvSetImageROI(img64F,cvRect(cvRound(xr),cvRound(yr),initialSize,
                                                            initialSize));
    cvCopy(img64F,croppedImage64F);

    /**Creating variations for each image**/
    for (j=0;j<NO_VARIATIONS;++j){
      lightingCorrection(croppedImage64F,mask);
      rg                                =   cvRNG(time(0)*1000*(i+20)*(j+30));

      cvConvertScale(croppedImage64F,croppedImage32F);
      cvResize(croppedImage32F,finalImage32F);
      cvConvertScale(finalImage32F,finalImage8U);
      tempStr[1]                        =   (j/100)%10+48;
      tempStr[2]                        =   (j/10)%10+48;tempStr[3]=j%10+48;

      strncpy(tempFileName,fileNames[i],strlen(fileNames[i])-4);

      tempFileName[strlen(fileNames[i])-4]
                                        ='\0';
      strcat(tempFileName,tempStr);

      cvSaveImage(tempFileName,finalImage8U);
      switch (cvRandInt(&rg)%3){
        /**Scaling**/
        case 0:
          if (cvRandInt(&rg)%2)
            scale                       =   cvRandReal(&rg)*MAX_SCALE*
                                            initialSize/CROPPED_WIDTH;
          else
            scale                       =   cvRandReal(&rg)*MIN_SCALE*
                                            initialSize/CROPPED_HEIGHT;

          transformImage64F             =   cvCreateImage(
                                            cvSize(cvRound(initialSize-2*scale),
                                            cvRound(initialSize-2*scale)),
                                            IPL_DEPTH_64F,1);

          transformImage32F             =   cvCreateImage(
                                            cvSize(cvRound(initialSize-2*scale),
                                            cvRound(initialSize-2*scale)),
                                            IPL_DEPTH_32F,1);

          cvSetImageROI(img64F,cvRect(cvRound(xr+scale),cvRound(yr+scale),
                    cvRound(initialSize-2*scale),cvRound(initialSize-2*scale)));

          cvCopy(img64F,transformImage64F);
          cvConvertScale(transformImage64F,transformImage32F);

          cvResize(transformImage32F,croppedImage32F);
          cvConvertScale(croppedImage32F,croppedImage64F);
          cvReleaseImage(&transformImage64F);
          cvReleaseImage(&transformImage32F);
          break;
        /**Rotation**/
        case 1:
          if (cvRandInt(&rg)%2)
            rotate                      =   cvRandReal(&rg)*MAX_ROTATE;
          else
            rotate                      =   cvRandReal(&rg)*MIN_ROTATE;

          cvResetImageROI(img64F);

          transformImage64F             =   cvCreateImage(cvGetSize(img64F),
                                                            IPL_DEPTH_64F,1);
          transformRotate(img64F,transformImage64F,
          &cvPoint2D64f(xr+initialSize/2,yr+initialSize/2),rotate*M_PI/180);

          cvSetImageROI(transformImage64F,
                            cvRect(xr,yr,initialSize,initialSize));

          cvCopy(transformImage64F,croppedImage64F);
          cvReleaseImage(&transformImage64F);
          break;
        default:
          /**Translation**/
          if (cvRandInt(&rg)%2){
            if (cvRandInt(&rg)%2){
              translateX                =   cvRandReal(&rg)*MAX_TRANSLATE*
                                                    initialSize/CROPPED_WIDTH;
              translateY                =   cvRandReal(&rg)*MAX_TRANSLATE*
                                                    initialSize/CROPPED_HEIGHT;
            }
            else{
              translateX                =   cvRandReal(&rg)*MIN_TRANSLATE*
                                                    initialSize/CROPPED_WIDTH;
              translateY                =   cvRandReal(&rg)*MIN_TRANSLATE*
                                                    initialSize/CROPPED_HEIGHT;
            }
          }
          else{
            if (cvRandInt(&rg)%2){
              translateX                =   cvRandReal(&rg)*MAX_TRANSLATE*
                                                    initialSize/CROPPED_WIDTH;
              translateY                =   cvRandReal(&rg)*MIN_TRANSLATE*
                                                    initialSize/CROPPED_HEIGHT;
            }
            else{
              translateX                =   cvRandReal(&rg)*MIN_TRANSLATE*
                                                    initialSize/CROPPED_WIDTH;
              translateY                =   cvRandReal(&rg)*MAX_TRANSLATE*
                                                    initialSize/CROPPED_HEIGHT;
            }
          }
          cvSetImageROI(img64F,cvRect(cvRound(xr+translateX),
                              cvRound(yr+translateY),initialSize,initialSize));
          cvCopy(img64F,croppedImage64F);
      }
    }
    cvReleaseImage(&img64F);
    cvReleaseMat(&coords[i]);
  }
  cvReleaseImage(&finalImage8U);
  cvReleaseImage(&finalImage32F);
  cvReleaseImage(&croppedImage32F);
  cvReleaseImage(&croppedImage64F);
  cvReleaseMat(&stdCoords);
  cvReleaseMat(&testMat);
  cvReleaseMat(&avgMat);
  cvReleaseMat(&temp2);
}


int main(){
  faceDbCreator("../db/faces/","../db/face_coords.txt",1,5,1,20);
  return 0;
}
