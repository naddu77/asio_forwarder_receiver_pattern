#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <deque>
#include "CommunicationMessage.h"

template<typename Msg>
class Participant
{
public:
	virtual ~Participant() = default;
	virtual void Deliver(const Msg& msg) = 0;
};

template<typename Msg>
class Room
{
public:
	void Join(std::shared_ptr<Participant<Msg>> participant)
	{
		participants.insert(participant);
	}

	void Leave(std::shared_ptr<Participant<Msg>> participant)
	{
		participants.erase(participant);
	}

	void Deliver(const Msg& msg)
	{
		for (auto pariticipant : participants)
			pariticipant->Deliver(msg);
	}

private:
	std::set<std::shared_ptr<Participant<Msg>>> participants;
};

template<typename Msg>
class TCP_Session
	: public Participant<Msg>, public std::enable_shared_from_this<TCP_Session<Msg>>
{
public:
	TCP_Session(boost::asio::ip::tcp::socket socket, Room<Msg>& room)
		: socket(std::move(socket)), room(room)
	{

	}

	void Start(void)
	{
		room.Join(shared_from_this());
		DoReadHeader();
	}

	void Deliver(const Msg& msg)
	{
		auto write_in_progress = !write_msgs.empty();
		write_msgs.push_back(msg);
		if (!write_in_progress)
			DoWrite();
	}

private:
	boost::asio::ip::tcp::socket socket;
	Room<Msg>& room;
	Msg read_msg;
	std::deque<Msg> write_msgs;

	void DoReadHeader(void)
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket, boost::asio::buffer(read_msg.Data(), ChatMessage::header_length), [this, self](const auto& ec, std::size_t /*length*/)
		{
			if (!ec && read_msg.DecodeHeader())
				DoReadBody();
			else
				room.Leave(shared_from_this());
		});
	}

	void DoReadBody()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket, boost::asio::buffer(read_msg.Body(), read_msg.BodyLength()), [this, self](const auto& ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				room.Deliver(read_msg);
				DoReadHeader();
			}
			else
				room.Leave(shared_from_this());
		});
	}

	void DoWrite(void)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket, boost::asio::buffer(write_msgs.front().Data(), write_msgs.front().Length()), [this, self](const auto& ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				write_msgs.pop_front();
				if (!write_msgs.empty())
					DoWrite();
			}
			else
				room.Leave(shared_from_this());
		});
	}
};

template<typename Msg>
class TCP_Server
{
public:
	TCP_Server(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint)
		: acceptor(io_service, endpoint), socket(io_service)
	{
		DoAccept();
	}

private:
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
	Room<Msg> room;

	void DoAccept(void)
	{
		acceptor.async_accept(socket, [this](const auto& ec)
		{
			if (!ec)
				std::make_shared<TCP_Session<Msg>>(std::move(socket), room)->Start();
			
			DoAccept();
		});
	}
};