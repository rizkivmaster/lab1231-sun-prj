source ../config
echo "Evaluator folder: $voc_evaluator_folder"
echo "VOC PNG folder: $voc_png_folder"
echo "Test prediction folder: $google_test_png_dir"
echo "Test list path: $voc_test_list"
echo ##################################

`/usr/local/MATLAB/R2012a/bin/matlab -nojvm -nodesktop -nosplash "cd '$evaluator_path';groundtruth_folder='$groundtruth_folder';prediction_folder='$prediction_folder';test_list='$voc_test_list';evaluate(groundtruth_folder,prediction_folder,test_list);"