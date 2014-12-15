source ../config
#yang dibutuhkan: parameter dan targetmodel directory
echo "SVM learner path: $svm_learner_path"
echo "SVM model path: $svm_model_path"
echo "Init train list: $init_train_path"
echo "VOC Image Folder: $voc_img_folder"
echo "Target unaries folder: $google_unaries_dir"
echo "Combined train list: $combined_train_list"
echo "##################################"

#Generating unaries for SVM Learning
# `$predict_bin $normal_param $init_train_path $voc_img_folder  $google_unaries_dir`

#relist
# DEBUG
# `cut -d , -f 1 $google_object_file > objid_tmp`
# `cut -d , -f 2 $google_object_file > imgname_tmp`
# IFS=$'\n' read -d '' -r -a objid < objid_tmp
# IFS=$'\n' read -d '' -r -a imgname < imgname_tmp
# `rm objid_tmp`
# `rm imgname_tmp`
# `rm $combined_train_list`
# dont uncomment
#initialize sampling bucket
# for (( ii = 0; ii < 21; ii++ )); do
# 	quota[$ii]=0
# done
# DEBUG
# image_size=${#imgname[@]}
# c=0
# while [ $c -lt $image_size ]
# do
# 	filename=$(basename "${imgname[$c]}")
# 	extension="${filename##*.}"
# 	singlename="${filename%.*}"
# 	if [[ -f "$dpm_img_folder/$singlename.jpg" ]]; then
# 		if [[ -f "$google_png_dir/$singlename.png" ]]; then
# 			`echo "$singlename" >> $combined_train_list`
# 			`cp $dpm_img_folder/$singlename.jpg $voc_img_folder/$singlename.jpg`
# 			`cp $google_png_dir/$singlename.png $voc_png_folder/$singlename.png`
# 		fi
# 	fi
#     (( c++ ))
# done

# `cat $init_train_path >> $combined_train_list`

#create google 
echo "START TRAINING SVM"

#SVM Learning
echo "$svm_learner_path -w 2 -c 1 -e 100 $combined_train_list $svm_model_path"
`$svm_learner_path -w 2 -c 1 -e 100 $combined_train_list $svm_model_path`

# echo "$svm_learner_path -w 2 -c 1 $combined_train_list $svm_model_path"
