#pragma once

#include <functional>

namespace sGFX
{

template<typename... Args>
struct Signal
{
	std::vector< std::function<void (Args...)> > connections;

	template<typename T> void connect(T to_connect)
	{
		connections.push_back(to_connect);
	};
	template<typename T> bool disconnect(T to_disconnect)
	{
		for(int i = 0; i<connections.size(); i++)
			if(connections[i] == to_disconnect)
			{
				connections[i] = connections.back();
				connections.pop_back();
				return true;
			}
		return false;
	};
	void call(Args... args)
	{
		for(auto& func : connections)
			if(func)
				func(args...);
	}
};

}