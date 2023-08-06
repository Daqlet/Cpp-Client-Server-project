#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "server.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using server::Server;
using server::HelloRequest;
using server::PrintRequest;

class Client {
public:
	Client(std::shared_ptr<Channel> channel)
		: stub_(Server::NewStub(channel)) {}

	// Assembles the client's payload, sends it and presents the response back
	// from the server.
	void SayHello() {
		// Data we are sending to the server.
		HelloRequest request;

		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		ClientContext context;

		google::protobuf::Empty reply;

		// The actual RPC.
		Status status = stub_->SayHello(&context, request, &reply);

		// Act upon its status.
		if (status.ok()) {
			std::cout << "Status OK" << std::endl;
		} else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
		}
	}

	// Assembles the client's payload, sends it and presents the response back
	// from the server.
	void PrintX(const std::string& x) {
		// Data we are sending to the server.
		PrintRequest request;
		request.set_x(x);

		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		ClientContext context;

		google::protobuf::Empty reply;

		// The actual RPC.
		Status status = stub_->Print(&context, request, &reply);

		// Act upon its status.
		if (status.ok()) {
			std::cout << "Status OK" << std::endl;
		} else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
		}
	}


private:
	std::unique_ptr<Server::Stub> stub_;
};

int main(int argc, char **argv) {
	// Instantiate the client. It requires a channel, out of which the actual RPCs
	// are created. This channel models a connection to an endpoint specified by
	// the argument "--target=" which is the only expected argument.
	// We indicate that the channel isn't authenticated (use of
	// InsecureChannelCredentials()).
	std::string target_str;
	std::string arg_str("--target");
	if (argc > 1) {
		std::string arg_val = argv[1];
		size_t start_pos = arg_val.find(arg_str);
		if (start_pos != std::string::npos) {
			start_pos += arg_str.size();
			if (arg_val[start_pos] == '=') {
				target_str = arg_val.substr(start_pos + 1);
			} else {
				std::cout << "The only correct argument syntax is --target=" << std::endl;
				return 0;
			}
		} else {
			std::cout << "The only acceptable argument is --target=" << std::endl;
			return 0;
		}
	} else {
		target_str = "localhost:50051";
	}

	Client greeter(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
	int choice = 1;
	while (choice != 0) {
		std::cout << "Choose request type." << std::endl;
		std::cout << "[0] kill the client" << std::endl;
		std::cout << "[1] hello request" << std::endl;
		std::cout << "[2] print request" << std::endl;
		std::cin >> choice;
		if (choice == 1) {
			greeter.SayHello();
		} else if (choice == 2) {
			std::string x;
			std::cout << "Type request string: ";
			std::cin.ignore();
			std::getline(std::cin, x, '\n');
			greeter.PrintX(x);
		}
	}
	
	return 0;
}