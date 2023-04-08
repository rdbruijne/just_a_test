#pragma once

#define NO_COPY_ALLOWED(c)				\
	c(const c&) = delete;				\
	c& operator =(const c&) = delete;

// mark variables as used
inline void MarkVariablesUsed(...) {}
