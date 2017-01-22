#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

class ChatMessage
{
public:
	enum { header_length = 4 };
	enum { max_body_length = 512 };

	ChatMessage()
		: body_length(0)
	{
	}

	const char* Data() const
	{
		return data;
	}

	char* Data()
	{
		return data;
	}

	std::size_t Length() const
	{
		return header_length + body_length;
	}

	const char* Body() const
	{
		return data + header_length;
	}

	char* Body()
	{
		return data + header_length;
	}

	std::size_t BodyLength() const
	{
		return body_length;
	}

	void BodyLength(std::size_t new_length)
	{
		body_length = new_length;
		if (body_length > max_body_length)
			body_length = max_body_length;
	}

	bool DecodeHeader()
	{
		char header[header_length + 1] = "";
		std::strncat(header, data, header_length);
		body_length = std::atoi(header);
		if (body_length > max_body_length)
		{
			body_length = 0;
			return false;
		}
		return true;
	}

	void EncodeHeader()
	{
		char header[header_length + 1] = "";
		std::sprintf(header, "%4d", static_cast<int>(body_length));
		std::memcpy(data, header, header_length);
	}

private:
	char data[header_length + max_body_length];
	std::size_t body_length;
};