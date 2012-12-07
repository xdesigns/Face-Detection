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
 **   This header file contains all the supporting math functions.           ***
 *******************************************************************************
 ******************************************************************************/


/**Returns absolute value**/
inline double absVal(double a){
  return a>0?a:-a;
}

/**Returns Max Value**/
inline double maxVal(double a,double b){
  if (a>b)
    return a;
  else
    return b;
}


/**Returns Min Value**/
inline double minVal(double a,double b){
  if (a<b)
    return a;
  else
    return b;
}

/**Function to compute rotation transformation of a point. Positive angle
 is taken in the counter clockwise direction.**/
inline void transformPoint(const double XSrc,const double YSrc,double *XDest,
                 double *YDest,const CvPoint2D64f* center,const double angle){

  double XCen                           =   center->x;
  double YCen                           =   center->y;
  double sinA                           =   sin(angle);
  double cosA                           =   cos(angle);
  *XDest                                =   (XSrc-XCen)*cosA+
                                                    (YSrc-YCen)*sinA+XCen;

  *YDest                                =   (YSrc-YCen)*cosA+
                                                    (XCen-XSrc)*sinA+YCen;
}


/**Function for computing pseudoInverse of a matrix**/
inline void pseudoInverse(const CvMat* A,const CvMat* b,CvMat* dest){

  CvMat *temp                           =   cvCreateMat(A->cols,A->cols,
                                                                CV_64FC1);

  double *tempData                      =   (double*)cvAlloc(sizeof(double)*
                                                            A->cols*A->cols);
  temp->data.db                         =   tempData;

  /**PsuedoInverse = inv(trans(A)*A)*trans(A)*b **/
  cvGEMM(A,A,1.0,NULL,0,temp,CV_GEMM_A_T);

  cvInvert(temp,temp,CV_LU);
  cvGEMM(A,b,1.0,NULL,0,dest,CV_GEMM_A_T);
  cvGEMM(temp,dest,1.0,NULL,0,dest);

  cvReleaseMat(&temp);
  cvFree(&tempData);
}


