n=$1
for (( i=0; i<150; i=i+1)) do
  ( ./SimpleNetworkConnectionTester $n> "SimpleNetworkConnectionTester.$i.txt" ) &
  sleep 1;
done

