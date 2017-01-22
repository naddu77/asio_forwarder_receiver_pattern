#pragma once
#include <string>
#include <boost/asio.hpp>

template<typename Msg, typename Handler = std::function<void(const boost::system::error_code&)>>
class Transmittable
{
public:
	Transmittable() = default;
	virtual ~Transmittable() = default;

	void AsyncSend(const Msg& msg, Handler handler)
	{
		// Serialize the data first so we know how large it is.
		std::ostringstream archive_stream;
		boost::archive::text_oarchive archive(archive_stream);
		archive << msg;
		outbound_data = archive_stream.str();

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::setw(header_length)
			<< std::hex << outbound_data.size();
		if (!header_stream || header_stream.str().size() != header_length)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			//socket.get_io_service().post(std::bind(handler, error));
			return;
		}
		outbound_header = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header));
		buffers.push_back(boost::asio::buffer(outbound_data));
		AsyncSendImpl(buffers, std::make_tuple(handler));
	}

	virtual void AsyncSendImpl(const std::vector<boost::asio::const_buffer>& buffers, std::tuple<Handler> handler) = 0;

protected:
	/// The size of a fixed length header.
	enum { header_length = 8 };

	/// Holds an outbound header.
	std::string outbound_header;

	/// Holds the outbound data.
	std::string outbound_data;
};

