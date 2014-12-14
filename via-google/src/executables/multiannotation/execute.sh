source ../config
#we need svm classify path,test list path, model path, output path
echo "SVM classify path: $svm_classify_path"
echo "Test List file: $voc_test_list"
echo "SVM model path: $svm_model_path"
echo "SVM output path: $svm_output_path"
echo "##################################"

echo "START ANNOTATION"
`$svm_classify_path $voc_test_list $svm_model_path $svm_output_path`
echo "DONE MULTICLASS ANNOTATION"