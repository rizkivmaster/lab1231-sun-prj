#include <shotton/shotton.h>
#include <saliency/saliency.h>
#define __(x) printf("%s\n",x);

using namespace std;
namespace lab1231_sun_prj
{
namespace shotton
{

void train(const string datasets_name, EnergyParam *energy_param)
{
    cout << "train(): BEGIN\n";

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

    cout << "train(): END\n";
}

Eigen::MatrixXi annotate(const size_t n_label, const string img_dir, const string unary_dir, EnergyParam energy_param, const size_t object_label)
{

    cout << "annotate(): BEGIN\n";

    //read image file
    cv::Mat img_mat = cv::imread(img_dir, CV_LOAD_IMAGE_COLOR);
    ///read unary
    ProbImage unary_mat;

    unary_mat.load( unary_dir.c_str() );

    Saliency saliency;
    const cv::Mat_<double> saliencyMap = saliency.saliency(img_mat);
    imwrite("debug.png", saliencyMap * 255);

    const size_t n_var = img_mat.rows * img_mat.cols;

    GraphicalModel gm( opengm::SimpleDiscreteSpace<size_t, size_t>(n_var, n_label) );

    printf("%d\n",n_label);

    set_1st_order(img_mat, saliencyMap, unary_mat, n_label, object_label, gm);

    set_2nd_order(img_mat, n_label,object_label, energy_param, gm);

    Eigen::MatrixXi ann(img_mat.rows, img_mat.cols);
    const string method = "AlphaExpansion";//: "AlphaExpansion", "ICM"
    infer(method, gm, n_var, ann);

    cout << "annotate(): END\n";
    return ann;
}

void infer(const string method, GraphicalModel gm, const size_t n_var, Eigen::MatrixXi &ann)
{
    cout << "infer(): BEGIN\n";
    cout << "method= " << method << endl;

    vector<size_t> ann_vec(n_var);


    if (strcmp(method.c_str(), "MinCut") == 0)
    {

        typedef
        opengm::external::MinSTCutKolmogorov<size_t, double>
        MinStCutType;

        typedef
        opengm::GraphCut<GraphicalModel, opengm::Minimizer, MinStCutType>
        MinGraphCut;

        MinGraphCut inf_engine(gm);

        cout << "Inferring ..." << endl;
        inf_engine.infer();
        inf_engine.arg(ann_vec);
    }
    else if (method == "AlphaExpansion")
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
        IcmType::VerboseVisitorType visitor;

        IcmType inf_engine(gm);
        inf_engine.infer(visitor);
    }
    else
    {
        assert(false && "Unknown inference method");
    }

    //
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

    cout << "infer(): END\n";
}

void set_1st_order(const cv::Mat img_mat, const cv::Mat_<double> saliency_mat, ProbImage unary_mat, const size_t n_label, const size_t object_label, GraphicalModel &gm)
{
    float alpha = 1.0;
    float label_penalty = 1.0;
    using namespace std;
    printf("Unary width: %d\n",unary_mat.width());
    printf("Unary height: %d\n",unary_mat.width());
    assert(unary_mat.width() == img_mat.cols && "err width");
    assert(unary_mat.height() == img_mat.rows && "err height");


    for (size_t x = 0; x < img_mat.cols; ++x)
    {
        for (size_t y = 0; y < img_mat.rows; ++y)
        {
            // add a function
            // const size_t shape[] = {2};
            // opengm::ExplicitFunction<float> energy(shape, shape+1);

            // //FIRST FORMULATION
            // energy(1) = -unary_mat(x,y,object_label) + (1-saliency_mat.at<double>(y,x));
            // energy(0) = 100000000.0;

            // for(size_t i = 0; i < n_label; i++)
            //   if(i!=object_label)
            //     energy(0) = min(energy(0),unary_mat(x,y,i));

            // energy(0) +=alpha*saliency_mat.at<double>(y,x);
            //SECOND FORMULATION

            // add a function
            const size_t shape[] = {n_label};
            opengm::ExplicitFunction<float> energy(shape, shape + 1);

            

            for (size_t i = 0; i < n_label; i++)
            {
                energy(i) = -unary_mat(x, y, i);

                if (i != object_label)
                {
                    energy(i) += alpha * saliency_mat.at<double>(y, x);
                }
                else
                {
                    energy(i) += 0.5 * (1 - saliency_mat.at<double>(y, x));
                }

                // energy(0) += alpha * saliency_mat.at<double>(y, x);
                // energy(object_label) += alpha * (1 - saliency_mat.at<double>(y, x));
                if (i != object_label && i != 0 )
                    energy(i) += label_penalty;
            }

            GraphicalModel::FunctionIdentifier fid = gm.addFunction(energy);



            // add a factor
            size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols)};
            gm.addFactor(fid, var_idxes, var_idxes + 1);
        }
    }
}

void set_2nd_order(cv::Mat img_mat, const size_t n_label, const size_t object_label, EnergyParam energy_param, GraphicalModel &gm)
{
    // Params needed by the Pott model

    const float equal_pen = 0.0;

    //
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

                //
                opengm::PottsFunction<float> pott(2, 2, equal_pen, unequal_pen);
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

                //
                opengm::PottsFunction<float> pott(2, 2, equal_pen, unequal_pen);
                GraphicalModel::FunctionIdentifier fid = gm.addFunction(pott);

                // add a factor
                size_t var_idxes[] = {util::flat_idx(x, y, img_mat.cols), util::flat_idx(x, y + 1, img_mat.cols)};
                sort(var_idxes, var_idxes + 2);
                gm.addFactor(fid, var_idxes, var_idxes + 2);
            }
        }
    }

}

}// namespace shotton
} // namespace lab1231_sun_prj