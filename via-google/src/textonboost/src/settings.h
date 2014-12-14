/*
    Copyright (c) 2011, Philipp Krähenbühl
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the Stanford University nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Philipp Krähenbühl ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Philipp Krähenbühl BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string>

#pragma once
// TODO: Make sure to adjust the parameters in this file
//#define USE_MSRC
const int CACHE_SIZE = 100;
static const int N_TEXTONS = 50;
static const float FILTER_BANK_SIZE = 1.0;
// Boosting parameters
static const int N_BOOSTING_ROUNDS  = 5; // Number of boosting rounds
static const int N_CLASSIFIERS      = 200; // Number of random classifiers to test [per round]
static const int N_THRESHOLDS       = 100; // Number of thresholds to test [per round]
#ifdef USE_MSRC
static const int BOOSTING_SUBSAMPLE = 5  ; // Subsampling factor (tradeoff between memory/computation and accuracy)
#else
static const int BOOSTING_SUBSAMPLE = 7  ; // Subsampling factor (tradeoff between memory/computation and accuracy)
#endif
static const int MIN_RECT_SIZE      = BOOSTING_SUBSAMPLE; // Minimum size of texton rectangle
static const int MAX_RECT_SIZE      = 200; // Maximum size of texton rectangle
#define GAUSSIAN_OFFSET // Use Sample the offset from a gaussian centered at 0

static const std::string VOC2010_DIRECTORY = "/home/rizkivmaster/ws/datasets/dummy_voc_2010";
static const std::string VOC2010_COLOR_PATH = "/home/rizkivmaster/ws/datasets/dummy_voc_2010/VOC2010.ct";
static const std::string EXPERIMENT_FOLDER = "/home/rizkivmaster/ws/results/11584";
static const std::string VOC2010_PNG_FOLDER = VOC2010_DIRECTORY+"/SegmentationClass";
static const std::string VOC2010_JPG_FOLDER = VOC2010_DIRECTORY+"/JPEGImages";
static const std::string VOC2010_UNARY_FOLDER = EXPERIMENT_FOLDER+"/unaries";
static const std::string TESTING_PNG_FOLDER = EXPERIMENT_FOLDER+"/test_png";
static const int IMAGE_MAX_LENGTH = 250;




















// Shall we return the raw boosting results H or P = 1/Z * exp(-H)
//#define RAW_BOOSTING_OUTPUT
// Dataset parameters
static const char MSRC_DIRECTORY  [] = "/home/rizkivmaster/ws/TextonBoost/msrc/datasets";
static const char MSRC_TRAIN_FILE [] = "/home/rizkivmaster/ws/TextonBoost/msrc/split/Train.txt";
static const char MSRC_VALID_FILE [] = "/home/rizkivmaster/ws/TextonBoost/msrc/split/Validation.txt";
static const char MSRC_TEST_FILE  [] = "/home/rizkivmaster/ws/TextonBoost/msrc/split/Test.txt";
// Dataset parameters
// static const char VOC2010_DIRECTORY  [] = "/home/rizkivmaster/ws/TextonBoost/google/datasets";
static const char VOC2010_TRAIN_FILE [] = "/home/rizkivmaster/ws/TextonBoost/google/split/Train.txt";
static const char VOC2010_VALID_FILE [] = "/home/rizkivmaster/ws/TextonBoost/google/split/Validation.txt";
static const char VOC2010_TEST_FILE  [] = "/home/rizkivmaster/ws/TextonBoost/google/split/Test.txt";
// VOC Cache
static const char VOC2010_BBOX_DIRECTORY [] = "data/VOC2010_BBox/";