#include "Message.hpp"

TransformationResult strToResult(const std::string& result)
{
	if (result == "FINISHED")
		return TransformationResult::FINISHED;
	else if (result == "INTERRUPED")
		return TransformationResult::INTERRUPED;
	return TransformationResult::TIMEOUT;
}

std::string resultToStr(const TransformationResult result)
{
	switch (result)
	{
	case TransformationResult::FINISHED:
		return "FINISHED";
	case TransformationResult::INTERRUPED:
		return "INTERRUPED";
	case TransformationResult::TIMEOUT:
		return "TIMEOUT";
	default:
		return "ERROR";
	}
}

std::string getMessageHeader(const std::string& data) {
	const auto spacePos = data.find(' ');
	if (spacePos == std::string::npos) {
		return data;
	}
	return data.substr(0, spacePos);
}