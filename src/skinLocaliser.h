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
 **   This header file contain functions for identifying skin pixels         ***
 *******************************************************************************
 ******************************************************************************/

bool isSkin(unsigned char* pixel){
  unsigned char R,G,B;
  /**Extract the RGB pixels**/
  R                                 =   pixel[2];
  G                                 =   pixel[1];
  B                                 =   pixel[0];

  if (R>95 && G>40 &&B>20 && (R>=G&&R>=B?R:(G>=B?G:B))-(R<=G&&R<=B?R:
                                 (G<=B?G:B))>15 && abs(R-G)>15 && R>G && R>B){
    return true;
  }
  if (R>0 && B>0 && G>0 && B*R*R/pow(R+G+B,3)>0.0425 &&
               (R+G+B)/(3*R)+(R-G)/(R+G+B)<=0.9498 && (R*B+G*G)/(G*B)<=2.7775){
    return true;
  }

  return false;
}


