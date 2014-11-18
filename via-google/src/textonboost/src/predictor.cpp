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
#include <QDir>
#include "classifier/textonboost.h"

#ifdef USE_TBB
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#endif

    void evaluate( const TextonBoost & booster, const Image<short> & texton, const QString & save_file ){
      Image<float> r = booster.evaluate( texton );

  // Save the result
      QFile file( save_file );
      if (file.open( QFile::WriteOnly ) ){
        QDataStream stream( &file );

    // We want to write floats [saves 2x space]
        stream.setVersion( QDataStream::Qt_4_7 );
        stream.setFloatingPointPrecision( QDataStream::SinglePrecision );

        stream << r;
      }
      file.close();

    }

#ifdef USE_TBB
    class TBBEvaluate{
      const TextonBoost & booster;
      const QVector< Image<short> > & textons;
      const QVector<QString> & names;
      const QString & save_dir;
    public:
      TBBEvaluate( const TextonBoost & booster, const QVector< Image<short> > & textons, const QVector<QString> & names, const QString & save_dir ):booster(booster), textons(textons), names(names), save_dir(save_dir){}
      void operator()( tbb::blocked_range<int> rng ) const{
        for( int i=rng.begin(); i<rng.end(); i++ ){
          qDebug("Doing Image %d", i );
          evaluate( booster, textons[i], save_dir + "/" + names[i] + ".unary" );
        }
      }
    };
    void evaluate_all( const TextonBoost & booster, const QVector< Image<short> > & textons, const QVector<QString> & names, const QString & save_dir ){
      tbb::parallel_for(tbb::blocked_range<int>(0, textons.size(), 1), TBBEvaluate(booster, textons, names, save_dir));
    }
#else
    void evaluate_all( const TextonBoost & booster, const QVector< Image<short> > & textons, const QVector<QString> & names, const QString & save_dir ){
      for( int i=0; i<textons.count(); i++ ){
        qDebug("Doing Image %d", i );
        evaluate( booster, textons[i], save_dir + "/" + names[i] + ".unary" );
      }
    }
#endif
//this program creates texton parameters and booster


    int main( int argc, char * argv[]){
//need folder of parameters
      //set thou folder containing parameter
      QString param_dir = argv[1];
      //set thou folder to put thou unaries
      QString save_dir = argv[2];
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
      //evaluation
      int blockSize=BLOCK_SIZE;
      qDebug("WELCOME TO OUR WORLD. GET HIGH!");

      QSharedPointer<Feature> filter[4] = {QSharedPointer<Feature>( new FilterBank( filterbank_size ) ),QSharedPointer<Feature>( new ColorFeature() ),QSharedPointer<Feature>( new LocationFeature() ), QSharedPointer<Feature>( new HogFeature(type) )};
      qDebug("Hello");
      QVector< ColorImage > images;
      QVector< Image<float> > lab_images;
      QVector< LabelImage > labels;
      QVector< QString > names;

      qDebug("Loading the database");      
      QVector< QString > filenames = listVOC2010( TRAIN );

      for( int nn=0; nn<filenames.count(); nn+=blockSize ){
        QVector< QString > cur_names;
        for( int ii=nn; ii<nn+blockSize && ii<filenames.count(); ii++ )
          cur_names.append( filenames[ii] );
        qDebug(QString("Generating unaries for first %1th images").arg(nn+blockSize).toStdString().c_str());
        loadVOC2010byNames(images, labels, names, cur_names);
        qDebug("Converting to Lab");
        lab_images = RGBtoLab( images );          
        QVector< Image<short> > textons;

        for(int ii = 0; ii < 4;ii++)
        {
          Texton texton( filter[ii], n_textons );
          qDebug(QString("Loading Parameters of Feature %1").arg(ii).toStdString().c_str());
          QString kmeansname = QString(param_dir+"/"+"kmeans%1").arg(ii);
          QString meansname = QString(param_dir+"/"+"means%1").arg(ii);
          QString transname = QString(param_dir+"/"+"trans%1").arg(ii);
          if(!QFile::exists( kmeansname))  qWarning( "File not found '%s'", qPrintable( kmeansname) );
          if(!QFile::exists( meansname))  qWarning( "File not found '%s'", qPrintable( meansname) );
          if(!QFile::exists( transname))  qWarning( "File not found '%s'", qPrintable( transname) );
          texton.loadParameters(kmeansname, meansname,transname);
          qDebug("Textonizing");
          QVector< Image<short> > texton_mat = texton.textonize( lab_images, names );
          qDebug(QString("Cumulating Parameters of Feature %1").arg(ii).toStdString().c_str());
          for( int jj=0; jj<texton_mat.size(); jj++ ){
            if (jj >= textons.count())
              textons.append( Image<short>(texton_mat[jj].width(), texton_mat[jj].height(), nTextons) );
            for( int kk=0; kk<texton_mat[jj].width()*texton_mat[jj].height(); kk++ )
              textons[jj][kk*nTextons+ii] = texton_mat[jj][kk];
          }
        }

        qDebug("Evaluating");
        QString boostername = param_dir+"/"+"booster";
        if(!QFile::exists( boostername))  qWarning( "File not found '%s'", qPrintable( boostername) );
        TextonBoost booster;
        booster.load( boostername );

        QDir dir( save_dir );
        if (!dir.exists())
          dir.mkpath( dir.absolutePath() );

        evaluate_all( booster, textons, names, save_dir );
        qDebug(QString("Successfully produced unaries for first %1th images").arg(nn+blockSize).toStdString().c_str());
      }
      qDebug("ALL FINISHED, HAVE A NICE DAY BABY!");
    }