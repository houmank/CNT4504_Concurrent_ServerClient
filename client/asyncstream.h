/*
 * osyncstream - A wrapper around ostream that properly locks while consuming stream operators
 * Copyright Riley Strickland 2021
 */
#pragma once
#include <mutex>
#include <ostream>
#include <iostream>

class osyncstream_locked final
{
    std::unique_lock<std::recursive_mutex> lock_;
    std::ostream& stream_;
public:
    osyncstream_locked(std::recursive_mutex& mtx, std::ostream& stream) : lock_(mtx), stream_(stream) {} //construct and lock the mutex from osyncstream.
    osyncstream_locked(const osyncstream_locked& o) = delete;
	/* A recursive mutex isn't great here (reference forwarding would be better except...only a recursive mutex can assure lock-before-release on ownership transfer */
    template<typename T>
    friend auto operator<<(osyncstream_locked&& os, const T& d) -> osyncstream_locked&&
    {
        os.stream_ << d;
        return std::move(os); /* move semantics :( ... thanks Clang/GCC */
    }
    friend auto operator<<(osyncstream_locked&& os, std::ostream& (*func)(std::ostream&)) -> std::ostream&
    {
        return os.stream_ << func;
    }
	osyncstream_locked(osyncstream_locked&& o) noexcept : osyncstream_locked(*o.lock_.mutex(), o.stream_) {} //could be better... max depth 2, usual depth 1.
    osyncstream_locked& operator=(osyncstream_locked&) = delete; //disallow
    osyncstream_locked& operator=(osyncstream_locked&&) = delete; //disallow
    ~osyncstream_locked() = default;
};

class osyncstream final
{
    std::recursive_mutex mutex_{};
    std::ostream& stream_;
public:
    explicit osyncstream(std::ostream& stream) : stream_(stream) {}
    template<typename T>
    friend auto operator<<(osyncstream& os, const T& d) -> osyncstream_locked
    {
        std::unique_lock<std::recursive_mutex> lock(os.mutex_);
        os.stream_ << d;
        return osyncstream_locked { os.mutex_, os.stream_ };
    }
    friend auto operator<<(osyncstream& os, std::ostream& (*func)(std::ostream&)) -> osyncstream_locked
    {
        std::unique_lock<std::recursive_mutex> lock(os.mutex_);
        os.stream_ << func;
        return osyncstream_locked{ os.mutex_, os.stream_ };
    }
};

struct streams
{
    static std::ostream& cout;
    static std::ostream& cerr;
};

inline std::ostream& streams::cout = std::cout;
inline std::ostream& streams::cerr = std::cerr;
inline osyncstream sscout(streams::cout);
inline osyncstream sscerr(streams::cerr);