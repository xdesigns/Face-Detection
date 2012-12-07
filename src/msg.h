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
 **   This header file contain functions to print info messages.             ***
 *******************************************************************************
 ******************************************************************************/


inline void msgSeperator(){
  printf("*****************************************************************\n");
}

inline void msgPreamble(){
  msgSeperator();
  printf("              Neural Network Based Face Detector\n");
  printf("            Copyright (C) 2010  All Rights Reserved\n");
  printf("                   Created By: Nijith Jacob\n");
  printf("                   Email:nijith89@yahoo.com\n");
  msgSeperator();

}

inline void msgHelp(){
  printf("Usage:\tfacedetect [-h] [-o output dir] [-v[v]] file [file]*\n");
  printf("\t-h: Displays this help\n");
  printf("\t-o: Output directory to save the final images\n");
  printf("\t-v: Verbose Mode\n");
  printf("\tfile: Input File\n");
}

inline void msgConstants(){
  int i;
  printf("DETECTOR PARAMETERS:-\n");
  msgSeperator();
  printf("LIGHTING CORRECTION FACTOR : %0.2lf\n",LIGHTING_CORRECTION_FACTOR);
  printf("COLOR CORRECTION AMOUNT : %i PIXELS\n",COLOR_FACTOR);
  if (ROTATION_TYPE){
    printf("ROTATION ALGORTHIM : CORRELATION/SYMMETRY\n");
  }
  else{
    printf("ROTATION ALGORTHIM : PCA\n");
  }
  printf("\n");
  printf("NUMBER NETWORKS USED : %i\n",NUM_NETWORKS);
  for (i=0;i<NUM_NETWORKS;++i){
    printf("- NETWORK %i THRESHOLD : %0.2lf\n",i+1,NETWORK_THRESHOLD[i]);
    printf("- NETWORK %i WEIGHT : %0.2lf\n",i+1,NETWORK_MUL_FACTOR[i]);
  }
  printf("\n");
  printf("SCAN STEP SIZE X : %i PIXELS\n",SCAN_STEP_SIZE_X);
  printf("SCAN STEP SIZE Y : %i PIXELS\n",SCAN_STEP_SIZE_Y);
  printf("SCALING FACTOR : %0.2lf\n",SCALING_FACTOR);
  printf("MIN SKIN THRESHOLD : %0.2lf\n",MIN_SKIN_THRESHOLD);
  printf("MAX SKIN THRESHOLD : %0.2lf\n",MAX_SKIN_THRESHOLD);
  printf("\n");
  printf("CLEAN UP SCALE INCREMENT : %i\n",CLEAN_UP_SCALE_INC);
  printf("CLEAN UP MIN PIXELS : %0.2lf pixels\n",CLEAN_UP_MIN_PIXELS);
  printf("CLEAN UP WIDTH MARGIN : %0.2lf\n",CLEAN_UP_WIDTH_MARGIN);
  printf("CLEAN UP HEIGHT MARGIN : %0.2lf\n",CLEAN_UP_HEIGHT_MARGIN);
  printf("CLEAN UP ANGLE : %0.2lf DEGREES\n",CLEAN_UP_ANGLE*180/M_PI);
  printf("CLEAN UP MIN DETECTIONS : %i\n",CLEAN_UP_MIN_NUM_DETECTION);
  printf("\n");
  printf("MAX NUMBER FILES : %i\n",MAX_NO_FILES);
  printf("VALID IMAGE EXTENSIONS : ");
  for (i=0;i<NO_VALID_EXT;++i){
    printf("%s,",VALID_EXT[i]);
  }
  printf("\n");
  msgSeperator();
}
