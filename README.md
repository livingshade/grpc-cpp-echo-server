# grpc-cpp-echo-server

A C++ template to quickly start your own project.

This template includes a simplest runnable helloworld program, which breaks down into a `helloworld` lib (see src/ and include/helloworld/) and an executable (see bin/main.cpp).

## Bootstrap

```bash
git clone --recurse-submodules <this_repo>

git clone --recurse-submodules --depth 1 --shallow-submodules https://github.com/livingshade/grpc.git
```

This project uses a modified version of gRPC. But the installation is identical to offical gRPC [https://grpc.io/docs/languages/cpp/quickstart/]

Make sure that gRPC cpp is installed in `$MY_INSTALL_DIR`, and can be found by `cmake find_package()`.

Then run:

``` bash
bash ./bootstrap.sh
```

## Compile

```shell
mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR -DCMAKE_BUILD_TYPE=Debug ..
# or cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```



## Run

We need three shells.

```shell
./build/bin/server_dynamic
# start the server
```

```shell
./build/bin/client
# client will send request every 5s
```

```shell
./build/bin/nf_sender
# nf_sender will change the server side interceptor
# changing the reply from "hello! world" to "good byee hello! world"
```

## TODO

## Note

This repo uses https://github.com/YanB25/cpp-project-template as template