source ../config
#we need img, unary,class,target png
echo "FG Annotator path: $fg_annotator_path"
echo "Google Image-Object linker: $google_object_file"
echo "DPM Image folder: $dpm_img_folder"
echo "Google Unaries folder: $google_unaries_dir"
echo "Google Annotation folder: $google_csv_dir"
echo "Google PNG folder: $google_png_dir"
echo "##################################"
#relist
`cut -d , -f 1 $google_object_file > objid_tmp`
`cut -d , -f 2 $google_object_file > imgname_tmp`
IFS=$'\n' read -d '' -r -a objid < objid_tmp
IFS=$'\n' read -d '' -r -a imgname < imgname_tmp
`rm objid_tmp`
`rm imgname_tmp`
#initialize sampling bucket
for (( ii = 0; ii < 20; ii++ )); do
	quota[$ii]=0
done

image_size=${#imgname[@]}
c=0
while [ $c -lt $image_size ]
do
	filename=$(basename "${imgname[$c]}")
	extension="${filename##*.}"
	singlename="${filename%.*}"
	#check image
	if [[ -f "$dpm_img_folder/$singlename.jpg" ]]; then
		#check unaries
		if [[ -f "$google_unaries_dir/$singlename.unary" ]]; then
			#in debug
			echo "Annotating $filename"
			echo "$fg_annotator_path VOC 21 ${objid[$c]} $dpm_img_folder/$singlename.jpg  $google_unaries_dir/$singlename.unary $google_png_dir/$singlename.png"
			`$fg_annotator_path VOC 21 ${objid[$c]} $dpm_img_folder/$singlename.jpg $google_unaries_dir/$singlename.unary $google_png_dir/$singlename.png`
		fi
	fi
    (( c++ ))
done

# pngfolddirs=`ls -d $google_png_dir/*`

# #PALETTING
# `rm pnglist_tmp`
# for path in $pngfolddirs
# do
# 	`echo "$path" >> pnglist_tmp`
# done
# matlab -nodesktop -nosplash -r "paletting();quit;"
# `rm pnglist_tmp`

#FINISH PALETTING
echo "FINISH"