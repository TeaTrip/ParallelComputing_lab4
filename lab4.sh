num_threads=(1 2 4 6 8)
n1=90
n2=1770
program_name=("lab3")
n_arr=

function get_n_arr {
  ((delta = ($n2 - $n1) / 10))

  counter=$n1
  n_arr[0]=$counter
  i=1
  while [ $counter -lt $n2 ]; do
    ((counter += $delta))
    n_arr[$i]=$counter
    ((i = $i + 156))
  done
  n_arr[$i]=$n2
}

get_n_arr

result_file=lab4.csv
echo "file; n; t,ms" >$result_file
for name in "${program_name[@]}"; do
  for m_thread in "${num_threads[@]}"; do
    for n in "${n_arr[@]}"; do
      r=$(./"$name" "$n" "$m_thread")
      echo "$name" ";" "$r" ";" "$m_thread" >>$result_file
      echo $name - $n - $m_thread
    done
  done
done
