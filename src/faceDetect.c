/*******************************************************************************
 ******************** NEURAL NETWORK BASED FACE DETECTION **********************
 ****************** Copyright (C) 2010  All Rights Reserved ********************
 ************************* Created By: Nijith Jacob ****************************
 ************************ Email : nijith89@yahoo.com ***************************
 *******************************************************************************
 **   This program is free software: you can redistribute it and/or modify   ***
 **   it under the terms of the GNU General Public License as published by   ***
 **   the Free Software Foundation, either version 3 of the License, or      ***
 **   (at your option) any later version.                                    ***
 **                                                                          ***
 **   This program is distributed in the hope that it will be useful,        ***
 **   but WITHOUT ANY WARRANTY; without even the implied warranty of         ***
 **   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          ***
 **   GNU General Public License for more details.                           ***
 *******************************************************************************
 *******************************************************************************
 **   This is the main file. Header files include:-                          ***
 **   * auxilary.h      :   Supporting functions                             ***
 **   * constants.h     :   Constants and structures used                    ***
 **   * mathFunc.h      :   Math functions                                   ***
 **   * msg.h           :   Information messages                             ***
 **   * neural.h        :   Neural network initialisation and simulation     ***
 **   * skinLocaliser.h :   Skin Pixels related functions                    ***
 *******************************************************************************
 ******************************************************************************/
#include <conio.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "highgui.h"
#include "cv.h"
#include "constants.h"
#include "skinLocaliser.h"
#include "auxilary.h"
#include "msg.h"
#include "neural.h"


/**Function that stores the skin pixels found in the input image and stores them
 in skinImage. This image is used in computing skin fraction for each scanned
 window**/
void initSkinRegion(IplImage* img,  IplImage* skinImage, bool verbose){
  int i, j;
  int numSkinPixels                     =   0;
  int width                             =   img->width;
  int height                            =   img->height;
  unsigned char *ptrSrc                 =   NULL;
  unsigned char *ptrDest                =   NULL;

  unsigned char* srcImgData             =   (unsigned char*)img->imageData;
  unsigned char* destImgData            =   (unsigned char*)skinImage->
                                                                 imageData;
  int srcWidthStep                      =   img->widthStep;
  int destWidthStep                     =   skinImage->widthStep;

  for (i=0;i<height;++i){
    ptrSrc                              =   srcImgData+i*srcWidthStep;
    ptrDest                             =   destImgData+i*destWidthStep;
    for (j=0;j<width;++j){
      if (isSkin(ptrSrc+j*3)){
        ++numSkinPixels;
        /**If skin, the pixel is made white in skinImage**/
        *(ptrDest+j)                    =   0xFFu;
      }
      else{
        /**If not skin, the pixel is made black in skinImage**/
        *(ptrDest+j)                    =   0x00u;
      }
    }
  }
  if (verbose){
    if (numSkinPixels){
      printf("Skin pixels in the image: %i%%\n",numSkinPixels*100/(height*width));
    }
    else{
      printf("Warning: No skin pixels found\n");
    }
    msgSeperator();
  }

}


/**Function that performs clean up after the image is scanned. Clean up is done
 by eliminating overlapping detections with window of the highest weight**/
void cleanUp(struct list* listFirstPtr,int noScales,bool verbose){

  int i;
  /**The current scale of the list being scanned**/
  double currentScale                   =   0;
  double tempScale                      =   0;
  /**Center, angle & weight of the ROI currently scanned**/
  double XCen                           =   0;
  double YCen                           =   0;
  double angle                          =   0;
  double weight                         =   0;

  /**Linked list pointers used to iterate through the list**/
  struct list *listPtr                  =   listFirstPtr;
  struct list *listTempPtr              =   NULL;
  struct ROI *ROIPtr                    =   NULL;
  struct ROI *ROITempPtr                =   NULL;

  int noTempScales                      =   noScales;

  /**Dimensions of the rectangular region around the center of the reference
   and the target used for aggregation**/
  double overlapWidthRef                =   0;
  double overlapHeightRef               =   0;
  double overlapWidth                   =   0;
  double overlapHeight                  =   0;


  if (verbose){
    printf("Performing aggregation of weights...\n");
  }
  while(listPtr!=NULL){
    if (verbose){
      printf("%i of %i Scales Completed...\r",noScales-noTempScales,noScales);
    }
    noTempScales--;
    currentScale                        =   listPtr->scale;
    ROIPtr                              =   listPtr->first;
    overlapWidthRef                     =   CLEAN_UP_MIN_PIXELS+
                                            CLEAN_UP_WIDTH_MARGIN/currentScale;
    overlapHeightRef                    =   CLEAN_UP_MIN_PIXELS+
                                            CLEAN_UP_HEIGHT_MARGIN/currentScale;
    while(ROIPtr!=NULL){
      XCen                              =   ROIPtr->x;
      YCen                              =   ROIPtr->y;
      angle                             =   ROIPtr->angle;
      weight                            =   ROIPtr->weight;

      ROIPtr->XSum                      +=  weight*XCen;
      ROIPtr->YSum                      +=  weight*YCen;
      ROIPtr->cumulWeight               +=  weight;
      ROIPtr->numDetections++;

      ROITempPtr                        =   ROIPtr->next;
      listTempPtr                       =   listPtr;

      for (i=0;i<=CLEAN_UP_SCALE_INC;++i){
        tempScale                       =   listTempPtr->scale;
        overlapWidth                    =   CLEAN_UP_MIN_PIXELS+
                                               CLEAN_UP_WIDTH_MARGIN/tempScale;
        overlapHeight                   =   CLEAN_UP_MIN_PIXELS+
                                              CLEAN_UP_HEIGHT_MARGIN/tempScale;
        while(ROITempPtr!=NULL){
          /**Check to see if the two ROIs overlap and the angle between the two
           lie within a particular margin**/
          if (isOverlapping(ROITempPtr->x,ROITempPtr->y,overlapWidth,
                  overlapHeight,ROITempPtr->angle,XCen,YCen,overlapWidthRef,
                  overlapHeightRef,angle) && absVal(absVal(ROITempPtr->angle)-
                                            absVal(angle))<=CLEAN_UP_ANGLE){
            ROITempPtr->numDetections++;
            ROITempPtr->cumulWeight     +=  weight;
            ROITempPtr->XSum            +=  XCen*weight;
            ROITempPtr->YSum            +=  YCen*weight;
            ROIPtr->numDetections++;
            ROIPtr->cumulWeight         +=  ROITempPtr->weight;
            ROIPtr->XSum                +=  ROITempPtr->weight*ROITempPtr->x;
            ROIPtr->YSum                +=  ROITempPtr->weight*ROITempPtr->y;
          }

          ROITempPtr                    =   ROITempPtr->next;
        }

        listTempPtr                     =   listTempPtr->next;
        if (listTempPtr==NULL)
          break;
        ROITempPtr                      =   listTempPtr->first;
      }

      /**Compute the centroid**/
      if (ROIPtr->cumulWeight!=0){
        ROIPtr->x                       =   ROIPtr->XSum/ROIPtr->cumulWeight;
        ROIPtr->y                       =   ROIPtr->YSum/ROIPtr->cumulWeight;
      }
      ROIPtr                            =   ROIPtr->next;
    }
    listPtr                             =   listPtr->next;
  }
  if (verbose){
    printf("%i Of %i Scales Completed...\n",noScales-noTempScales,noScales);
    msgSeperator();
  }

  listPtr                               =   listFirstPtr;

  double width                          =   0;
  double height                         =   0;
  double cumulWeight                    =   0;
  double numDetections                  =   0;

  /**Boolean indicating whether the current ROI have maximum number of
   detections among all its overlapping detections**/
  bool isMax                            =   true;

  /**List containing ROI considered so far**/
  struct ROIPtrList* ROIListItem        =   NULL;
  struct ROIPtrList* ROITempListItem    =   NULL;
  struct ROIPtrList* ROIListFirst       =   NULL;
  struct ROIPtrList* ROIListLast        =   NULL;

  noTempScales                          =   noScales;

  if (verbose){
    printf("Performing clean up...\n");
  }

  while(listPtr!=NULL){
    if (verbose){
      printf("%i Of %i Scales Completed...\r",noScales-noTempScales,noScales);
    }

    --noTempScales;
    ROIPtr                              =   listPtr->first;

    while (ROIPtr!=NULL){
      if (!ROIPtr->ignore){
        cumulWeight                     =   ROIPtr->cumulWeight;
        numDetections                   =   ROIPtr->numDetections;
        XCen                            =   ROIPtr->x;
        YCen                            =   ROIPtr->y;
        angle                           =   ROIPtr->angle;
        width                           =   ROIPtr->width;
        height                          =   ROIPtr->height;

        isMax                           =   true;

        listTempPtr                     =   listPtr;
        ROITempPtr                      =   ROIPtr->next;

        while(listTempPtr!=NULL){
          while(ROITempPtr!=NULL){

            if (!ROITempPtr->ignore){
              /**Check whether the detections overlap**/
              if (isOverlapping(ROITempPtr->x,ROITempPtr->y,ROITempPtr->width,
                                      ROITempPtr->height,ROITempPtr->angle,XCen,
                                                      YCen,width,height,angle)){
                /**Check whether current ROI is not maximum**/
                if (ROITempPtr->numDetections>=numDetections ||
                                ROITempPtr->cumulWeight>=cumulWeight ||
                                            ROITempPtr->weight>=ROIPtr->weight){

                  isMax                 =   false;
                  ROIPtr->ignore        =   true;
                  /**Clear the list**/
                  if (ROIListFirst!=NULL){

                    ROIListItem         =   ROIListFirst;
                    while(ROIListItem!=NULL){
                      ROITempListItem   =   ROIListItem;
                      ROIListItem       =   ROIListItem->next;
                      free(ROITempListItem);
                    }
                    ROIListFirst        =   NULL;
                    ROIListLast         =   NULL;
                  }

                  break;
                }
                ROIListItem             =   (struct ROIPtrList*)
                                            malloc(sizeof(struct ROIPtrList));
                ROIListItem->ptr        =   ROITempPtr;
                ROIListItem->next       =  NULL;

                if (ROIListFirst==NULL)
                  ROIListFirst          =   ROIListItem;
                else
                  ROIListLast->next     =   ROIListItem;

                ROIListLast             =   ROIListItem;
              }
            }
            ROITempPtr                  =   ROITempPtr->next;

          }
          if (!isMax)
            break;

          listTempPtr                   =   listTempPtr->next;
          if (listTempPtr!=NULL)
            ROITempPtr                  =   listTempPtr->first;
        }
        if (isMax){
          if (ROIListFirst!=NULL){
            ROIListItem                 =   ROIListFirst;
            /**Ignoring all overlapping detections**/
            while(ROIListItem!=NULL){
              ROIListItem->ptr->ignore  =   true;
              ROITempListItem           =   ROIListItem;
              ROIListItem               =   ROIListItem->next;
              free(ROITempListItem);
            }
            ROIListFirst                =   NULL;
            ROIListLast                 =   NULL;
          }
        }
      }
      ROIPtr                            =   ROIPtr->next;
    }
    listPtr                             =   listPtr->next;
  }
  if (verbose){
    printf("%i Of %i Scales Completed...\n",noScales-noTempScales,noScales);
    msgSeperator();
  }
}


/**Function that scans the image for faces**/
void scanner(char *file, char *outFile, bool verbose){
  int i,j,k;
  IplImage* img                         =   cvLoadImage(file);

  int width                             =   img->width;
  int height                            =   img->height;

  CvSize imgSize                        =   {width,height};
  CvSize windowSize                     =   {WINDOW_WIDTH,WINDOW_HEIGHT};
  CvSize croppedSize                    =   {CROPPED_WIDTH,CROPPED_HEIGHT};

  /**Color Corrected Image**/
  IplImage *imgColorCorrected           =   cvCreateImage(imgSize,
                                                  img->depth,img->nChannels);
  cvCopy(img,imgColorCorrected);

  imgColorCorrection(imgColorCorrected);

  /**Image storing skin pixels as white and the rest as black**/
  IplImage *skinImage                   =   cvCreateImage(imgSize,
                                                            IPL_DEPTH_8U,1);
  initSkinRegion(imgColorCorrected,skinImage,verbose);

  cvReleaseImage(&imgColorCorrected);

  /**GrayScale Image**/
  IplImage *grayImg                     =   cvCreateImage(imgSize,
                                                             IPL_DEPTH_8U,1);
  cvCvtColor(img,grayImg,CV_BGR2GRAY);


  /**Image Storing the elliptical mask**/
  IplImage* windowMask                  =   cvCreateImage(windowSize,
                                                            IPL_DEPTH_8U,1);

  maskGenerator(windowMask,MASK_PARAM_X*CROPPED_WIDTH/WINDOW_WIDTH,MASK_PARAM_Y
                                                *CROPPED_HEIGHT/WINDOW_HEIGHT);
   /**Counter of total face windows detected**/
  int faceWindows                       =   0;

  /**Counter of total windows preprocessed**/
  int totalWindows                      =   0;

  /**Weight assigned to the current window**/
  double weight                         =   0;

  /**Maximum weight of all windows feeded to network**/
  double maxWeight                      =   0;

  /**The fraction of skin pixels present in the window**/
  double skinAmount                     =   0;

  double angleRatio;

  /**The center of the current window**/
  double XCen                           =   WINDOW_WIDTH/2;
  double YCen                           =   WINDOW_HEIGHT/2;

  /**The angle of rotation in radians of the current window**/
  double angle                          =   0;

  /**No of scales completed**/
  int noScales                          =   0;

  /**Scaled dimensions of the image**/
  int scaledWidth                       =   width;
  int scaledHeight                      =   height;

  /**X,Y limit upto which windows are scanned**/
  int scanX                             =   0;
  int scanY                             =   0;

  /**Current scaling factor used**/
  double currentScale                   =   1;

  CvRect cropArea                       =   cvRect(0,0,WINDOW_WIDTH,
                                                        WINDOW_HEIGHT);

  IplImage  *currentWindow,
            *croppedWindow,
            *skinCropWindow,
            *scaledImg,
            *scaledSkinImg;

  /**Linked list storing the ROIs for the current scale**/
  struct list *currentScaleList         =   NULL;

  /**First Element in the list of scales**/
  struct list *listFirstPtr             =   NULL;

  /**Last Element in the list of scales**/
  struct list *listLastPtr              =   NULL;

  /**ROI of the current window**/
  struct ROI *curROI                    =   NULL;

  /**Check whether scaled image is having dimensions greater than scan window**/
  while(scaledWidth>=WINDOW_WIDTH && scaledHeight>=WINDOW_HEIGHT ){

    ++noScales;
    currentScaleList                    =   (list*)malloc(sizeof(struct list));

    if ( listFirstPtr!=NULL ){
      listLastPtr->next                 =   currentScaleList;
    }
    else{
      listFirstPtr                      =   currentScaleList;
    }

    listLastPtr                         =   currentScaleList;

    currentScaleList->next              =   NULL;
    currentScaleList->first             =   NULL;
    currentScaleList->last              =   NULL;
    currentScaleList->scale             =   currentScale;

    scanX                               =   scaledWidth-WINDOW_WIDTH;
    scanY                               =   scaledHeight-WINDOW_HEIGHT;

    imgSize.width                       =   scaledWidth;
    imgSize.height                      =   scaledHeight;
    scaledImg                           =   cvCreateImage(imgSize,
                                                            IPL_DEPTH_8U,1);
    scaledSkinImg                       =   cvCreateImage(imgSize,
                                                            IPL_DEPTH_8U,1);
    cvResize(grayImg,scaledImg);
    cvResize(skinImage,scaledSkinImg);

    for (i=0;i<=scanY;i+=SCAN_STEP_SIZE_Y){

      for (j=0;j<=scanX;j+=SCAN_STEP_SIZE_X){
        cropArea.x                      =   j;
        cropArea.y                      =   i;
        skinCropWindow                  =   cropImage(scaledSkinImg,&cropArea);
        skinAmount                      =   getSkinAmount(skinCropWindow);

        if (skinAmount>=MIN_SKIN_THRESHOLD && skinAmount<=MAX_SKIN_THRESHOLD){
          ++totalWindows;
          currentWindow                 =   cropImage(scaledImg,&cropArea);

          lightingCorrection (currentWindow,windowMask);


          if (ROTATION_TYPE==0){
            croppedWindow               =   windowRotate(currentWindow,&angle,
                                                      &croppedSize,windowMask);
          }else{
            croppedWindow               =   windowRotate2(currentWindow,&angle,
                                                      &croppedSize,windowMask);
          }
          weight                        =   simNetwork(croppedWindow);
          if (DO_ANGLE_THRESHOLD){
            angleRatio                  =   getCorrelationRatio(currentWindow,
                                                &XCen,&YCen,-angle,windowMask);
          }


          if(weight>WEIGHT_THRESHOLD && (!DO_ANGLE_THRESHOLD ||
                                            angleRatio<=ANGLE_RATIO_THRESHOLD)){

            if (weight>maxWeight){
              maxWeight                 =   weight;
            }
            ++faceWindows;
            curROI                      =   (struct ROI*)malloc(sizeof(struct ROI));
            curROI->x                   =   (j+XCen)/currentScale;
            curROI->y                   =   (i+YCen)/currentScale;
            curROI->XSum                =   0;
            curROI->YSum                =   0;
            curROI->width               =   CROPPED_WIDTH/currentScale;
            curROI->height              =   CROPPED_HEIGHT/currentScale;
            curROI->angle               =   angle;
            curROI->weight              =   weight;
            curROI->cumulWeight         =   0;
            curROI->numDetections       =   0;
            curROI->next                =   NULL;
            curROI->ignore              =   false;
            if (listLastPtr->first==NULL){
              listLastPtr->first        =   curROI;
            }
            else{
              listLastPtr->last->next   =   curROI;
            }
            listLastPtr->last           =   curROI;
          }
          cvReleaseImage(&currentWindow);
          cvReleaseImage(&croppedWindow);

        }
        cvReleaseImage(&skinCropWindow);
      }
      if (scanY!=0 && verbose){
        printf("%i%% Complete, Scale: %0.2lf, Faces Found: %i\r",(i*100)/scanY,
                                                      currentScale,faceWindows);
      }
    }
    if (verbose){
      printf("100%% Complete, Scale: %0.2lf, Faces Found: %i\n",currentScale,
                                                                   faceWindows);
    }
    currentScale                        /=  SCALING_FACTOR;
    scaledWidth                         =   cvRound(width*currentScale);
    scaledHeight                        =   cvRound(height*currentScale);

    cvReleaseImage(&scaledImg);
    cvReleaseImage(&scaledSkinImg);
  }
  if (verbose){
    printf("\nFace Windows: %i,Scanned Windows: %i\n",faceWindows,totalWindows);
    msgSeperator();
  }

  if (DO_CLEANUP){
    cleanUp(listFirstPtr,noScales,verbose);
  }

  /**List and ROI Pointers to scan through the probable face ROIs**/
  struct list *listPtr                  =   listFirstPtr;
  struct list *listTempPtr              =   NULL;
  struct ROI *ROIPtr                    =   NULL;
  struct ROI *ROITempPtr                =   NULL;

  /**Represent the face locations with squares**/
  while (listPtr!=NULL){

    ROIPtr                              =   listPtr->first;
    while(ROIPtr!=NULL){
      if (!DO_CLEANUP || (!ROIPtr->ignore &&
                    ROIPtr->numDetections>=CLEAN_UP_MIN_NUM_DETECTION)){
        drawRectangle(img,ROIPtr,maxWeight);
      }
      ROITempPtr                        =   ROIPtr;
      ROIPtr                            =   ROIPtr->next;
      free(ROITempPtr);
    }
    listTempPtr                         =   listPtr;
    listPtr                             =   listPtr->next;
    free(listTempPtr);
  }

  FILE* tempFile                        =   fopen(outFile,"w");
  fclose(tempFile);

  if (verbose){
    printf("Saving Image %s...\n",outFile);
    msgSeperator();
  }
  cvSaveImage(outFile,img);

  cvReleaseImage(&grayImg);
  cvReleaseImage(&skinImage);
  cvReleaseImage(&img);
  cvReleaseImage(&windowMask);
}

/**Function to parse the arguments given to the program**/
void parseInput(int argc, char** argv){
  int i;

  /**Remove the program name**/
  --argc;
  ++argv;

  bool verbose                              =   false;

  /**Structure to store file Details. Defined in io.h**/
  struct _finddata_t fileDetails;
  long fileHandle;

  /**Temporary variable to hold the full path name**/
  char fullPath[FILENAME_MAX];

  /**Output Directory**/
  char outDir[FILENAME_MAX+1]               =   "faceDetect\\";

  /**Array of files to process. Index 0 stores the file name and index 1
   stores the file path**/
  char filesFound[MAX_NO_FILES][2][FILENAME_MAX];

  int noFiles                               =   0;

  /**Valid options**/
  char option1[]                            =   "-o";
  char option2[]                            =   "-v";
  char option3[]                            =   "-h";

  while (argc){
    /**If argument is an option**/
    if (argv[0][0]=='-'){
      /**Output directory option**/
      if (stricmp(option1,argv[0])==0){
        ++argv;
        --argc;
        if (argc>0){
          if (strlen(argv[0])>FILENAME_MAX){
            printf("Error: Output Directory length exceeds %i\n",FILENAME_MAX);
            exit(EXIT_FAILURE);
          }
          if (argv[0][0]=='-'){
            printf("Error: No output directory specified\n");
            exit(EXIT_FAILURE);
          }
          strcpy (outDir,argv[0]);
        }
        else{
          printf("Error: No output directory specified\n");
          exit(EXIT_FAILURE);
        }
      }
      /**Verbose option**/
      else if(stricmp(option2,argv[0])==0){
          verbose                           =   true;
      }
      /**Help option**/
      else if(stricmp(option3,argv[0])==0){
        msgHelp();
        exit(EXIT_SUCCESS);
      }
      /**Invalid option**/
      else{
        printf("Error: Invalid option %s\n",argv[0]);
        exit(EXIT_FAILURE);
      }
    }
    else{
      /**Find the first file matching the given file expression**/
      if (fileHandle=_findfirst(argv[0],&fileDetails)!=-1){
        do{
          if (getPath(argv[0],fileDetails.name,fullPath)&&noFiles<MAX_NO_FILES){
             strcpy(filesFound[noFiles][0],fileDetails.name);
             strcpy(filesFound[noFiles++][1],fullPath);
          }
          else if (noFiles>=MAX_NO_FILES){
            printf("Warning: Exceeded file limit. Skipping %s\n",fullPath);
          }
          else{
            printf("Warning: Invalid image file. Skipping %s\n",fullPath);
          }
        }
        while (_findnext(fileHandle,&fileDetails)!=-1);
      }
      else{
        printf("Error: No file matching %s found\n",argv[0]);
        exit(EXIT_FAILURE);
      }
    }
    ++argv;
    --argc;
  }

  if (noFiles>0){
    msgPreamble();

    if (verbose){
      msgConstants();
    }
    getch();
    initNetworks(verbose);
    makeDir(outDir);

    for (i=0;i<noFiles;++i){

      printf("Processing file %s (%i of %i)...\n",
                                    filesFound[i][1],i+1,noFiles);
      getPath(outDir,filesFound[i][0],fullPath);
      scanner(filesFound[i][1],fullPath,verbose);
    }
    cleanUpNetwork();
  }
  else{
    printf("Error: No file to process\n");
    initNetworks(true);
    scanner("../test/test40.jpg","test402.jpg",true);
    cleanUpNetwork();
  }
}

/**Function for testing the performance of network**/
void tester(){
  const int weightStartFile             =   36;
  const int weightEndFile               =   63;
  const int noFaces[3]                  =   {1760,1760,1760};
  const int totalNoFaces                =   5280;

  int i,j,k,count;
  double weight;
  char file[100];
  /**Consider each weight file**/
  for (i=weightStartFile;i<=weightEndFile;++i){
    sprintf(WEIGHTS_FILE[0][0],"../netnew/t3/weights/T%iL1.txt",i);
    sprintf(WEIGHTS_FILE[0][1],"../netnew/t3/weights/T%iL2.txt",i);
    initNetworks(false);
    count=0;
    /**Test the network with the face DB**/
    for (k=1;k<=3;++k){
      for (j=1;j<=noFaces[k-1];++j){
        sprintf(file,"../db/faces%i/%04i.jpg",k,j);
        printf("%s\r",file);
        weight                          =   simNetwork(cvLoadImage(file,
                                                CV_LOAD_IMAGE_GRAYSCALE));
        if (weight!=0)
          ++count;
      }
    }
    printf("\nWeight File : %i True Face Detections : %lf%%\n",i,
                                                count*100.0/totalNoFaces);
  }
}

int main(int argc, char** argv){
  //tester();
  parseInput(argc,argv);

  return 0;
}
