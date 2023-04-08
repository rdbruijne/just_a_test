#pragma once

// C++
#include <string>

class Named
{
public:
	Named() = default;
	explicit Named(const std::string& name) : mName(name) {}
	virtual ~Named() {}

	const std::string& Name() const { return mName; }
	virtual void SetName(const std::string& name) { mName = name; }

protected:
	std::string mName = "";
};
