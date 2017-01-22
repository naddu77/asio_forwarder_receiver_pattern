// asio_forwarder_receiver_pattern.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "CommunicationMessage.h"
#include "Peer.h"
#include "IPv4_TCP_Server_Transceiver.h"
#include "IPv4_TCP_Client_Transceiver.h"
#include <thread>

int main()
{
	try
	{
		Peer server(true);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		Peer client(false);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		server.AsyncReceive();
		client.AsyncReceive();
		std::this_thread::sleep_for(std::chrono::seconds(3));
		server.AsyncSend("Hello, Client");
		client.AsyncSend("Hello, Server");
		std::this_thread::sleep_for(std::chrono::seconds(3));

		//boost::asio::io_service io_service_for_server;
		//IPv4_TCP_Server_Transceiver<CommunicationMessage, std::function<void(const boost::system::error_code&)>, std::function<void(const boost::system::error_code&)>> server(io_service_for_server, 1004);
		//std::thread server_thread([&io_service_for_server]() {
		//	io_service_for_server.run();
		//});

		//std::this_thread::sleep_for(std::chrono::seconds(3));

		//boost::asio::io_service io_service_for_client;
		//IPv4_TCP_Client_Transceiver<CommunicationMessage, std::function<void(const boost::system::error_code&)>, std::function<void(const boost::system::error_code&)>> client(io_service_for_client, "localhost", "1004");
		//std::thread client_thread([&io_service_for_client]() {
		//	io_service_for_client.run();
		//});

		//std::this_thread::sleep_for(std::chrono::seconds(2));

		//CommunicationMessage msg;
		//client.AsyncReceive(msg, [&msg](const boost::system::error_code& e) {
		//	if (!e)
		//		std::cout << msg.title << " " << msg.data << std::endl;
		//	else
		//		std::cerr << e.message() << std::endl;
		//});

		//CommunicationMessage msg2;
		//client.AsyncReceive(msg2, [&msg2](const boost::system::error_code& e) {
		//	if (!e)
		//		std::cout << msg2.title << " " << msg2.data << std::endl;
		//	else
		//		std::cerr << e.message() << std::endl;
		//});

		//std::this_thread::sleep_for(std::chrono::seconds(2));

		//client.AsyncSend(CommunicationMessage("Client: ", "Hello"), [](const boost::system::error_code& e) {
		//	if (!e)
		//		std::cout << "Client sent a message" << std::endl;
		//	else
		//		std::cerr << e.message() << std::endl;
		//});

		//std::this_thread::sleep_for(std::chrono::seconds(2));

		//server.AsyncSend(CommunicationMessage("Server: ", "Hello"), [](const boost::system::error_code& e) {
		//	if (!e)
		//		std::cout << "Server sent a message" << std::endl;
		//	else
		//		std::cerr << e.message() << std::endl;
		//});

		//std::this_thread::sleep_for(std::chrono::seconds(2));

		//io_service_for_client.stop();
		//client_thread.join();
		//io_service_for_server.stop();
		//server_thread.join();

		//std::cout << "Test End" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
    return 0;
}

