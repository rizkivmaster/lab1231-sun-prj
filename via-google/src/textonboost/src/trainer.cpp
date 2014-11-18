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

#include "util/colorimage.h"
#include "util/colorconvertion.h"
#include "util/labelimage.h"
#include "util/util.h"
#include "feature/texton.h"
#include "feature/filterbank.h"
#include "feature/colorfeature.h"
#include "settings.h"
#include <QVector>
#include <QString>
#include "feature/locationfeature.h"
#include "feature/hogfeature.h"
#include "feature/bboxfeature.h"
#include "classifier/textonboost.h"


//this program creates texton parameters and booster


    int main( int argc, char * argv[]){
//need folder of parameters
      //set thou folder to put all your parameter
      QString param_dir = argv[1];
//inner params
  //texton
      int nTextons = 4;
      float filterbank_size = FILTER_BANK_SIZE;
      int n_textons = N_TEXTONS;
      HogFeature::HogFeatureType type = HogFeature::L;
  //textonboost
      int n_rounds = N_BOOSTING_ROUNDS;
      int n_classifiers = N_CLASSIFIERS;
      int n_thresholds = N_THRESHOLDS;
      int subsample = BOOSTING_SUBSAMPLE;
      int min_rect_size = MIN_RECT_SIZE;
      int max_rect_size = MAX_RECT_SIZE;
      qDebug("Hello");

      QSharedPointer<Feature> filter[4] = {QSharedPointer<Feature>( new FilterBank( filterbank_size ) ),QSharedPointer<Feature>( new ColorFeature() ),QSharedPointer<Feature>( new LocationFeature() ), QSharedPointer<Feature>( new HogFeature(type) )};
      qDebug("Hello");
      QVector< ColorImage > images;
      QVector< Image<float> > lab_images;
      QVector< LabelImage > labels;
      QVector< QString > names;
      QVector< Image<short> > textons;

      qDebug("(train) Loading the database");
      loadImages( images, labels, names, TRAIN );
      qDebug("(train) Converting to Lab");
      lab_images = RGBtoLab( images );

      for(int ii = 0; ii < 4;ii++)
      {
        
        qDebug(QString("(train) Training Textons of Feature %1").arg(ii).toStdString().c_str());
        Texton texton( filter[ii], n_textons );
        texton.train( lab_images, names );
        qDebug(QString("(train) Saving Parameters of Feature %1").arg(ii).toStdString().c_str());
        QString kmeansname = QString(param_dir+"/"+"kmeans%1").arg(ii);
        QString meansname = QString(param_dir+"/"+"means%1").arg(ii);
        QString transname = QString(param_dir+"/"+"trans%1").arg(ii);
        texton.saveParameters(kmeansname, meansname,transname);
        qDebug(QString("(train) Cumulating Parameters of Feature %1").arg(ii).toStdString().c_str());
        QVector< Image<short> > texton_mat = texton.textonize( lab_images, names );
        for( int jj=0; jj<texton_mat.size(); jj++ ){
          printf("int: %d \n",jj);  
          if (jj >= textons.count())
            textons.append( Image<short>(texton_mat[jj].width(), texton_mat[jj].height(), nTextons) );
          for( int kk=0; kk<texton_mat[jj].width()*texton_mat[jj].height(); kk++ )
            textons[jj][kk*nTextons+ii] = texton_mat[jj][kk];
        }
      }

      QString boostername = param_dir+"/"+"booster";
      qDebug("(train) Seat tight! Boosting");
      TextonBoost booster;
      booster.train( textons, labels, n_rounds, n_classifiers, n_thresholds, subsample, min_rect_size, max_rect_size );
      booster.save( boostername );
    }