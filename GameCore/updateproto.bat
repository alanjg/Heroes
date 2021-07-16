protoc -I . --grpc_out=. --plugin=protoc-gen-grpc="C:/Users/alan_/source/repos/vcpkg/installed/x86-windows/tools/grpc/grpc_cpp_plugin.exe" entity.proto
protoc -I . --cpp_out=. entity.proto