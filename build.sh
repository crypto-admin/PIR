base_dir=`pwd`

bazel build --cxxopt=-std=c++17 //pir/cpp:pir_test
[ $? -ne 0 ] && echo "bazel build failed" && exit 1

bazel build --cxxopt=-std=c++17 //pir/cpp:pir_server
[ $? -ne 0 ] && echo "bazel build failed" && exit 1

bazel build --cxxopt=-std=c++17 //pir/cpp:pir_client
[ $? -ne 0 ] && echo "bazel build failed" && exit 1

rm -rf output && \
mkdir output

cp bazel-bin/pir/cpp/pir_server output/ && \
cp bazel-bin/pir/cpp/pir_client output/ && \
cp bazel-bin/pir/cpp/pir_test output/ && \
chmod 755 startup_client.sh && \
chmod 755 startup_server.sh
cp startup_server.sh output/ && \
cp startup_client.sh output/ && \
# cp config/* output && \
#cp server_key_test.csv output && \
cp client_phonenum.csv output
exit $?

