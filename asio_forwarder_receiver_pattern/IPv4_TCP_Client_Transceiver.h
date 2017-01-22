#pragma once
#include "Transmittable.h"
#include "Receivable.h"
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <functional>
#include <sstream>

#include <string>

template<typename Msg, typename SHandler = std::function<void(const boost::system::error_code&)>, typename RHandler = std::function<void(const boost::system::error_code&)>>
class IPv4_TCP_Client_Transceiver :
	public Transmittable<Msg, SHandler>, public Receivable<Msg, RHandler>
{
public:
	IPv4_TCP_Client_Transceiver(boost::asio::io_service& io_service, const std::string& host, const std::string& service)
		: socket(io_service)
	{
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(host, service);
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		// Start an asynchronous connect operation.
		boost::asio::async_connect(socket, endpoint_iterator, std::bind(&IPv4_TCP_Client_Transceiver::HandleConnect, this, std::placeholders::_1));
	}

	virtual ~IPv4_TCP_Client_Transceiver() = default;

	virtual void AsyncReceive(Msg& msg, RHandler handler) override
	{
		boost::asio::async_read(socket, boost::asio::buffer(inbound_header),
			std::bind(&Receivable::HandleReadHeader, this, std::placeholders::_1, std::ref(msg), std::make_tuple(handler)));
	}

protected:
	virtual void AsyncReadData(Msg& msg, std::tuple<RHandler> handler) override
	{
		boost::asio::async_read(socket, boost::asio::buffer(inbound_data),
			std::bind(&Receivable::HandleReadData, this, std::placeholders::_1, std::ref(msg), handler));
	}

	virtual void AsyncSendImpl(const std::vector<boost::asio::const_buffer>& buffers, std::tuple<SHandler> handler) override
	{
		boost::asio::async_write(socket, buffers, std::bind(std::get<0>(handler), std::placeholders::_1));
	}

private:
	boost::asio::ip::tcp::socket socket;

	void HandleConnect(const boost::system::error_code& e)
	{
		if (!e)
		{
			std::cout << "Connected" << std::endl;
		}
		else
		{
			std::cerr << e.message() << std::endl;
		}
	}

};

