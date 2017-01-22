// ServerTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
//#include <Peer.h>
#include <ChatMessage.h>
#include <TCP_Server.h>
#include <iostream>
#include <thread>


int main()
{
	try
	{
		boost::asio::io_service io_service;

		TCP_Server<ChatMessage> server(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1004));

		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	//try
	//{
	//	Peer server(true);
	//	std::this_thread::sleep_for(std::chrono::seconds(3));

	//	std::cout << "-= Sever Start =-" << std::endl;
	//	system("pause");
	//	server.AsyncReceive();
	//	char line[100];
	//	while (std::cin.getline(line, 100))
	//	{
	//		server.AsyncSend(line);
	//	}

	//	std::cout << "Test End" << std::endl;
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//}

	return 0;
}

