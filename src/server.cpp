#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "server.grpc.pb.h" // might need to write absolute path to the file

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using server::HelloRequest;
using server::PrintRequest;

// Logic and data behind the server's behavior.
class ServerServiceImpl final : public server::Server::Service {
	Status SayHello(ServerContext *context, const HelloRequest *request, google::protobuf::Empty *reply) override {
		std::cout << "hello" << std::endl;
		return Status::OK;
	}

	Status Print(ServerContext *context, const PrintRequest *request, google::protobuf::Empty *reply) override {
		std::cout << request->x() << std::endl;
		return Status::OK;
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	ServerServiceImpl service;

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main(int argc, char **argv) {
	RunServer();

	return 0;
}