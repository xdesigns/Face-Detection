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
 **   This header file defines all the constants and structures.             ***
 *******************************************************************************
 ******************************************************************************/


/**Final window dimesions feeded to the neural network. DO NOT CHANGE**/
#define CROPPED_WIDTH                       20
#define CROPPED_HEIGHT                      20

/**Scanned window dimensions. The window is feeded for lighting correction
 followed by rotation correction and the image is cropped around the image
 center with cropped window dimensions**/
#define WINDOW_WIDTH                        29
#define WINDOW_HEIGHT                       29

/**The parameter used in adjusting amount of linear gradient lighting correction
 performed.**/
#define LIGHTING_CORRECTION_FACTOR          0.4

/**Factor by which the X and Y axis of the mask ellipse is multiplied**/
#define MASK_PARAM_X                        1.0
#define MASK_PARAM_Y                        1.0

/**Amount by which RGB colors are incremented or decremented for color
 correction of the input image**/
#define COLOR_FACTOR                        10u

/**Algorithm used in finding the rotation angle of the face. Type 0 uses
 PCA while any other type uses statistical correlation/symmetry methods**/
#define ROTATION_TYPE                       0

/**Determine whether to use symmetry or correlation for rotation algorithm 2**/
#define USE_SYMMETRY                        0

/**Pixels by which symmetry scanner is incremented**/
#define SYMMETRY_INC_X                      1
#define SYMMETRY_INC_Y                      1


/**Pixels by which correlation scanner is incremented**/
#define CORRELATION_INC_X                   2
#define CORRELATION_INC_Y                   2


/**Range of rotated axes used for finding the alignment of the face in
 correlation / symmetry based window rotation**/
#define ROTATION_START_ANGLE                -M_PI/2
#define ROTATION_SWEEP_ANGLE                M_PI


/**Step size in degrees for correlation / symmetry checking. Step size initally
 starts with the mininum angle increment. Whenever correlation / symmetry
 decreases as the axes is rotated, the angle increment is increased by rotation
 multiplication factor upto a maximum limit given by maximum angle increment.
 If it increases, the angle increment is reduced by the same factor.**/
#define ROTATION_MIN_ANGLE_INC              2.0
#define ROTATION_MAX_ANGLE_INC              8.0

/**Factor by which angle step size is increased (decreased) when correlation/
 symmetry decreases (increases)**/
#define ROTATION_MUL_FACTOR                 2.0

/**Architecture of the neural network used. DO NOT CHANGE**/
#define NUM_NEURONS_LAYER_1                 26
#define NUM_NEURONS_LAYER_2                 1

/**Number of networks used for arbitration**/
#define NUM_NETWORKS                        1

/**Array containing paths to files containing layer 1 and 2 weights**/
char WEIGHTS_FILE[NUM_NETWORKS][2][100] =   {
                {"../netnew/t3/weights/T59L1.txt",
                 "../netnew/t3/weights/T59L2.txt"
                },
};

/**Thresholds for each networks used in distingushing face from non-face.
 Must be <1 and >-1 since tanh is used as activation function for the
 network**/
const double NETWORK_THRESHOLD[NUM_NETWORKS]
                                        =   {0.9};

/**Factor by which the network Probability is multiplied for each network
 for calculating the final weight for a window**/
const double NETWORK_MUL_FACTOR[NUM_NETWORKS]
                                        =   {1.0};

/**Whether angle ratio thresholding should be performed**/
#define DO_ANGLE_THRESHOLD                  1

/**Maximum correlation ratio between horizontal axis and vertical axis for
 a window to be considered as face**/
#define ANGLE_RATIO_THRESHOLD               1.46

/**Final weight threshold used in distingushing face from non face**/
#define WEIGHT_THRESHOLD                    0

/**Pixel increment in X and Y directions during scanning**/
#define SCAN_STEP_SIZE_X                    1
#define SCAN_STEP_SIZE_Y                    1

/**Min fraction of skin pixels required in each window. Must be >0 & <1. If the
 amount is less than this fraction, the window is ignored.**/
#define MIN_SKIN_THRESHOLD                  0.8

/**Max fraction of skin pixels for a window to be considered as a face**/
#define MAX_SKIN_THRESHOLD                  0.9875

/**Factor by which the image is scaled during each scanning. Scaling is
 carried until image dimensions is reduced below WINDOW_WIDTH or
 WINDOW_HEIGHT**/
#define SCALING_FACTOR                      1.2

/**Whether clean up should be performed**/
#define DO_CLEANUP                          1

/**Neighbouring scales checked for aggregating face weights**/
#define CLEAN_UP_SCALE_INC                  3

/**Min dimension of the square considered around the center of a window for
 asserting overlap with similar regions around nearby windows**/
#define CLEAN_UP_MIN_PIXELS                 3.0

/**Number of pixels that are added to the above square after scale
 adjustment. This margin is divided by the corresponding scale and added to
 the minimum number of pixels**/
#define CLEAN_UP_WIDTH_MARGIN               1.0
#define CLEAN_UP_HEIGHT_MARGIN              1.0

/**Max angle difference between the two aggregating windows**/
#define CLEAN_UP_ANGLE                      10*M_PI/180

/**Minimum number of nearby detection required for a window to be considered
 as face**/
#define CLEAN_UP_MIN_NUM_DETECTION          3

/**Edge Thickess in pixels for the final squares**/
#define LINE_THICKESS                       2

/**Factor determining the color intensity of the final squares. Greater factor
 signify greater determination**/
#define COLOR_DETERMINE_FACTOR              10

/**Color(BGR) used in drawing the final squares**/
CvScalar FACE_COLOR                       =   {0,0,255};


/**Maximum number of files that should be processed at once**/
#define MAX_NO_FILES                        20

#define NO_VALID_EXT                        7

const char VALID_EXT[NO_VALID_EXT][6]   =   {   ".jpg",
                                                ".jpeg",
                                                ".bmp",
                                                ".png",
                                                ".gif",
                                                ".tif",
                                                ".tiff"
                                            };



/**Region Of Interest(ROI) for each window**/
struct ROI{
  /**Coordinates of center of ROI**/
  double x,y;

  /**Sum of X and Y coordinates of nearby windows. Used in calculating centroid
   of the resultant window**/
  double XSum,YSum;

  /**Scaled width and height of the window**/
  double width,height;

  /**Angle of Rotation of the window**/
  double angle;

  /**Weight assigned to this window. Calculated by summing the factor
   obtained by NETWORK_MUL_FACTOR*NETWORK_THRESHOLD for each networks**/
  double weight;

  /**Aggregation of weights of overlapping windows with this window**/
  double cumulWeight;

  /**Number of neighbouring windows assigned during clean up**/
  int numDetections;

  /**Boolean indicating whether to ignore this window**/
  bool ignore;

  /**Pointer to next ROI of the same scale**/
  struct ROI* next;
};

/**Linked List structure defined to store the ROIs for different scales**/
struct list{
  /**Pointer to first ROI**/
  struct ROI* first;

  /**Pointer to last ROI**/
  struct ROI* last;

  /**Scaling factor representing the list**/
  double scale;

  /**Pointer to the next element of a lower scale in the list**/
  struct list* next;
};

/**Linked List for keeping a list of ROI items**/
struct ROIPtrList{
  /**Pointer to the ROI Element**/
  struct ROI* ptr;
  /**Pointer to the next item**/
  struct ROIPtrList* next;
};
