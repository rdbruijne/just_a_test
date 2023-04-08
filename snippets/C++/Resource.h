#pragma once

// Project
#include "Defilable.h"
#include "Named.h"
#include "Syncable.h"

// C++
#include <memory>
#include <string>
#include <vector>

class Resource : public Defilable, public Named, public Syncable
{
public:
	Resource() = default;
	explicit Resource(const std::string& name) : Named(name) {}
	virtual ~Resource() {}

	// dirty
	bool IsDirty() const override { return IsDirty(true); }
	bool IsDirty(bool parseDependencies) const;

	// dependencies
	void AddDependency(std::shared_ptr<Resource> dependency);
	void RemoveDependency(std::shared_ptr<Resource> dependency);

protected:
	std::vector<std::weak_ptr<Resource>> mDependencies;
};
