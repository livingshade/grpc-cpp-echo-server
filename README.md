# grpc-cpp-echo-server

## Dependency

Make sure that gRPC cpp is installed in `$MY_INSTALL_DIR`,

and can be found by `cmake find_package()` 

## Compile

```shell
mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ..
make -j
```

## Run

Use two shell

```shell
./build/client
./build/server

# expect: Client received: Hello world
```

