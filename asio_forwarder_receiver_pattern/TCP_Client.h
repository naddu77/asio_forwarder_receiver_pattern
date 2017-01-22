#pragma once

#include <deque>
#include <boost/asio.hpp>

template<typename Msg>
class TCP_Client
{
public:
	TCP_Client(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
		: io_service(io_service), socket(io_service)
	{
		DoConnect(endpoint_iterator);
	}

	void Write(const Msg& msg)
	{
		io_service.post([this, msg]()
		{
			auto write_in_progress = !write_msgs.empty();
			write_msgs.push_back(msg);
			if (!write_in_progress)
				DoWrite();
		});
	}

	void Close()
	{
		io_service.post([this]() { socket.close(); });
	}

private:
	void DoConnect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	{
		boost::asio::async_connect(socket, endpoint_iterator,
			[this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
		{
			if (!ec)
				DoReadHeader();
		});
	}

	void DoReadHeader()
	{
		boost::asio::async_read(socket, boost::asio::buffer(read_msg.Data(), ChatMessage::header_length), [this](const auto& ec, std::size_t /*length*/)
		{
			if (!ec && read_msg.DecodeHeader())
				DoReadBody();
			else
				socket.close();
		});
	}

	void DoReadBody()
	{
		boost::asio::async_read(socket, boost::asio::buffer(read_msg.Body(), read_msg.BodyLength()), [this](const auto& ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				std::cout.write(read_msg.Body(), read_msg.BodyLength());
				std::cout << "\n";
				DoReadHeader();
			}
			else
				socket.close();
		});
	}

	void DoWrite()
	{
		boost::asio::async_write(socket, boost::asio::buffer(write_msgs.front().Data(), write_msgs.front().Length()), [this](const auto& ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				write_msgs.pop_front();
				if (!write_msgs.empty())
					DoWrite();
			}
			else
				socket.close();
		});
	}

private:
	boost::asio::io_service& io_service;
	boost::asio::ip::tcp::socket socket;
	Msg read_msg;
	std::deque<Msg> write_msgs;
};