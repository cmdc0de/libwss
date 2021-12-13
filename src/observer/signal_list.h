// signal_list.h

#ifndef SIGNAL_LIST_H
#define SIGNAL_LIST_H

#include "signal_base.h"

namespace wss {

class SignalList : public SignalBaseClass
{
	// SUMMARY:
	// The SignalList class implements the Observer pattern. Unlike the
	// SignalMap, listeners are not bound to a particular subject.
	// Instead, they receive all events emits on a particular SignalList.

	// Callback functions must have a specific signature. All callback
	//	functions, member or free, return void. (If callbacks must return
	// a value, they can modify a field in the event.) A member callback 
	// takes a pointer to an event and a pointer to a subject. The event 
	// and the subject can have any type. A free callback takes an additional 
	// context parameter.

	// EXAMPLE:
	//		SignalList Signal;
	//
	//		class ActionEvent {
	//			// event specific state
	//		};
	//
	//		class Subject {
	//		public:
	//			void Action(void)	{ 
	//				// perform action
	//
	//				// update observers
	//				Signal.Emit(this, &ActionEvent()) 
	//			}
	//		};
	//
	//		class Observer {
	//		public:
	//			void On_Action(ActionEvent *e, Subject *s) {
	//				// update state based on event
	//			}
	//		};
	//
	//		void Test_Member_Callback(void) {
	//			Subject s;
	//			Observer o;
	//
	//			Signal.Connect_Member(&o, &Observer::On_Action, SignalList::Binder<Subject, ActionEvent>());
	//			s.Action();
	//			Signal.Disconnect_Member(&o, &Observer::On_Action, SignalList::Binder<Subject, ActionEvent>());
	//		}
	//
	//		class Context { };
	//
	//		void On_Action_Function(ActionEvent *e, Subject *s, Context *c) {
	//			// use context to respond to action
	//		}
	//
	//		void Test_Free_Callback(void) {
	//			Context c;
	//			
	//			Signal.Connect_Free(&c, &On_Action_Function, SignalList::Binder<Subject, ActionEvent>());
	//			s.Action();
	//			Signal.Disconnect_Free(&c, &On_Action_Function, SignalList::Binder<Subject, ActionEvent>());
	//		}

	public:
		template<class Subject, class Event>
			struct Binder 
		{
			typedef Subject	SubjectType;
			typedef Event		EventType;
		};

		SignalList(void);
		~SignalList(void);

		// connect a member function callback to a subject
		template<class Observer, class MemFunc, class Bind>
			void Add_Member_Callback(Observer *observer, MemFunc callback, const Bind &b)
		{
			typedef typename Bind::SubjectType	SubjectType;
			typedef typename Bind::EventType	EventType;
			Callback *cb = new MemberCallback<SubjectType, Observer, MemFunc, EventType>(observer, callback);
			add(cb);
		}

		// disconnect a member function callback from a subject
		template<class Observer, class MemFunc, class Bind>
			bool Disconnect_Member_Callback(Observer *observer, MemFunc mem_func, const Bind &b)
		{
			typedef typename Bind::SubjectType	SubjectType;
			typedef typename Bind::EventType	EventType;
			MemberCallback<SubjectType, Observer, MemFunc, EventType> cb(observer, mem_func);
			return remove(&cb);
		}

		// connect a free function callback to a subject
		template<class Context, class FreeFunc, class Bind>
			void Add_Free_Callback(Context context, FreeFunc free_func, const Bind &b)
		{
			typedef typename Bind::SubjectType	SubjectType;
			typedef typename Bind::EventType	EventType;
			Callback *cb = new FreeCallback<SubjectType, Context, FreeFunc, EventType>(context, free_func);
			add(cb);
		}
		
		// disconnect a free function callback from a subject
		template<class Context, class FreeFunc, class Bind>
			bool Remove_Free_Callback(Context context, FreeFunc free_func, const Bind &b)
		{
			typedef typename Bind::SubjectType	SubjectType;
			typedef typename Bind::EventType	EventType;
			FreeCallback<SubjectType, Context, FreeFunc, EventType> cb(context, free_func);
			return remove(&cb);
		}

		// dispatch an event from a subject to all connected observers
		template<class Subject, class UserEvent>
			void Emit(Subject *subject, UserEvent *event)
		{
			emit(subject, event, typeid(Subject), typeid(UserEvent));
		}


	private:
		// helper functions that operate on generic pointers and callback objects
		void add		(Callback *cb);
		void emit	(void *subject, void *event, const std::type_info &subject_type, const std::type_info &event_type);
		bool remove	(const Callback *cb);

		typedef HASH_MAP_NS::hash_map<int, CallbackList>		CallbackMap;
		//typedef __gnu_cxx::hash_map<int, CallbackList>		CallbackMap;
		CallbackMap		Map;
};

} //namespace

#endif // SIGNAL_LIST_H
