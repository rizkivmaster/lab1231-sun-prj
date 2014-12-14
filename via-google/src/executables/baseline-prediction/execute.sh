source ../config
echo "Predictor path: $predict_bin"
echo "Multi Annotator path: $multiannotator_path"
echo "VOC Testing List: $voc_test_list"
echo "VOC Image folder: $voc_img_folder"
echo "Normal Test Unaries Dir: $normal_test_unary_path"
echo "Normal Test PNG Dir: $normal_test_png_dir"
echo "Normal Test CSV Dir: $normal_test_csv_dir"
echo "Normal Param: $normal_param"
echo "GT CSV: $voc_gt_csv"
echo "Evaluator: $evaluator_path"
echo "##################################"

#create normal unary
echo "START TO GENERATE UNARIES FOR NORMAL TEST IMAGES"
`$predict_bin $normal_param $voc_test_list $voc_img_folder  $normal_test_unary_path`

#create multiclass segmentation
`cut -d , -f 1 $voc_test_list > imgname_tmp`
IFS=$'\n' read -d '' -r -a imgname < imgname_tmp
`rm imgname_tmp`

image_size=${#imgname[@]}
c=0
while [ $c -lt $image_size ]
do
	filename=$(basename "${imgname[$c]}")
	extension="${filename##*.}"
	singlename="${filename%.*}"
	#check image
	if [[ -f "$voc_img_folder/$singlename.jpg" ]]; then
		#check unaries
		if [[ -f "$normal_test_unary_path/$singlename.unary" ]]; then
			#in debug
			echo "Annotating $filename"
			echo "$multiannotator_path VOC 21 $voc_img_folder/$singlename.jpg $normal_test_unary_path/$singlename.unary $normal_test_png_dir/$singlename.png $normal_test_csv_dir/$singlename.csv"
			`$multiannotator_path VOC 21 $voc_img_folder/$singlename.jpg $normal_test_unary_path/$singlename.unary $normal_test_png_dir/$singlename.png $normal_test_csv_dir/$singlename.csv`
		fi
	fi
    (( c++ ))
done

echo "DONE ANNOTATING"

#create evaluation
`cut -d , -f 1 $voc_test_list > testlist_tmp`
IFS=$'\n' read -d '' -r -a testlist < testlist_tmp
`rm testlist_tmp`

test_size=${#testlist[@]}
c=0
while [ $c -lt $test_size ]
do
	if [[ -f "$normal_test_csv_dir/${testlist[$c]}.csv" ]]; then
			echo "${testlist[$c]}.csv"
			`echo "${testlist[$c]}.csv" >> eval_tmp`
	fi
    (( c++ ))
done