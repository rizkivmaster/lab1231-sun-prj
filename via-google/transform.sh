#this will automatically compile all codes
# and manage experiments throughly

cd build
make
cd ..

cd src/executables
source config


rand=$RANDOM

logpath="$current_exp/log$rand"
`cp config $current_exp/$randconfig`
filename=$(basename "$settings_path")
`cp $settings_path $current_exp/$rand$filename`


echo "START TRANSFORM" > $logpath
date +"%m-%d-%y %T" >> $logpath


#download google images
# cd google-retriever
# ./execute.sh
# cd ..
#
# cd list-and-extract
# ./execute.sh
# cd ..
#resize to fit voc
# cd list-and-resize
# ./execute.sh
# cd ..
# #train initial parameters START HERE

# echo "START GENERATING INITIAL MODEL" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd param-generator
# ./execute.sh 2>&1 | tee temp_log
# cat temp_log >> $logpath
# cd ..


#FROM HERE

# echo "START GENERATING GOOGLE UNARY" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd google-unary-generator
# ./execute.sh 2>&1 | tee temp_log
# cat temp_log >> $logpath
# cd ..

# echo "START F/G SEGMENTATION" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd fgannotator
# ./execute.sh 2>&1 | tee temp_log
# cat temp_log >> $logpath
# cd ..


# echo "START IMPROVING MODEL" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd param-enhancer
# ./execute.sh 2>&1 | tee temp_log
# cat temp_log >> $logpath
# cd ..

#echo "START SVM LEARNER" >> $logpath
#date +"%m-%d-%y %T" >> $logpath

#cd svm-learning
#./execute.sh 2>&1 | tee temp_log
#cat temp_log >> $logpath
#cd ..

#echo "START TEST UNARY GENERATION" >> $logpath
#date +"%m-%d-%y %T" >> $logpath

#cd test-unary-generator
#./execute.sh 2>&1 | tee temp_log
#cat temp_log >> $logpath
#cd ..

# echo "START TESTSET ANNOTATION" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd multiannotation
# ./execute.sh 2>&1 | tee temp_log
# cat temp_log >> $logpath
# cd ..


# echo "START EVALUATION" >> $logpath
# date +"%m-%d-%y %T" >> $logpath

# cd evaluation
# ./execute.sh
# cd ..

echo "START BASELINE PREDICTION" >> $logpath
 date +"%m-%d-%y %T" >> $logpath

 cd baseline-prediction
 ./execute.sh 2>&1 | tee temp_log
 cat temp_log >> $logpath
 cd ..


cd ../..
