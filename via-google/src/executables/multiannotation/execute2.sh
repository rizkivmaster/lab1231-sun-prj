source ../config
#we need img, unary,class,target png
echo "Multi Annotator path: $multiannotator_path"
echo "Test Images Folder: $voc_img_folder"
echo "Test List file: $voc_test_list"
echo "Test Unaries folder: $google_test_unary_path"
echo "Target Test PNG folder: $google_test_png_dir"
echo "Target Test CSV folder: $google_test_csv_dir"
echo "##################################"
#relist
#initialize sampling bucket

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
		if [[ -f "$google_test_unary_path/$singlename.unary" ]]; then
			#in debug
			echo "Annotating $filename"
			echo "$multiannotator_path VOC 21 $voc_img_folder/$singlename.jpg $google_test_unary_path/$singlename.unary $google_test_png_dir/$singlename.png $google_test_csv_dir/$singlename.csv"
			`$multiannotator_path VOC 21 $voc_img_folder/$singlename.jpg $google_test_unary_path/$singlename.unary $google_test_png_dir/$singlename.png $google_test_csv_dir/$singlename.csv`
		fi
	fi
    (( c++ ))
done

echo "DONE ANNOTATING"