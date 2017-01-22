#pragma once
#include "Transmittable.h"
#include "Receivable.h"
#include "CommunicationMessage.h"
#include "IPv4_TCP_Server_Transceiver.h"
#include "IPv4_TCP_Client_Transceiver.h"

#include <memory>
#include <thread>

class Peer
{
public:
	Peer(bool is_server)
	{
		if (is_server)
		{
			std::shared_ptr<IPv4_TCP_Server_Transceiver<CommunicationMessage>> ptr(new IPv4_TCP_Server_Transceiver<CommunicationMessage>(io_service, 1007));
			t = ptr;
			r = ptr;
		}
		else
		{
			std::shared_ptr<IPv4_TCP_Client_Transceiver<CommunicationMessage>> ptr(new IPv4_TCP_Client_Transceiver<CommunicationMessage>(io_service, "localhost", "1007"));
			t = ptr;
			r = ptr;
		}

		my_thread = std::thread([this]() {
			io_service.run();
		});
	}

	~Peer()
	{
		io_service.stop();
		my_thread.join();
	}

	void AsyncSend(const std::string& msg)
	{
		t->AsyncSend(CommunicationMessage("Peer", msg), [](const auto& e) {
			if (!e)
			{
				std::cout << "Sent" << std::endl;
			}
			else
			{
				std::cerr << e.message() << std::endl;
			}
		});
	}

	void AsyncReceive(void)
	{
		r->AsyncReceive(communication_message, [this](const auto& e) {
			if (!e)
			{
				std::cout << communication_message.title << " " << communication_message.data << std::endl;

				AsyncReceive();
			}
			else
			{
				std::cerr << e.message() << std::endl;
			}
		});
	}

	boost::asio::io_service io_service;

private:
	std::shared_ptr<Transmittable<CommunicationMessage>> t;
	std::shared_ptr<Receivable<CommunicationMessage>> r;
	std::thread my_thread;
	CommunicationMessage communication_message;

	
};

