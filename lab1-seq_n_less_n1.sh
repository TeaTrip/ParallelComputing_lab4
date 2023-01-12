m=2
n1=210
i=1
programs=("lab1-seq2")

result_file="lab1-seq_n_less_n1.csv"
echo "program; n; t,ms" >$result_file
for program_name in "${programs[@]}"; do
  while [ $i -lt $n1 ]; do
    echo $i
    r=$(./$program_name $i)
    echo $program_name ";" $r >>$result_file
    ((i = $i + 5))
  done
  i=0
done
