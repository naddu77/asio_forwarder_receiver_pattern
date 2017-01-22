#pragma once
#include <vector>
#include <tuple>
#include <iostream>
#include <boost/asio.hpp>

template<typename Msg, typename Handler = std::function<void(const boost::system::error_code&)>>
class Receivable
{
public:
	Receivable() = default;
	virtual ~Receivable() = default;

	virtual void AsyncReceive(Msg& msg, Handler handler) = 0;

	void HandleReadHeader(const boost::system::error_code& e, Msg& msg, std::tuple<Handler> handler)
	{
		if (e)
		{
			std::get<0>(handler)(e);
		}
		else
		{
			// Determine the length of the serialized data.
			std::istringstream is(std::string(inbound_header, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				std::get<0>(handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			inbound_data.resize(inbound_data_size);
			AsyncReadData(msg, handler);
		}
	}

	void HandleReadData(const boost::system::error_code& e, Msg& msg, std::tuple<Handler> handler)
	{
		if (e)
		{
			std::get<0>(handler)(e);
		}
		else
		{
			// Extract the data structure from the data just received.
			try
			{
				std::string archive_data(&inbound_data[0], inbound_data.size());
				std::istringstream archive_stream(archive_data);
				boost::archive::text_iarchive archive(archive_stream);
				archive >> msg;
			}
			catch (std::exception& e)
			{
				// Unable to decode data.
				std::cerr << e.what() << std::endl;
				boost::system::error_code error(boost::asio::error::invalid_argument);
				std::get<0>(handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			std::get<0>(handler)(e);
		}
	}

protected:
	/// The size of a fixed length header.
	enum { header_length = 8 };

	/// Holds an inbound header.
	char inbound_header[header_length];

	/// Holds the inbound data.
	std::vector<char> inbound_data;

	virtual void AsyncReadData(Msg& msg, std::tuple<Handler> handler) = 0;

	
};

