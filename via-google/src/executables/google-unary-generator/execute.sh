source ../config
echo "Predictor path: $predict_bin"
echo "Google Unaries Dir: $google_unaries_dir"
echo "Google Training List: $google_train_list"
echo "DPM Image folder: $dpm_img_folder"
echo "Images list: $google_object_file"
echo "Normal Param: $normal_param"
echo "Quota per class: $google_sampling_size"
echo "##################################"
`cut -d , -f 1 $google_object_file > objid_tmp`
`cut -d , -f 2 $google_object_file > imgname_tmp`
IFS=$'\n' read -d '' -r -a objid < objid_tmp
IFS=$'\n' read -d '' -r -a imgname < imgname_tmp
`rm objid_tmp`
`rm imgname_tmp`

`rm $google_train_list`

obj_size=${#objid[@]}
c=0
while [ $c -lt $obj_size ]
do
	quota[${objid[$c]}]=0
	(( c++ ))
done

image_size=${#imgname[@]}
c=0
while [ $c -lt $image_size ]
do
	filename=$(basename "${imgname[$c]}")
	extension="${filename##*.}"
	singlename="${filename%.*}"
	if [[ -f "$dpm_img_folder/${imgname[$c]}" ]]; then
        #if ok
        if [[ ${quota[${objid[$c]}]} -lt $google_size ]]; then
        	#id object, nama file dpm, source path, target img path, target xml path
        	`echo "$singlename" >> $google_train_list`
        	quota[${objid[$c]}]=`expr ${quota[${objid[$c]}]} + 1`
    	fi
    else
    	#if not ok
    	echo "${imgname[$c]} is missing"
    fi
    (( c++ ))
done

#create google unary
`$predict_bin $normal_param $google_train_list $dpm_img_folder  $google_unaries_dir`