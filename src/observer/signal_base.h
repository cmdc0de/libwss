// signal_base.h

#ifndef SIGNAL_BASE_H
#define SIGNAL_BASE_H

#include <list>
#include "stl_iter_stack_list.h"
#include <typeinfo>

#ifdef WIN32
#include <hash_map>
#define HASH_MAP_NS stdext
#else
#include <ext/hash_map>
#define HASH_MAP_NS __gnu_cxx
#endif

namespace wss {

class SignalBaseClass
{
	protected:
		SignalBaseClass(void);
		~SignalBaseClass(void);

		class Callback
		{
			// This class defines the abstract callback interface.
			public:
				virtual ~Callback(void)																	{ }

				// these accessors return type information about the event and callback object.
				virtual const std::type_info &	Get_Subject_Type	(void) const						= 0;
				virtual const std::type_info &	Get_Event_Type		(void) const						= 0;
				virtual const std::type_info &	Get_Type				(void) const						= 0;

				// returns true if this equals cb, false otherwise.
				virtual bool					Is_Equal				(const Callback *cb)	const		= 0;
				
				// runs the callback with given subject and event.
				// INVARIANT: subject and event match types used in derived callback.
				virtual void					Execute				(void *subject, void *event)	= 0;
		};


		template<class UserEvent>
			class CallbackImpl : public Callback
		{
			// This class handles common implementation details of the callback interface.
			public:
				virtual const std::type_info &	Get_Event_Type		(void) const						{ return typeid(UserEvent);							}
				virtual bool					Is_Equal				(const Callback *cb) const		{ return ((Get_Type() == cb->Get_Type()) != 0);	}
		};


		template<class Subject, class Observer, class MemFunc, class UserEvent>
			class MemberCallback : public CallbackImpl<UserEvent>
		{
			// This class calls a member function in response to an event.
			public:
				MemberCallback(Observer *observer, MemFunc mem_func);

				virtual const std::type_info &	Get_Subject_Type	(void) const						{ return typeid(Subject);	}
				virtual const std::type_info &	Get_Type				(void) const						{ return typeid(this);		}
				virtual bool					Is_Equal				(const Callback *cb) const;
				virtual void					Execute				(void *subject, void *event);

			private:
				Observer *	m_observer;
				MemFunc		m_mem_func;
		};


		template<class Subject, class Context, class FreeFunc, class UserEvent>
			class FreeCallback : public CallbackImpl<UserEvent>
		{
			// This class calls a free function in response to an event.
			public:
				FreeCallback(Context context, FreeFunc free_func);

				virtual const std::type_info &	Get_Subject_Type	(void) const						{ return typeid(Subject);	}
				virtual const std::type_info &	Get_Type				(void) const						{ return typeid(this);		}
				virtual bool					Is_Equal				(const Callback *cb)	const;
				virtual void					Execute				(void *subject, void *event);

			private:
				Context		m_context;
				FreeFunc		m_free_func;
		};

		// callback list manipulation
		typedef std::list<Callback *>								CallbackList;			// list of callback objects

		void List_Emit		(CallbackList *l, void *subject, void *event, const std::type_info &subject_type, const std::type_info &event_type);
		bool List_Remove	(CallbackList *l, const Callback *cb);
		void List_Clear	(CallbackList *l);


		// callbacks can emit other events as well as cause disconnection.
		// we must therefore keep track of every iterator in use on the 
		// callback lists. If we remove a callback currently pointed
		// to by an iterator, we need to advance the iterator. If we 
		// remove the entire list, we need to invalidate the iterator.

		// invariant: any iterator on the stack points to the next callback
		// for an active Emit call. This allows us to advance the iterator
		// if necessary.

		// note: iterators on the stack are modified in place.

		typedef IterStackList<CallbackList>		IterStackType;
		IterStackType			IterStack;
};

#include "signal_base.inl"
}  //namespace

#endif // SIGNAL_BASE_H
