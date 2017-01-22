// ClientTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
//#include <Peer.h>
#include <ChatMessage.h>
#include <TCP_Client.h>
#include <iostream>
#include <thread>


int main()
{
	try
	{
		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve({ "localhost", "1004" });
		TCP_Client<ChatMessage> client(io_service, endpoint_iterator);

		std::thread t([&io_service]() { io_service.run(); });

		char line[ChatMessage::max_body_length + 1];
		while (std::cin.getline(line, ChatMessage::max_body_length + 1))
		{
			ChatMessage msg;
			msg.BodyLength(std::strlen(line));
			std::memcpy(msg.Body(), line, msg.BodyLength());
			msg.EncodeHeader();
			client.Write(msg);
		}

		client.Close();
		t.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	//try
	//{
	//	Peer client(false);

	//	std::cout << "-= Client Start =-" << std::endl;
	//	system("pause");
	//	client.AsyncReceive();
	//	char line[100];
	//	while (std::cin.getline(line, 100))
	//	{
	//		client.AsyncSend(line);
	//	}

	//	std::cout << "Test End" << std::endl;
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//}

	return 0;
}

