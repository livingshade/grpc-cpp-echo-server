# grpc-cpp-echo-server

A C++ template to quickly start your own project.

This template includes a simplest runnable helloworld program, which breaks down into a `helloworld` lib (see src/ and include/helloworld/) and an executable (see bin/main.cpp).

## Bootstrap

```bash
git clone --recurse-submodules <this_repo>
```

Make sure that gRPC cpp is installed in `$MY_INSTALL_DIR`, and can be found by `cmake find_package()`.

You may need to refer to [https://grpc.io/docs/languages/cpp/quickstart/]

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

Use two shells

```shell
./build/bin/client
./build/bin/server

# expect: Client received: Hello world
```

## Note

This repo uses https://github.com/YanB25/cpp-project-template as template