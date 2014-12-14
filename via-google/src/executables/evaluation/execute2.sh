source ../config
echo "Evaluator: $evaluator_path"
echo "GT CSV: $voc_gt_csv"
echo "Test CSV: $google_test_csv_dir"
echo "Test List: $voc_test_list"

#relist
`cut -d , -f 1 $voc_test_list > testlist_tmp`
IFS=$'\n' read -d '' -r -a testlist < testlist_tmp
`rm testlist_tmp`

test_size=${#testlist[@]}
c=0
while [ $c -lt $test_size ]
do
	if [[ -f "$google_test_csv_dir/${testlist[$c]}.csv" ]]; then
			echo "${testlist[$c]}.csv"
			`echo "${testlist[$c]}.csv" >> eval_tmp`
	fi
    (( c++ ))
done

`python $evaluator_path ${PWD}/eval_tmp $google_test_csv_dir $voc_gt_csv`
`rm eval_tmp`