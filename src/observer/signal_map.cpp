// signal_map.cpp
#ifdef WIN32
#pragma warning( disable : 4786 )
#pragma warning( disable : 4996 )
#pragma warning( disable : 4311 )
#endif

#include "signal_map.h"

using namespace wss;

////////////////////////////////////////////////////////////////////////////////
//
// SignalMap implementation
//
////////////////////////////////////////////////////////////////////////////////

SignalMap::SignalMap(void)
: SignalBaseClass(), Map()
{
	// nothing to do
}


SignalMap::~SignalMap(void)
{
	// for each element in map
	for (CallbackMap::iterator mi = Map.begin(); mi != Map.end(); ++mi)
	{
		List_Clear(&mi->second);	// clear callback list
	}

	Map.clear();						// clear callback map
}


void SignalMap::connect(void *subject, Callback *cb)
{
	// add callback to list for subject object
	Map[(intptr_t)subject].push_back(cb);
}


bool SignalMap::disconnect(void *subject, const Callback *cb)
{
	// find callback list for subject object
	CallbackMap::iterator mi = Map.find((intptr_t)subject);

	// if no callbacks, exit.
	if (mi == Map.end())
		return false;

	CallbackList &l = mi->second;

	// remove callback from list
	bool found = List_Remove(&l, cb);

	// if list is now empty
	if (l.empty())
	{
		// invalidate any active iterators that point to this list.
		// because the list is empty, any active iterators must point
		// to the end iterator.
		IterStack.Invalidate(l.end());

		// remove the list from the map.
		Map.erase((intptr_t)subject);
	}

	return found;
}


void SignalMap::emit(void *subject, void *event, const std::type_info &subject_type, const std::type_info &event_type)
{
	// find callback list for subject object
	CallbackMap::iterator mi = Map.find((intptr_t)subject);

	// if no callbacks, exit.
	if (mi == Map.end())
		return;

	// emit event for every callback in list
	List_Emit(&mi->second, subject, event, subject_type, event_type);
}


