#include "Stopwatch.h"

Stopwatch::Stopwatch()
{
	Reset();
}



void Stopwatch::Reset()
{
	mTimePoint = clock::now();
}



int64_t Stopwatch::ElapsedNs() const
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - mTimePoint).count();
}



std::string Stopwatch::ElapsedString() const
{
	const int64_t t = ElapsedNs();

	char text[64];
	if(t < 1'000)
	{
		sprintf_s(text, "%lld ns", t);
	}
	else if(t < 1'000'000)
	{
		sprintf_s(text, "%lld.%01lld us", t / 1'000, (t / 100) % 10);
	}
	else if(t < 1'000'000'000)
	{
		sprintf_s(text, "%lld.%01lld ms", t / 1'000'000, (t / 100'000) % 10);
	}
	else if(t < 60'000'000'000)
	{
		sprintf_s(text, "%lld.%01lld s", t / 1'000'000'000, (t / 100'000'000) % 10);
	}
	else
	{
		// (hh:)mm:ss format
		const int64_t t2 = t / 1'000'000'000;
		if(t2 < 3600)
		{
			sprintf_s(text, "%02lld:%02lld", t2 / 60, t2 % 60);
		}
		else
		{
			sprintf_s(text, "%lld:%02lld:%02lld", t2 / 3600, (t2 % 3600) / 60, t2 % 60);
		}
	}

	return std::string(text);
}
