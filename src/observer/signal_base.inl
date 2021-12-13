// signal_base.inl

#ifndef SIGNAL_BASE_INL
#define SIGNAL_BASE_INL

#ifndef SIGNAL_BASE_H
#error "This file should only be included by signal_base.h"
#endif


template<class Subject, class Observer, class MemFunc, class UserEvent>
SignalBaseClass::MemberCallback<Subject, Observer,MemFunc, UserEvent>::MemberCallback(Observer *observer, MemFunc mem_func)
: m_observer(observer), m_mem_func(mem_func) 
{ 
}


template<class Subject, class Observer, class MemFunc, class UserEvent>
bool SignalBaseClass::MemberCallback<Subject, Observer,MemFunc, UserEvent>::Is_Equal(const SignalBaseClass::Callback *cb) const
{
	// check base type information
	if (!CallbackImpl<UserEvent>::Is_Equal(cb)) 
		return false;

	// callbacks have same type. Cast the parameter up.
	MemberCallback *mcb = (MemberCallback *)cb;

	// check instance data.
	return ((m_observer == mcb->m_observer) && (m_mem_func== mcb->m_mem_func));
}


template<class Subject, class Observer, class MemFunc, class UserEvent>
void SignalBaseClass::MemberCallback<Subject, Observer,MemFunc, UserEvent>::Execute(void *subject, void *event)
{
	// convert generic pointers to proper types, then call member function.
	(*m_observer.*m_mem_func)((UserEvent *)event, (Subject *)subject);
}					


template<class Subject, class Context, class FreeFunc, class UserEvent>
SignalBaseClass::FreeCallback<Subject,Context, FreeFunc, UserEvent>::FreeCallback(Context context, FreeFunc free_func)
: m_context(context), m_free_func(free_func) 
{ 
}


template<class Subject, class Context, class FreeFunc, class UserEvent>
bool SignalBaseClass::FreeCallback<Subject,Context, FreeFunc, UserEvent>::Is_Equal(const Callback *cb) const
{
	// check base type information
	if (!CallbackImpl<UserEvent>::Is_Equal(cb)) 
		return false;
	
	// callbacks have same type. Cast the parameter up.
	FreeCallback *fcb = (FreeCallback *)cb;

	// check instance data
	return ((m_context == fcb->m_context) && (m_free_func == fcb->m_free_func));
}


template<class Subject, class Context, class FreeFunc, class UserEvent>
void SignalBaseClass::FreeCallback<Subject,Context, FreeFunc, UserEvent>::Execute(void *subject, void *event)
{
	// convert generic pointers to proper types, then call free function
	m_free_func((UserEvent *)event, (Subject *)subject, m_context);
}					




#endif // SIGNAL_BASE_INL
