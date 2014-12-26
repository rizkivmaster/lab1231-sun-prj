#include <shotton/shotton.h>
#include <math.h>
#include "../textonboost/src/settings.h"
#define __(x,y) printf("%s %d\n",x,y);
using namespace std;
using namespace lab1231_sun_prj::util;
namespace lab1231_sun_prj
{
namespace shotton
{

void save_image(char *filename, Eigen::MatrixXi m)
{
    QString colorfile = VOC2010_COLOR_PATH.c_str();
    QVector<QRgb> colorTable;
    QFile file(colorfile);
    if (!file.open(QFile::ReadOnly))
        qFatal( "Failed to load '%s'", qPrintable( colorfile ) );
    QDataStream s(&file);
    s >> colorTable;
    file.close();

    QImage targetPNG(m.cols(), m.rows(), QImage::Format_Indexed8);
    targetPNG.setColorTable(colorTable);
    for (int  ii = 0; ii < m.rows(); ii++)
    {
        for (int jj = 0; jj < m.cols(); jj++)
        {
            targetPNG.setPixel(jj, ii, m(ii, jj));
        }
    }
    targetPNG.save(filename, "png", 0);
}

void train(const string datasets_name, EnergyParam *energy_param)
{
    // cout << "train(): BEGIN\n";

    if (strcmp(datasets_name.c_str(), "VOC") == 0)
    {
        // From [Shotton, 2009                                                 ]
        (*energy_param)["theta_phi_1"] = 4.5;
        (*energy_param)["theta_phi_2"] = 1.0;
    }
    else
    {
        assert(false && "Unknown dataset!");
    }

    // cout << "train(): END\n";
}
/*void set_color(QImage png_matrix)
{


}*/

Eigen::MatrixXi annotate(const size_t n_label, const string img_dir, const string unary_dir, EnergyParam energy_param)
{
    //read image file
    cv::Mat img_mat = cv::imread(img_dir, CV_LOAD_IMAGE_COLOR);
    //read unary
    ProbImage unary_matrix;
    unary_matrix.load( unary_dir.c_str() );

    const size_t n_var = img_mat.rows * img_mat.cols;

    GraphicalModel gm( opengm::SimpleDiscreteSpace<size_t, size_t>(n_var, n_label) );

    set_1st_order(img_mat, unary_matrix, n_label, gm);
    set_2nd_order(img_mat, n_label, energy_param, gm);

    Eigen::MatrixXi ann(img_mat.rows, img_mat.cols);
    const string method = "AlphaExpansion";//: "AlphaExpansion", "ICM"
    infer(method, gm, n_var, ann);

    // cout << "annotate(): END\n";
    return ann;
}


//svm without loss
Eigen::MatrixXi annotate(size_t n_label, cv::Mat &image_matrix, ProbImage &unary_matrix, double *unary_weights, double *pair_weights)
{
    EnergyParam energy_param;
    train("VOC", &energy_param);
    size_t n_var = image_matrix.rows * image_matrix.cols;
    GraphicalModel gm( opengm::SimpleDiscreteSpace<size_t, size_t>(n_var, n_label) );
    set_1st_order( image_matrix , unary_matrix, unary_weights, n_label, gm);
    set_2nd_order( image_matrix , n_label, energy_param, pair_weights, gm);
    Eigen::MatrixXi ann(image_matrix.rows, image_matrix.cols);
    infer("AlphaExpansion", gm, n_var, ann);
    return ann;
}

//svm with augmented loss
Eigen::MatrixXi annotateWithAugmentedLoss(size_t n_label, cv::Mat &image_matrix, ProbImage &unary_matrix, double *unary_weights, double *pair_weights, Eigen::MatrixXi &ytrue)
{
    EnergyParam energy_param;
    train("VOC", &energy_param);
    size_t n_var = image_matrix.rows * image_matrix.cols;
    GraphicalModel gm( opengm::SimpleDiscreteSpace<size_t, size_t>(n_var, n_label) );
    set_1st_orderWithLoss( image_matrix , unary_matrix, unary_weights, n_label, gm, ytrue);
    set_2nd_order( image_matrix , n_label, energy_param, pair_weights, gm);
    Eigen::MatrixXi ann(image_matrix.rows, image_matrix.cols);
    infer("AlphaExpansion", gm, n_var, ann);
    return ann;
}

void set_1st_orderWithLoss(cv::Mat img_mat, ProbImage unary_matrix, double *unary_weights, const size_t n_label, GraphicalModel &gm, Eigen::MatrixXi &ytrue)
{
    using namespace std;
    for (size_t x = 0; x < img_mat.cols; ++x)
    {
        for (size_t y = 0; y < img_mat.rows; ++y)
        {
            const size_t shape[] = {n_label};
            opengm::ExplicitFunction<float> energy(shape, shape + 1);

            for (size_t i = 0; i < n_label; i++)
            {
		float hinge_loss = 0;
                //make sure target pixel is not void, void gains zero loss
                if(ytrue(y,x) != 255)
                {
                    //different labels apply
                    if(i!=ytrue(y,x))
                        hinge_loss = 1.0;
                }

                energy(i) =  (float)max(0.0,unary_weights[util::flat_idx(x, y, img_mat.cols)]) * energy_probability(unary_matrix(x, y,i))-hinge_loss;

                //energy(i) =  max(0.0, unary_weights[util::flat_idx(x, y, img_mat.cols)]) * energy_probability(unary_matrix(5 * x, 5 * y, i)) - (i != ytrue(y, x) ? 1.0 : 0.0);
            }
            GraphicalModel::FunctionIdentifier fid = gm.addFunction(energy);
            size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols)};
            gm.addFactor(fid, var_idxes, var_idxes + 1);
        }
    }
}



void infer(const string method, GraphicalModel gm, const size_t n_var, Eigen::MatrixXi &ann)
{
    // cout << "infer(): BEGIN\n";
    // cout << "method= " << method << endl;

    vector<size_t> ann_vec(n_var);

    if (method == "AlphaExpansion")
    {
        typedef
        opengm::external::MinSTCutKolmogorov<size_t, double>
        MinStCutType;

        typedef
        opengm::GraphCut<GraphicalModel, opengm::Minimizer, MinStCutType>
        MinGraphCut;

        typedef
        opengm::AlphaExpansion<GraphicalModel, MinGraphCut>
        MinAlphaExpansion;

        MinAlphaExpansion inf_engine(gm);

        // cout << "Inferring ..." << endl;
        inf_engine.infer();
        inf_engine.arg(ann_vec);
    }
    else if (method == "ICM")
    {
        typedef opengm::ICM<GraphicalModel, opengm::Minimizer> IcmType;
        // IcmType::VerboseVisitorType visitor;

        IcmType inf_engine(gm);
        inf_engine.infer();
    }
    else
    {
        assert(false && "Unknown inference method");
    }

    size_t idx = 0;
    for (size_t i = 0; i < ann.rows(); ++i)
    {
        for (size_t j = 0; j < ann.cols(); ++j)
        {
            //ok no bug
            ann(i, j) = ann_vec.at(idx);
            ++ idx;
        }
    }
}

//for regular use
void set_1st_order(const cv::Mat img_mat, ProbImage unary_matrix, const size_t n_label, GraphicalModel &gm)
{
    using namespace std;

    for (size_t x = 0; x < img_mat.cols; ++x)
    {
        for (size_t y = 0; y < img_mat.rows; ++y)
        {
            // add a function
            const size_t shape[] = {n_label};
            opengm::ExplicitFunction<float> energy(shape, shape + 1);

            for (size_t i = 0; i < n_label; i++)
                energy(i) = -unary_matrix(x, y, i);
            GraphicalModel::FunctionIdentifier fid = gm.addFunction(energy);

            // add a factor
            size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols)};
            gm.addFactor(fid, var_idxes, var_idxes + 1);
        }
    }
}


//for svm
void set_1st_order(cv::Mat img_mat, ProbImage unary_matrix, double *unary_weights, const size_t n_label, GraphicalModel &gm)
{
    using namespace std;

    // assert(unary_matrix.width()==img_mat.cols && "err");// debug resize no constraints
    // assert(unary_matrix.height()==img_mat.rows && "err");
    //assert(sizeof(unary_weights)/sizeof(double) == unary_matrix.width()*unary_matrix.height());


    for (size_t x = 0; x < img_mat.cols; ++x)
    {
        for (size_t y = 0; y < img_mat.rows; ++y)
        {
            const size_t shape[] = {n_label};
            opengm::ExplicitFunction<float> energy(shape, shape + 1);

            for (size_t i = 0; i < n_label; i++)
            {

                // assert(unary_weights[util::flat_idx(x, y, img_mat.cols)]==1.0);
                energy(i) = max(0.0, unary_weights[util::flat_idx(x, y, img_mat.cols)]) * -unary_matrix(x, y, i);
            }
            GraphicalModel::FunctionIdentifier fid = gm.addFunction(energy);
            // add a factor
            size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols)};
            gm.addFactor(fid, var_idxes, var_idxes + 1);
        }
    }
}

void set_2nd_order(cv::Mat img_mat, const size_t n_label, EnergyParam energy_param, GraphicalModel &gm)
{
    // Params needed by the Pott model
    const float equal_pen = 0.0;

    float beta;
    beta = edge_potential::get_beta(img_mat);


    Eigen::MatrixXd theta_phi(2, 1);
    theta_phi << energy_param["theta_phi_1"],
              energy_param["theta_phi_2"];

    //
    for (size_t x = 0; x < img_mat.cols; ++x)
    {
        for (size_t y = 0; y < img_mat.rows; ++y)
        {
            cv::Point2i p1;
            p1.x = x; p1.y = y;

            // (x, y) -- (x + 1, y)
            if (x + 1 < img_mat.cols)
            {
                // add a function
                cv::Point2i p2;
                p2.x = x + 1; p2.y = y;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);
                // printf("%f \n",unequal_pen);

                //
                opengm::PottsFunction<float> pott(n_label, n_label, equal_pen, unequal_pen);
                GraphicalModel::FunctionIdentifier fid = gm.addFunction(pott);

                // add a factor
                size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols), util::flat_idx(x + 1, y, img_mat.cols)};
                sort(var_idxes, var_idxes + 2);
                gm.addFactor(fid, var_idxes, var_idxes + 2);
            }

            // (x, y) -- (x, y + 1)
            if (y + 1 < img_mat.rows)
            {
                // add a function
                cv::Point2i p2;
                p2.x = x; p2.y = y + 1;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);
                // printf("%f \n",unequal_pen);

                //
                opengm::PottsFunction<float> pott(n_label, n_label, equal_pen, unequal_pen);
                GraphicalModel::FunctionIdentifier fid = gm.addFunction(pott);

                // add a factor
                size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols), util::flat_idx(x, y + 1, img_mat.cols)};
                sort(var_idxes, var_idxes + 2);
                gm.addFactor(fid, var_idxes, var_idxes + 2);
            }
        }
    }
}

//for svm
void set_2nd_order(cv::Mat img_mat, const size_t n_label, EnergyParam energy_param, double *pair_weights, GraphicalModel &gm)
{
    // Params needed by the Pott model
    float equal_pen = 0.0;

    //
    float beta;
    beta = edge_potential::get_beta(img_mat);

    Eigen::MatrixXd theta_phi(2, 1);
    theta_phi << energy_param["theta_phi_1"],
              energy_param["theta_phi_2"];

    size_t psioffset = (img_mat.cols - 1) * (img_mat.rows - 1);

    for (size_t x = 0; x < img_mat.cols - 1; ++x)
    {
        for (size_t y = 0; y < img_mat.rows - 1; ++y)
        {
            cv::Point2i p1;
            p1.x = x; p1.y = y;

            // (x, y) -- (x + 1, y)
            if (x + 1 < img_mat.cols)
            {
                // add a function
                cv::Point2i p2;
                p2.x = x + 1; p2.y = y;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);
                equal_pen *= max(0.0, pair_weights[util::flat_idx(x, y, img_mat.cols - 1)]);
                unequal_pen *=  max(0.0, pair_weights[util::flat_idx(x, y, img_mat.cols - 1)]);


                // assert(pair_weights[util::flat_idx(x,y,img_mat.cols-1)]==1.0);

                opengm::PottsFunction<float> pott(n_label, n_label, equal_pen, unequal_pen);
                GraphicalModel::FunctionIdentifier fid = gm.addFunction(pott);

                // add a factor
                size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols), util::flat_idx(x + 1, y, img_mat.cols)};
                sort(var_idxes, var_idxes + 2);
                gm.addFactor(fid, var_idxes, var_idxes + 2);
            }

            // (x, y) -- (x, y + 1)
            if (y + 1 < img_mat.rows)
            {
                // add a function
                cv::Point2i p2;
                p2.x = x; p2.y = y + 1;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);

                unequal_pen *= max(0.0, pair_weights[psioffset + util::flat_idx(x, y, img_mat.cols - 1)]);
                equal_pen *= max(0.0, pair_weights[psioffset + util::flat_idx(x, y, img_mat.cols - 1)]);

                // assert(pair_weights[psioffset+util::flat_idx(x,y,img_mat.cols-1)]==1.0);

                opengm::PottsFunction<float> pott(n_label, n_label, equal_pen, unequal_pen);
                GraphicalModel::FunctionIdentifier fid = gm.addFunction(pott);

                // add a factor
                size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols), util::flat_idx(x, y + 1, img_mat.cols)};
                sort(var_idxes, var_idxes + 2);
                gm.addFactor(fid, var_idxes, var_idxes + 2);
            }
        }
    }
}

void get_2nd_order_psi(cv::Mat &img_mat, Eigen::MatrixXi &annotation_matrix, double *psi)
{

    EnergyParam energy_param;
    train("VOC", &energy_param);
    const float equal_pen = 0.0;

    float beta;
    beta = edge_potential::get_beta(img_mat);

    Eigen::MatrixXd theta_phi(2, 1);
    theta_phi << energy_param["theta_phi_1"],
              energy_param["theta_phi_2"];



    size_t psioffset = (img_mat.cols - 1) * (img_mat.rows - 1);


    for (size_t x = 0; x < img_mat.cols - 1; ++x)
    {
        for (size_t y = 0; y < img_mat.rows - 1; ++y)
        {
            cv::Point2i p1;
            p1.x = x; p1.y = y;

            // (x, y) -- (x + 1, y)
            // if (x+1 < img_mat.cols) {
            {
                // add a function
                cv::Point2i p2;
                p2.x = x + 1; p2.y = y;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);

		if(annotation_matrix(y, x)==255 ||annotation_matrix(y, x+1)==255) 
                    unequal_pen=equal_pen;
                psi[x + y * (img_mat.cols - 1)] = annotation_matrix(y, x) == annotation_matrix(y, x + 1) ? equal_pen : unequal_pen; //horizontal psi
            }

            // }

            // (x, y) -- (x, y + 1)
            // if (y+1 < img_mat.rows)
            {
                // add a function
                cv::Point2i p2;
                p2.x = x; p2.y = y + 1;

                float unequal_pen;
                unequal_pen = edge_potential::potential(img_mat.at<cv::Vec3b>(p1), img_mat.at<cv::Vec3b>(p2), beta, theta_phi);
		
		 if(annotation_matrix(y, x)==255 ||annotation_matrix(y+1, x)==255) 
                    unequal_pen=equal_pen;

                psi[psioffset + x + y * (img_mat.cols - 1) ] = annotation_matrix(y, x) == annotation_matrix(y + 1, x) ? equal_pen : unequal_pen; //vertical psi

            }
        }
    }
}

}// namespace shotton
} // namespace lab1231_sun_prj
