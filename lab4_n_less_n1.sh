m=2
n1=210
i=1
programs=("lab4")

result_file="lab4.csv"
echo "program; n; t,ms" >$result_file
for program_name in "${programs[@]}"; do
  while [ $i -lt $n1 ]; do
    echo $i
    r=$(./$program_name $i $m)
    echo $program_name ";" $r >>$result_file
    ((i = $i + 5))
  done
  i=0
done
