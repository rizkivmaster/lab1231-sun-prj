#this code resembles the existing isolated google images into a google_object list
source ../config
google_raw_folder="/home/rizkivmaster/ws/datasets/googleimages/datasets"
echo "Raw google folder:$google_raw_folder"
echo "Target google folder: $dpm_img_folder"
echo "Google-Object list: $google_object_file"
echo "Object list file: $object_list_file"
echo "Image Width and Height : $image_length_max $image_length_max"

`cut -d , -f 1 $object_list_file > classid`
`cut -d , -f 2 $object_list_file > classname`
imgfolddirs=`ls -d $google_raw_folder/*`

`rm imgfolddir`

`rm $google_object_file`
for dir in $imgfolddirs
do
	echo $dir >> imgfolddir
done

IFS=$'\n' read -d '' -r -a imgfolds < imgfolddir
IFS=$'\n' read -d '' -r -a ids < classid
IFS=$'\n' read -d '' -r -a names < classname
c=0
max=`wc -l < $object_list_file`
echo $max
while [ $c -lt $max ]
do
	imgnames=`ls -d ${imgfolds[$c]}/*.jpg`
	for imgname in $imgnames		
	do
		echo "$imgname"
		filename=$(basename "$imgname")
		if [[ $(identify -format '%f' "$imgname" 2>/dev/null) == $filename ]]; then
			filename=$(basename "$imgname")
			echo "copying $imgname $dpm_img_folder/${names[$c]}$filename"
			`cp $imgname $dpm_img_folder/${names[$c]}$filename`
			echo "${ids[$c]},${names[$c]}$filename" >> $google_object_file
		fi
	done
	(( c++ ))
done

echo "RESIZING"
imgfolddirs=`ls -d $dpm_img_folder/*`
width=$image_length_max
height=$image_length_max
for path in $imgfolddirs
do
	command="mogrify -resize ${width}x${height}> $path"
	echo $command
	`$command`
done
