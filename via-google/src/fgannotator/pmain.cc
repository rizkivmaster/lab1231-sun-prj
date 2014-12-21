#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <shotton/shotton.h>
// #include <util/util.h>
#include <QVector>
#include <QStringList>
#include <QString>
#include <QRgb>
#include <QPair>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QImage>
#include <QtGlobal>
#include "../textonboost/src/settings.h"

#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>



  using namespace std;
  using namespace lab1231_sun_prj;

void png_write(Eigen::MatrixXi m, QString png_path,size_t label);
void png_write(Eigen::MatrixXi m, QString png_path);


void makeAnnotation(const int n_label, const QString img_path,const QString unary_path,const QString png_path, const int object_label)
{
  shotton::EnergyParam energy_param;
  shotton::train("VOC", &energy_param);
  Eigen::MatrixXi ann = shotton::annotate(n_label, qPrintable(img_path), qPrintable(unary_path), energy_param,object_label);
  png_write(ann,qPrintable(png_path),object_label);
}

// #ifdef USE_TBB
  class TBBAnnotate{
    const int n_label;
    const QVector< QPair< QString, QString > > & names;
    const QString & image_directory;
    const QString & unary_directory;
    const QString & png_directory;
  public:
    TBBAnnotate(const int n_label, QVector< QPair< QString, QString > > & names, QString & image_directory,QString & unary_directory,QString & png_directory):n_label(n_label),names(names),image_directory(image_directory),unary_directory(unary_directory),png_directory(png_directory){}
    void operator()( tbb::blocked_range<int> rng) const{
      for(int i = rng.begin();i<rng.end();i++){
        qDebug("Annotating image paralelly %d %s",i, qPrintable(names[i].first));
        const int object_label = names[i].first.toInt();
        const QString image_path = image_directory + "/" + names[i].second + ".jpg";
        const QString unary_path = unary_directory +"/" + names[i].second + ".unary";
        const QString png_path = png_directory + "/" + names[i].second + ".png";
        qDebug("Write %d to %s",object_label,qPrintable(png_path));
        makeAnnotation(n_label,image_path,unary_path,png_path,object_label);
      }
    }
  };
  void annotate_all(const size_t n_label, QVector< QPair< QString, QString > > & names, QString & image_directory,QString & unary_directory,QString & png_directory){
    tbb::parallel_for(tbb::blocked_range<int>(0,names.size(),1), TBBAnnotate(n_label,names,image_directory,unary_directory,png_directory));
  }

// #else
//   void annotate_all(const size_t n_label, QVector< QString > & names, QString & image_directory,QString & unary_directory,QString & png_directory, const size_t object_label){
//     for(int i = 0; i < names.size();i++)
//     {
//       qDebug("Annotating image serially %d",i);
//       const QString image_path = image_directory + "/" + names[i] + ".jpg";
//       const QString unary_path = unary_directory +"/"+"2007_000039.unary";
//       const QString png_path = png_directory + "/" + names[i] + ".png";
//       qDebug("Image name: %s",qPrintable(image_path));
//       qDebug("UNary name: %s",qPrintable(unary_path));
//       qDebug("png name: %s",qPrintable(png_path));
//       makeAnnotation(n_label,image_path,unary_path,png_path,object_label);
//     }
//   }  

// #endif


void png_write(Eigen::MatrixXi m, QString png_path,size_t label) {

  QString colorfile = VOC2010_COLOR_PATH.c_str();
  QVector<QRgb> colorTable;
  QFile file(colorfile);
  if (!file.open(QFile::ReadOnly))
              qFatal( "Failed to load '%s'", qPrintable( colorfile ) );
  QDataStream s(&file);
  s >> colorTable;
  file.close();

  QImage targetPNG(m.cols(),m.rows(),QImage::Format_Indexed8);
  
  targetPNG.setColorTable(colorTable);
  for(int  ii=0; ii<m.rows(); ii++){
    for(int jj=0; jj<m.cols(); jj++){
      if(m(ii,jj)==1) targetPNG.setPixel(jj,ii,label);
      if(m(ii,jj)==0) targetPNG.setPixel(jj,ii,255);
    }
  }
  targetPNG.save(png_path,"png",0);
}

void png_write(Eigen::MatrixXi m, QString png_path) {

  QString colorfile = VOC2010_COLOR_PATH.c_str();
  QVector<QRgb> colorTable;
  QFile file(colorfile);
  if (!file.open(QFile::ReadOnly))
              qFatal( "Failed to load '%s'", qPrintable( colorfile ) );
  QDataStream s(&file);
  s >> colorTable;
  file.close();


  QImage targetPNG(m.cols(),m.rows(),QImage::Format_Indexed8);
  targetPNG.setColorTable(colorTable);
  for(int  ii=0; ii<m.rows(); ii++){
    for(int jj=0; jj<m.cols(); jj++){
      targetPNG.setPixel(jj,ii,m(ii,jj));
    }
  }
  targetPNG.save(png_path,"png",0);
}


QVector< QPair< QString,QString > >  listImageNames(QString path, QString JPEGDirectory, QString UnaryDirectory)
{
  QVector< QPair< QString,QString > > names;
  QFile f( path );
  if( !f.open( QFile::ReadOnly ) )
    qFatal( "Failed to open file %s", qPrintable( path ) );

  while( !f.atEnd() ){
    QByteArray filename = f.readLine().trimmed();
    if(filename.length() > 0){
      QString namestring = filename;
      QStringList idname = namestring.split(",");
      QString name = idname[1].split(".")[0];
      QString jpgname = JPEGDirectory+"/"+name+".jpg";
      QString unaryname = UnaryDirectory+"/"+name+".unary";//name+".unary";
      if( !QFile::exists( jpgname ) )qWarning( "File not found %s" , qPrintable( jpgname ) );
      if( !QFile::exists( unaryname ) ) qWarning( "File not found %s", qPrintable( unaryname ) );
      
      QPair<QString, QString> pair;
      pair.first = idname[0];
      pair.second = name;
      names.append( pair );
    }
  }
  return names;
}

int main(int argc, char* argv[]) {

  // assert(argc==6 && "argc!=6");

  // Set the dataset param
  // const string datasets_name = argv[1];
  const size_t n_label = atoi(argv[1]);
  QString list_path = argv[2];
  QString img_dir = argv[3];
  QString unary_dir = argv[4];
  QString png_dir = argv[5];

  QVector< QPair< QString,QString > > image_names = listImageNames(list_path,img_dir,unary_dir);

  annotate_all(n_label,image_names,img_dir,unary_dir,png_dir);


  // Train
  // shotton::EnergyParam energy_param;
  // shotton::train(datasets_name, &energy_param);
  // Eigen::MatrixXi ann = shotton::annotate(n_label, img_dir, unary_dir, energy_param,object_label);
  // png_write(ann,png_dir,object_label);


  /*for (size_t i=0; i<ann.rows(); ++i) {
    for (size_t j=0; j<ann.cols(); ++j) {
      printf("%d",ann(i,j));
    }
  }*/
  // util::csv_write(ann, ann_dir);
  // cv::Mat output = util::ann2img(ann,"msrc");
  // imwrite(ann_img_dir,output);
    // png_write(ann,png_dir);


  // Annotate
  /*vector<string> test_img_filenames;
  
  ifstream test_img_list_file(data_param["test_img_list_filepath"].c_str());
  if (test_img_list_file.is_open()) {
    string line;
    while ( getline(test_img_list_file,line) ) {
      test_img_filenames.push_back(line);
    }
    test_img_list_file.close();
  }*/

  // const string ann_results_dir = argv[5];//"/home/tor/sun4/exp/rep-shotton-msrc/ann-csv/";
  // std::vector<Eigen::MatrixXi> ann_results;

/*  for (size_t i=0; i<test_img_filenames.size(); ++i) {
    const string img_filename = test_img_filenames.at(i);
    cout << "ANNOTATING img_filename" << img_filename << endl;
*/
    
    
    // ann_results.push_back(ann);
    /*break;
  }*/

  return 0;
}