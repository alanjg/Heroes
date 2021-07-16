// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: entity.proto
#include "pch.h"
#include "entity.pb.h"
#include "entity.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace heroes {

static const char* HeroesServer_method_names[] = {
  "/heroes.HeroesServer/ProcessClientCommand",
  "/heroes.HeroesServer/UpdateState",
  "/heroes.HeroesServer/CreateGame",
  "/heroes.HeroesServer/JoinGame",
};

std::unique_ptr< HeroesServer::Stub> HeroesServer::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< HeroesServer::Stub> stub(new HeroesServer::Stub(channel));
  return stub;
}

HeroesServer::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_ProcessClientCommand_(HeroesServer_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_UpdateState_(HeroesServer_method_names[1], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_CreateGame_(HeroesServer_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_JoinGame_(HeroesServer_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status HeroesServer::Stub::ProcessClientCommand(::grpc::ClientContext* context, const ::heroes::ClientCommand& request, ::heroes::ClientCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ProcessClientCommand_, context, request, response);
}

void HeroesServer::Stub::experimental_async::ProcessClientCommand(::grpc::ClientContext* context, const ::heroes::ClientCommand* request, ::heroes::ClientCommandResult* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_ProcessClientCommand_, context, request, response, std::move(f));
}

void HeroesServer::Stub::experimental_async::ProcessClientCommand(::grpc::ClientContext* context, const ::heroes::ClientCommand* request, ::heroes::ClientCommandResult* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_ProcessClientCommand_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::heroes::ClientCommandResult>* HeroesServer::Stub::PrepareAsyncProcessClientCommandRaw(::grpc::ClientContext* context, const ::heroes::ClientCommand& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::heroes::ClientCommandResult>::Create(channel_.get(), cq, rpcmethod_ProcessClientCommand_, context, request, false);
}

::grpc::ClientAsyncResponseReader< ::heroes::ClientCommandResult>* HeroesServer::Stub::AsyncProcessClientCommandRaw(::grpc::ClientContext* context, const ::heroes::ClientCommand& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncProcessClientCommandRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::heroes::ServerState>* HeroesServer::Stub::UpdateStateRaw(::grpc::ClientContext* context, const ::heroes::UpdateStateRequest& request) {
  return ::grpc::internal::ClientReaderFactory< ::heroes::ServerState>::Create(channel_.get(), rpcmethod_UpdateState_, context, request);
}

void HeroesServer::Stub::experimental_async::UpdateState(::grpc::ClientContext* context, ::heroes::UpdateStateRequest* request, ::grpc::experimental::ClientReadReactor< ::heroes::ServerState>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::heroes::ServerState>::Create(stub_->channel_.get(), stub_->rpcmethod_UpdateState_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::heroes::ServerState>* HeroesServer::Stub::AsyncUpdateStateRaw(::grpc::ClientContext* context, const ::heroes::UpdateStateRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::heroes::ServerState>::Create(channel_.get(), cq, rpcmethod_UpdateState_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::heroes::ServerState>* HeroesServer::Stub::PrepareAsyncUpdateStateRaw(::grpc::ClientContext* context, const ::heroes::UpdateStateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::heroes::ServerState>::Create(channel_.get(), cq, rpcmethod_UpdateState_, context, request, false, nullptr);
}

::grpc::Status HeroesServer::Stub::CreateGame(::grpc::ClientContext* context, const ::heroes::CreateGameRequest& request, ::heroes::CreateGameResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_CreateGame_, context, request, response);
}

void HeroesServer::Stub::experimental_async::CreateGame(::grpc::ClientContext* context, const ::heroes::CreateGameRequest* request, ::heroes::CreateGameResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_CreateGame_, context, request, response, std::move(f));
}

void HeroesServer::Stub::experimental_async::CreateGame(::grpc::ClientContext* context, const ::heroes::CreateGameRequest* request, ::heroes::CreateGameResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_CreateGame_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::heroes::CreateGameResponse>* HeroesServer::Stub::PrepareAsyncCreateGameRaw(::grpc::ClientContext* context, const ::heroes::CreateGameRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::heroes::CreateGameResponse>::Create(channel_.get(), cq, rpcmethod_CreateGame_, context, request, false);
}

::grpc::ClientAsyncResponseReader< ::heroes::CreateGameResponse>* HeroesServer::Stub::AsyncCreateGameRaw(::grpc::ClientContext* context, const ::heroes::CreateGameRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncCreateGameRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status HeroesServer::Stub::JoinGame(::grpc::ClientContext* context, const ::heroes::JoinGameRequest& request, ::heroes::JoinGameResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_JoinGame_, context, request, response);
}

void HeroesServer::Stub::experimental_async::JoinGame(::grpc::ClientContext* context, const ::heroes::JoinGameRequest* request, ::heroes::JoinGameResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_JoinGame_, context, request, response, std::move(f));
}

void HeroesServer::Stub::experimental_async::JoinGame(::grpc::ClientContext* context, const ::heroes::JoinGameRequest* request, ::heroes::JoinGameResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_JoinGame_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::heroes::JoinGameResponse>* HeroesServer::Stub::PrepareAsyncJoinGameRaw(::grpc::ClientContext* context, const ::heroes::JoinGameRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::heroes::JoinGameResponse>::Create(channel_.get(), cq, rpcmethod_JoinGame_, context, request, false);
}

::grpc::ClientAsyncResponseReader< ::heroes::JoinGameResponse>* HeroesServer::Stub::AsyncJoinGameRaw(::grpc::ClientContext* context, const ::heroes::JoinGameRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncJoinGameRaw(context, request, cq);
  result->StartCall();
  return result;
}

HeroesServer::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HeroesServer_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HeroesServer::Service, ::heroes::ClientCommand, ::heroes::ClientCommandResult>(
          [](HeroesServer::Service* service,
             ::grpc::ServerContext* ctx,
             const ::heroes::ClientCommand* req,
             ::heroes::ClientCommandResult* resp) {
               return service->ProcessClientCommand(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HeroesServer_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< HeroesServer::Service, ::heroes::UpdateStateRequest, ::heroes::ServerState>(
          [](HeroesServer::Service* service,
             ::grpc::ServerContext* ctx,
             const ::heroes::UpdateStateRequest* req,
             ::grpc::ServerWriter<::heroes::ServerState>* writer) {
               return service->UpdateState(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HeroesServer_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HeroesServer::Service, ::heroes::CreateGameRequest, ::heroes::CreateGameResponse>(
          [](HeroesServer::Service* service,
             ::grpc::ServerContext* ctx,
             const ::heroes::CreateGameRequest* req,
             ::heroes::CreateGameResponse* resp) {
               return service->CreateGame(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HeroesServer_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HeroesServer::Service, ::heroes::JoinGameRequest, ::heroes::JoinGameResponse>(
          [](HeroesServer::Service* service,
             ::grpc::ServerContext* ctx,
             const ::heroes::JoinGameRequest* req,
             ::heroes::JoinGameResponse* resp) {
               return service->JoinGame(ctx, req, resp);
             }, this)));
}

HeroesServer::Service::~Service() {
}

::grpc::Status HeroesServer::Service::ProcessClientCommand(::grpc::ServerContext* context, const ::heroes::ClientCommand* request, ::heroes::ClientCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HeroesServer::Service::UpdateState(::grpc::ServerContext* context, const ::heroes::UpdateStateRequest* request, ::grpc::ServerWriter< ::heroes::ServerState>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HeroesServer::Service::CreateGame(::grpc::ServerContext* context, const ::heroes::CreateGameRequest* request, ::heroes::CreateGameResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HeroesServer::Service::JoinGame(::grpc::ServerContext* context, const ::heroes::JoinGameRequest* request, ::heroes::JoinGameResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace heroes

