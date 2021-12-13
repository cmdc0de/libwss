// stl_iter_stack_list.h

#ifndef STL_ITER_STACK_LIST_H
#define STL_ITER_STACK_LIST_H

#include <vector>

namespace wss {

//#ifdef _MSC_VER
//template <typename ListType, typename IterType = ListType::iterator>
//#else
template <typename ListType, typename IterType = typename ListType::iterator>
//#endif

class IterStackList
{
	// This class provides an abstraction for managing iterators
	// at different depths in the call graph, updating them
	// when elements are removed or the list is deleted.

	// EXAMPLE:

	// We have an object Foo that keeps a list of Bar objects.
	// Foo has a member function that iterates the list, calling
	// a member function F of Bar. Unfortunately, Bar::F can call
	// functions in Foo that modify the list. How do we ensure that
	// elements are not deleted out from an iterator?

	// class Bar 
	// {
	//		public:
	//			void F(void) { // modify Foo's list of objects by calling Foo::Remove() }
	// };
	//
	// class Foo
	// {
	//		typedef std::list<Bar *>			BarList;
	//		typedef IterStackList<BarList>	BarIterStack;
	//
	//		BarList			List;
	//		BarIterStack	IterStack;
	//		
	//
	//		public:
	//			void Add		(Bar *b)			{ List.push_back(b); }
	//			void Remove	(Bar *b) 
	//			{
	//				BarList::iterator it = std::find(List.begin, List.end(), b);
	//				IterStack.Advance(it);
	//				List.erase(it);
	//			}
	//
	//			void Process(void) 
	//			{
	//				BarList::iterator it = List.begin();
	//				while (it != List.end()) {
	//					Bar *b = *it;						// save pointer to element
	//					IterStack.Push(&(++it));		// add iterator to stack for tracking
	//					b->F();								// call function that could modify list
	//					if (!IterStack.Pop()) break;	// if iterator is no longer valid, exit traversal immediately..
	//				}
	//			}
	//	};

	// NOTES:
	
	//	Iterators should always point to next element when they are added to the stack.
	// This way, the stack can advance the iterator if the underlying element is removed.
	// Otherwise, the caller wouldn't know whether to advance the iterator after calling
	// Pop().

	// If the list is destroyed, be sure to Invalidate() all iterators. If you know the
	// list is empty, this can be accomplished by calling Invalidate() on the end iterator.
	// (All active iterators will have been advanced to the end of the list by calls to
	// Advance().)

	// If Pop() returns false, do NOT use the iterator or the list, as they are invalid.

	// Because iterators are modified in place on the stack, there is no need to actually
	// pop the value from the stack. If the iterator is valid, the variable you pushed
	// points to the next element.

	protected:
		// TYPES
		typedef std::vector<IterType *>				IterStackType;

		// DATA
		IterStackType								IterStack;

	public:
		// MUTATORS
		void Push		(IterType *i);						// push an iterator on the stack for tracking. *i must point to next element of traversal.
		bool Pop			(void);								// pop iterator from top of stack. returns false if iterator is not valid.

		void Advance	(const IterType &invalid);		// advance any iterators that match argument
		void Invalidate(const IterType &invalid);		// invalidate any iterators that match argument
		void Invalidate(void);								// invalidate all iterators
};


////////////////////////////////////////////////////////////////////////////////
// 
// Implementation
// 
////////////////////////////////////////////////////////////////////////////////

template<class ListType, class IterType>
void IterStackList<ListType, IterType>::Push(IterType *it)
{
	// add iterator to top of stack
	IterStack.push_back(it);
}


template<class ListType, class IterType>
bool IterStackList<ListType, IterType>::Pop(void)
{
	// INVARIANT: iterator stack is not empty
	if (IterStack.empty()) { return false; }

	// pop last entry from iterator stack.
	IterType *back = IterStack.back();
	IterStack.pop_back();

	// iterator is safe if pointer is not zero.
	return (back != 0);
}


template<class ListType, class IterType>
void IterStackList<ListType, IterType>::Advance(const IterType &invalid)
{
	// for each iterator in stack
	for (typename IterStackType::iterator it = IterStack.begin(); it != IterStack.end(); ++it) 
	{
		// *it  = (ListType::iterator *) = pointer to an iterator
		// **it = (ListType::iterator &) = reference to an iterator

		// because we are comparing iterators, and not their contents,
		// we do not need to check against list.end().

		// if iterator is valid and matches invalid, advance it.
		if (*it && (**it == invalid))
			++(**it);
	}
}


template<class ListType, class IterType>
void IterStackList<ListType, IterType>::Invalidate(const IterType &invalid)
{
	// for each iterator in stack
	for (typename IterStackType::iterator it = IterStack.begin(); it != IterStack.end(); ++it) 
	{
		// *it  = (ListType::iterator *) = pointer to an iterator
		// **it = (ListType::iterator &) = reference to an iterator

		// because we are comparing iterators, and not their contents,
		// we do not need to check against list.end().

		// if iterator is valid and matches invalid, invalidate it.
		if (*it && (**it == invalid))
			*it = 0;
	}
}


template<class ListType, class IterType>
void IterStackList<ListType, IterType>::Invalidate(void)
{
	// for each iterator in stack
	for (typename IterStackType::iterator it = IterStack.begin(); it != IterStack.end(); ++it) 
	{
		// *it  = (ListType::iterator *) = pointer to an iterator
		// **it = (ListType::iterator &) = reference to an iterator

		// because we are comparing iterators, and not their contents,
		// we do not need to check against list.end().

		// if iterator is valid, invalidate it.
		if (*it)
			*it = 0;
	}
}

}

#endif // STL_ITER_STACK_LIST_H
