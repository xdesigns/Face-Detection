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
 **   This header file contains all network related functions.               ***
 *******************************************************************************
 ******************************************************************************/

/**Global variable that stores the network weights**/
CvMat* networks[NUM_NETWORKS][2];

/**Global variables storing network outputs**/
CvMat* layer_1_Output;
CvMat* layer_2_Output;

/**Function to load weights for the networks**/
void initNetworks(bool verbose){
  int i,j;
  double* layer_1_Data                  =   NULL;
  double* layer_2_Data                  =   NULL;

  /**20x20 Pixels plus a bias input of 1**/
  int numInputs                         =   CROPPED_WIDTH*CROPPED_HEIGHT+1;

  FILE* weightsFile;

  double scannedWeight;

  for (i=0;i<NUM_NETWORKS;++i){
    /**Define network layer 1 matrices**/
    networks[i][0]                      =   cvCreateMat(NUM_NEURONS_LAYER_1,
                                                        numInputs, CV_64FC1);

    layer_1_Data                        =   (double*)malloc(sizeof(double)*
                                             numInputs*NUM_NEURONS_LAYER_1);

    networks[i][0]->data.db             =   layer_1_Data;

    /**Define network layer 2 matrices**/
    networks[i][1] = cvCreateMat(NUM_NEURONS_LAYER_2,
                                            NUM_NEURONS_LAYER_1+1, CV_64FC1);
    layer_2_Data=(double*)malloc(sizeof(double)*NUM_NEURONS_LAYER_2*
                                            (NUM_NEURONS_LAYER_1+1));

    networks[i][1]->data.db             =   layer_2_Data;

    weightsFile                         =   fopen(WEIGHTS_FILE[i][0],"r");

    /**Initialising layer 1 weights**/
    if (verbose){
      printf("Initialising network %i...\n",i+1);
    }

    for (j=0;j<NUM_NEURONS_LAYER_1*numInputs;++j){
      fscanf(weightsFile,"%lf",&scannedWeight);
      layer_1_Data[j]                   =   scannedWeight;
    }
    fclose(weightsFile);

    weightsFile                         =   fopen(WEIGHTS_FILE[i][1],"r");

    /**Initialising layer 2 weights**/
    for (j=0;j<NUM_NEURONS_LAYER_1+1;++j){
      fscanf(weightsFile,"%lf",&scannedWeight);
      layer_2_Data[j]                   =   scannedWeight;
    }
    fclose(weightsFile);

  }
  if (verbose){
    msgSeperator();
  }
  /**Initalising the network output matrices**/
  layer_1_Output                        =   cvCreateMat(NUM_NEURONS_LAYER_1,1,
                                                                      CV_64FC1);
  double* layer_1_OutputData            =   (double*)malloc(sizeof(double)*
                                                       (NUM_NEURONS_LAYER_1+1));
  /**Bias Input**/
  layer_1_OutputData[NUM_NEURONS_LAYER_1]
                                        =   1;

  layer_1_Output->data.db               =   layer_1_OutputData;

  layer_2_Output                        =   cvCreateMat(NUM_NEURONS_LAYER_2,1,
                                                                      CV_64FC1);
  double* layer_2_OutputData            =   (double*)malloc(sizeof(double)*
                                                           NUM_NEURONS_LAYER_2);
  layer_2_Output->data.db               =   layer_2_OutputData;
}


/**Function to simulate the network**/
double simNetwork(IplImage* input){

  int i,j;
  int vectorSize                        =   CROPPED_WIDTH*CROPPED_HEIGHT+1;

  double weight                         =   0;
  double faceProb                       =   0;

  /**Matrix that stores the vectorized input image**/
  CvMat *inputMat                       =   cvCreateMat(vectorSize,1,CV_64FC1);

  double *inputMatData                  =   (double*) malloc(vectorSize*
                                                               sizeof(double));
  inputMat->data.db                     =   inputMatData;

  unsigned char *imgData                =   (unsigned char*)input->imageData;

  for (i=0;i<vectorSize-1;++i){
    inputMatData[i]                     =   *(imgData+i);
  }
  /**Bias Input**/
  inputMatData[vectorSize-1]            =   1;

  for (i=0;i<NUM_NETWORKS;++i){
    /**Get layer 1 Output**/
    cvMatMul(networks[i][0],inputMat,layer_1_Output);

    /**Include the bias input**/
    layer_1_Output->rows++;

    for (j=0;j<NUM_NEURONS_LAYER_1;++j){
      layer_1_Output->data.db[j]        =   tanh(layer_1_Output->data.db[j]);
    }
    /**Get layer 2 Output**/
    cvMatMul(networks[i][1],layer_1_Output,layer_2_Output);

    layer_1_Output->rows--;

    faceProb                            =   tanh(layer_2_Output->data.db[0]);

    if (faceProb>NETWORK_THRESHOLD[i]){
      weight                            +=  NETWORK_MUL_FACTOR[i]*faceProb;
    }
  }
  cvReleaseMat(&inputMat);
  free(inputMatData);
  return weight;
}

/**Function used to get the weight for a given window and its rotation
 transforms.**/
void getWeight(IplImage* img, double *angle, double *weight){
  double maxWeight;
  double currentWeight;
  double currentAngle;
  CvPoint2D64f center;
  center.x                              =   img->width/2;
  center.y                              =   img->height/2;

  maxWeight                             =   simNetwork(img);
  currentAngle                          =   *angle;

  cvFlip(img,NULL,-1);

  /**Simulate using 180 degrees rotated window**/
  if ((currentWeight=simNetwork(img))>maxWeight){
    maxWeight                           =   currentWeight;
    currentAngle                        =   *angle + M_PI;
  }

  /**If PCA is used**/
  if (ROTATION_TYPE==0){
    IplImage* img90                     =   cvCreateImage(cvGetSize(img),
                                                img->depth,img->nChannels);
    /**Rotate by 270 degrees**/
    transformRotate(img,img90,&center,M_PI/2);
    /**Simulate using 270 degrees rotated window**/
    if ((currentWeight=simNetwork(img90))>maxWeight){
      maxWeight                         =   currentWeight;
      currentAngle                      =   *angle + 3*M_PI/2;
    }

    cvFlip(img90,NULL,-1);

    /**Simulate using 90 degrees rotated window**/
    if ((currentWeight=simNetwork(img90))>maxWeight){
      maxWeight                         =   currentWeight;
      currentAngle                      =   *angle + M_PI/2;
    }
  }

  *weight                               =   maxWeight;
  *angle                                =   currentAngle;
}


/**Clean up funtion**/
void cleanUpNetwork(){
  /**Cleaning up the heap**/
  for (int i=0;i<NUM_NETWORKS;++i){
    free(networks[i][0]->data.db);
    cvReleaseMat(&networks[i][0]);
    free(networks[i][1]->data.db);
    cvReleaseMat(&networks[i][1]);
  }
  free(layer_1_Output->data.db);
  cvReleaseMat(&layer_1_Output);
  free(layer_2_Output->data.db);
  cvReleaseMat(&layer_2_Output);
}
