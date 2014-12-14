source ../config
echo "Target cropped image folder:"
echo "$dpm_img_folder"
echo "Image Width and Height : $image_length_max $image_length_max"
echo "######################"


imgfolddirs=`ls -d $dpm_img_folder/*`
width=$image_length_max
height=$image_length_max
for path in $imgfolddirs
do
	command="mogrify -resize ${width}x${height}> $path"
	echo $command
	`$command`
done
