#pragma once
#include "Transmittable.h"
#include "Receivable.h"
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <functional>
#include <sstream>

template<typename Msg>
class TCP_Connection
	: public std::enable_shared_from_this<TCP_Connection<Msg>>
{
public:
	TCP_Connection(boost::asio::io_service& io_service)
		: socket(io_service)
	{

	}

	void Start(void)
	{
		DoRead();
	}

private:
	boost::asio::ip::tcp::socket socket;

	void DoRead(void)
	{
		auto self(shared_from_this());
		socket.async_read(boost::asio::buffer(data_, max_length), [this, self](const boost::system::error_code& ec, std::size_t length)
		{
			if (!ec)
			{
				do_write(length);
			}
		});
	}
};

template<typename Msg, typename SHandler = std::function<void(const boost::system::error_code&)>, typename RHandler = std::function<void(const boost::system::error_code&)>>
class IPv4_TCP_Server_Transceiver :
	public Transmittable<Msg, SHandler>, public Receivable<Msg, RHandler>//, public std::enable_shared_from_this<IPv4_TCP_Server_Transceiver<Msg, SHandler, RHandler>>
{
public:
	IPv4_TCP_Server_Transceiver(boost::asio::io_service& io_service, unsigned short port)
		: acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), socket(io_service)
	{
		AsyncAccept();
	}

	virtual ~IPv4_TCP_Server_Transceiver() = default;

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
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	SHandler shandler;
	RHandler rhandler;

	void AsyncAccept()
	{
		acceptor.async_accept(socket, [this](const auto& ec)
		{
			if (!ec)
			{
				this->socket = std::move(socket);
			}

			AsyncAccept();
		});
		
	}

	//void HandleAccept(const boost::system::error_code& e)
	//{
	//	if (!e)
	//	{
	//		std::cout << "Accepted" << std::endl;
	//	}

	//	AsyncAccept();
	//}
};

