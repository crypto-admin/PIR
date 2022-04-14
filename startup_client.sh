#!/bin/bash
while read rows
do
  echo "index1: ${rows:0:3}"
  echo "private index: ${rows:3:11}"

  #./pir_client --server_addr=localhost:50051  --private_index=${rows:3:11} --x_host=${rows:0:3}
  ./pir_client --server_addr=localhost:50051  --private_index=6 --x_host=${rows:0:3}
done < client_phonenum.csv
