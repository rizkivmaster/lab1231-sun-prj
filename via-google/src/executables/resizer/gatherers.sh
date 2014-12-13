source ../config
target=$googleimgfolder
echo "Target cropped image folder:"
echo "$target"
echo "Class list"
echo "$classlistfile"
echo "######################"


imgfolddirs=`ls -d $target/*`

`rm imgfolddir`
for dir in $imgfolddirs
do
	echo $dir >> imgfolddir
	#echo $dir
done

IFS=$'\n' read -d '' -r -a imgfolds < imgfolddir

#`rm allclasssmrf`

c=0
max=`wc -l < $classlistfile`
echo $max
while [ $c -lt $max ]
do
	#`rm ${names[$c]}smrf`
	imgnames=`ls -d ${imgfolds[$c]}/*`
	for imgname in $imgnames		
	do
		echo "resizing $imgname"
		mogrify -resize '100x100>' $imgname
	done
	(( c++ ))
done
`rm imgfolddir`
