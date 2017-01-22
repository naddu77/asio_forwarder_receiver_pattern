#pragma once
#include <string>

class CommunicationMessage
{
public:
	std::string title;
	std::string data;

	CommunicationMessage() = default;
	CommunicationMessage(const std::string& title, const std::string& data)
		: title(title), data(data)
	{

	}

	~CommunicationMessage() = default;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & title;
		ar & data;
	}
};

