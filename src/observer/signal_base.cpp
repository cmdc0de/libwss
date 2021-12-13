// signal_base.cpp

#include "signal_base.h"

using namespace wss;

SignalBaseClass::SignalBaseClass(void)
: IterStack()
{
}


SignalBaseClass::~SignalBaseClass(void)
{
}


void SignalBaseClass::List_Clear(CallbackList *l)
{
	for (CallbackList::iterator li = l->begin(); li != l->end(); ++li) 
	{
		// invalidate any iterator pointing to this element.
		// normally, we would call IterStack.Advance, but we
		// know the whole list is going away in a moment. 
		IterStack.Invalidate(li);

		// delete the callback object
		delete *li;
	}

	l->clear();		// clear the callback list
}


void SignalBaseClass::List_Emit(CallbackList *l, void *subject, void *event, const std::type_info & /*subject_type*/, const std::type_info &event_type)
{
	// Note: the structure of this loop is important.
	// We must deal with callbacks that call Disconnect.
	// Consequently, we must track all our iterators and
	// invalidate them if necessary. Before the callback,
	// we add the iterator to a stack. If Disconnect is
	// called, it will scan the iterator stack and advance
	// any iterators that point to the removed callback.
	// If the entire list is removed, any active iterators
	// will be invalidated.

	CallbackList::iterator li = l->begin();
	while (li != l->end()) 
	{
		Callback *cb = *li;

		// if callback wants this event
		if (cb->Get_Event_Type() == event_type) 
		{
			// Add iterator to stack for tracking. Increment it so it
			// points to our next callback. (see header comments
			// in IterStack section for reason)
			// NOTE: we're pushing a pointer onto the stack; it will
			// be updated for us semi-magically.
			IterStack.Push(&(++li));

			// notify the callback function
			cb->Execute(subject, event);

			// IMPORTANT: do not use l or cb after this point. 
			// Both may have been deleted in call above.

			// pop iterator from stack. If iterator is not valid,
			// the entire callback list has been removed, and we
			// need to exit the function immediately.
			if (!IterStack.Pop()) return;
		} 
		else 
		{
			// otherwise, move on to next callback
			++li;
		}
	}
}


bool SignalBaseClass::List_Remove(CallbackList *l, const Callback *cb)
{
	// for each callback in list
	for (CallbackList::iterator li = l->begin(); li != l->end(); ++li)
	{
		// if callback matches, we need to remove it
		if (cb->Is_Equal(*li))
		{
			// advance any active iterators that point to this callback.
			IterStack.Advance(li);

			delete *li;

			// remove the callback from the list
			l->erase(li);

			// successfully disconnected.
			// return here because the iterator li is no longer valid.
			// if you change this behavior, you'll need to reexamine
			// the looping constructs.
			return true;
		}
	}

	return false;
}
