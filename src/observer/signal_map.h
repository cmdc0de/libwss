// signal_map.h

#ifndef SIGNAL_MAP_H
#define SIGNAL_MAP_H

#include "signal_base.h"

namespace wss {

class SignalMap : public SignalBaseClass
{
	// SUMMARY:
	// The Signal class implements the Observer pattern. Unlike
	// the implementation in Design Patterns, the Signal class allows
	// the mapping between subject and observer to be maintained
	// outside the participating objects. Neither class is required
	// to inherit from a particular class. Instead, any member
	// or free function can be registered as a callback function.
	// This allows new classes to be added to an observer relationship
	// with minimal code changes.

	// Callback functions must have a specific signature. All callback
	//	functions, member or free, return void. (If callbacks must return
	// a value, they can modify a field in the event.) A member callback 
	// takes a pointer to an event and a pointer to a subject. The event 
	// and the subject can have any type. A free callback takes an additional 
	// context parameter.

	// EXAMPLE:
	//		SignalMap Signal;
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
	//			Signal.Connect_Member(&s, &o, &Observer::On_Action, ActionEvent());
	//			s.Action();
	//			Signal.Disconnect_Member(&s, &o, &Observer::On_Action, ActionEvent());
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
	//			Signal.Connect_Free(&s, &c, &On_Action_Function, ActionEvent());
	//			s.Action();
	//			Signal.Disconnect_Free(&s, &c, &On_Action_Function, ActionEvent());
	//		}

	// NOTES:
	//	The Signal class tracks objects by their pointers. This raises concerns
	// when copying and destroying objects. If you copy a subject, the observers 
	// will not be transferred without explicitly connecting them to the new copy. 
	// Because many different types can be connected as observers, implementing a 
	// method to retrieve observers is non-trivial. The easiest way to transfer 
	// listeners is to emit a copy event from the source that contains the pointer 
	// of the new object. This allows observers to implement their own copying policy.

	// When destroying an object, be careful to disconnect all observers from it.
	// Otherwise, if the address is reused, old observers may receive spurious events.
	// The easiest way to accomplish this is using a deletion event.

	// The Signal class is implemented in terms of generic pointers. Therefore, the caller
	// must ensure pointers of the appropriate type are presented to the Signal functions.
	// This is most important when a subject is multiply derived. 

	//		Example:
	//			SignalMap Signal;
	//
	//			// an event associated with BaseSubject2
	//			class ActionEvent { };
	//
	//			class BaseSubject1 { 
	//			public:
	//				int x; 
	//			};
	//
	//			class BaseSubject2 { 
	//			public:
	//				void Action1(void) {
	//					Signal.Emit(this, &ActionEvent());
	//				}
	//	
	//				int y; 
	//			};
	//
	//			class DerivedSubject : public BaseSubject1, public BaseSubject2 {
	//			public:
	//				void Action2(void);
	//			};
	//
	//			class Observer {
	//			public:
	//				void On_Action(ActionEvent *e, BaseSubject2 *s) { }
	//			};
	//
	//			void Test_Multiple_Inheritance(void) {
	//				DerivedSubject s;
	//				Observer o;
	//				
	//				Signal.Connect_Member(&s,						&o, &Observer::On_Action, ActionEvent()); // runtime error: only listening for events generated at DerivedSubject level
	//				Signal.Connect_Member((BaseSubject2 *)&s,	&o, &Observer::On_Action, ActionEvent()); // correct
	//
	//				s.Action1();
	//				s.Action2();
	//			}
	//		
	//			void DerivedSubject::Action2(void) {
	//				Signal.Emit(this,							&ActionEvent());	// runtime error: Because (BaseSubject2 *)this != this, only observers listening for DerivedSubject events will be notified
	//				Signal.Emit((BaseSubject2 *)this,	&ActionEvent());	// correct
	//			}

	//	Even though SignalClass uses the typeid operator, it does not rely on Run-Time Type
	// Information (RTTI) support. Instead, it uses typeid to carry type-specific information
	// through virtual interfaces as a string.


	protected:
	public:
		SignalMap(void);
		~SignalMap(void);

		// connect a member function callback to a subject
		template<class Subject, class Observer, class MemFunc, class UserEvent>
			void Connect_Member(Subject *subject, Observer *observer, MemFunc callback, const UserEvent &event)
		{
			Callback *cb = new MemberCallback<Subject, Observer, MemFunc, UserEvent>(observer, callback);
			connect(subject, cb);
		}

		// disconnect a member function callback from a subject
		template<class Subject, class Observer, class MemFunc, class UserEvent>
			bool Disconnect_Member(Subject *subject, Observer *observer, MemFunc mem_func, const UserEvent &event)
		{
			MemberCallback<Subject, Observer, MemFunc, UserEvent> cb(observer, mem_func);
			return disconnect(subject, &cb);
		}

		// connect a free function callback to a subject
		template<class Subject, class Context, class FreeFunc, class UserEvent>
			void Connect_Free(Subject *subject, Context context, FreeFunc free_func, const UserEvent &event)
		{
			Callback *cb = new FreeCallback<Subject, Context, FreeFunc, UserEvent>(context, free_func);
			connect(subject, cb);
		}
		
		// disconnect a free function callback from a subject
		template<class Subject, class Context, class FreeFunc, class UserEvent>
			bool Disconnect_Free(Subject *subject, Context context, FreeFunc free_func, const UserEvent &event)
		{
			FreeCallback<Subject, Context, FreeFunc, UserEvent> cb(context, free_func);
			return disconnect(subject, &cb);
		}

		// dispatch an event from a subject to all connected observers
		template<class Subject, class UserEvent>
			void Emit(Subject *subject, UserEvent *event)
		{
			emit(subject, event, typeid(Subject), typeid(UserEvent));
		}


	private:
		// helper functions that operate on generic pointers and callback objects
		void connect	(void *subject, Callback *cb);
		void emit		(void *subject, void *event, const std::type_info &subject_type, const std::type_info &event_type);
		bool disconnect(void *subject, const Callback *cb);

		typedef HASH_MAP_NS::hash_map<intptr_t, CallbackList>			CallbackMap;			// maps subject to callback list
		CallbackMap			Map;
};

} ///namespace

#endif // SIGNAL_MAP_H
