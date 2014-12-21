#ifndef LAB1231_SUN_PRJ_UTIL_H_
#define LAB1231_SUN_PRJ_UTIL_H_

#include <string>
#include <fstream>
#include <stdlib.h>
#include <sstream>
// #include <cstdint>
#include <iostream> 
#include <opencv2/core/core.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <Eigen/Dense>


namespace lab1231_sun_prj {
  namespace util {

    typedef 
    std::map<std::string, double> 
    EnergyParam;

    typedef 
    std::map<std::string, std::string> 
    DataParam;

    typedef
    std::vector<size_t>
    Superpixel;

/*!
 * @brief
 */
template<typename MatType>
void csv_write(const MatType m, const std::string csv_path) {
  using namespace std;
  using namespace boost;

  ofstream csv;
  csv.open(csv_path.c_str());
  BOOST_ASSERT_MSG(csv.is_open(), string("ERROR: Cannot open: " + csv_path).c_str());

  for(uint64_t  i=0; i<m.rows(); i++){
    for(uint64_t j=0; j<m.cols(); j++){
      string str = lexical_cast<std::string>(m(i,j));
      if(j+1 == m.cols()) csv << str;
      else csv << str << ",";
    }
    csv << "\n";
  }

  csv.close();  
}

/*!
 * @brief
 */
template<typename MatType>
MatType csv_read(const std::string& csv_path) {
  // Adopted from: https://gist.github.com/stephenjbarr/2266900
  using namespace std;
  using namespace boost;
  
  ifstream csv(csv_path);
  BOOST_ASSERT_MSG(csv.is_open(), string("ERROR: Cannot find: " + csv_path).c_str());
  
  typedef tokenizer< escaped_list_separator<char> > Tokenizer;
  
  vector< string > vec;
  string line;
  vector< vector< string > > matrows;
  
  while (getline(csv,line)) {
    Tokenizer tok(line);
    vec.assign(tok.begin(),tok.end());
    matrows.push_back(vec);
  }
  csv.close();
  
  // FIGURE OUT HOW MANY OF THE ROWS HAVE THE RIGHT NUMBER
  // OF COLUMNS
  int Nrows = matrows.size();
  int Ncols = matrows[0].size();
  int Ngoodrows = 0;
  for(int i = 0; i < Nrows; i++) {
    if(matrows[i].size() == Ncols) {
      Ngoodrows++;
    }
  }
  
  // TRANSFORM THE VECTOR OF ROWS INTO AN EIGEN DOUBLE MATRIX
  MatType xmat = MatType(Ngoodrows, Ncols);
  //cout << "INPUT MATRIX: " << Nrows << "x" << Ncols << endl;
  int rc = 0;
  for(int i = 0; i < Nrows; i++) {
    int rowsize = matrows[i].size();
    if(rowsize != Ncols) {
      cout << "WARNING: Row " << i << " has bad column count" << endl;
      continue;
    } 
    
    for(int j = 0; j < Ncols; j++) {
      xmat(rc,j) = int( round(strtod(matrows[i][j].c_str(), NULL)) );
      xmat(rc,j) = lexical_cast<double>(matrows[i][j]);
    }
    rc++;
  }
  
  return xmat;
}




/*!
 * @brief
 */
 size_t n_pairwise(const size_t& n_row, const size_t& n_col, const std::string& type);

/*!
 * @brief get the one dim vector idx
 */
 size_t flat_idx(const size_t& x, const size_t& y, const size_t& n_col);

/*!
 * @brief
 */
 size_t get_row(size_t index, size_t n_cols);

/*!
 * @brief
 */
 size_t get_col(size_t index, size_t n_cols);

/*!
 * @brief calculate the L1-norm: || xi - xj ||
 */
 double l1_norm(cv::Vec3b p1, cv::Vec3b p2);

/*!
 * @brief
 */
 double l1_norm_squared(cv::Vec3b p1, cv::Vec3b p2);

/*!
 * @brief read list containing one entity per line
 */
 std::vector<std::string> read_list(const std::string& list_path);

/*!
 * @brief
 */
 Eigen::MatrixXi arr2mat(int* arr, const size_t& n_row, const size_t& n_col);

/*!
 * @brief
 */
std::vector<Superpixel> load_superpixel(const std::string& list_path);

/*!
 * @brief
 */
Eigen::MatrixXi arr2mat(int* arr, const size_t& n_row, const size_t& n_col);

/*!
 * @brief
 */
 cv::Mat ann2img(const Eigen::MatrixXi& ann, const std::string& dataset);


 float energy_probability(double probability);



}// namespace util
} // namespace lab1231_sun_prj
#endif