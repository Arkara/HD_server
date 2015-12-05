for (( i=0; i<150; i=i+1)) do
  ( ./SimpleNetworkConnectionTester > "SimpleNetworkConnectionTester.$i.txt" ) &
  sleep 1;
done

