//	============================================================================
//	CSequence.C														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Please see CSequence.H for a description of the CSequence class.
//	========================================|===================================

#include	"CSequence.H"

template<class T>
vrsn CSequence<T>::version = "94.05.03";

//	============================================================================
//	Prepend
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Prepends an element of type T to a list.
//	========================================|===================================
template<class T>
bool
CSequence<T>::Prepend(T item)
	{
	SeqNode<T>*	sn = new SeqNode<T>;
	if (NULL == sn)
		return TRUE;						// couldn't allocate new SeqNode
	sn->next = first;						// new node points to old first
	if (NULL != first)						// old first node points
		first->prev = sn;					//   to new node
	first = sn;								// update first node pointer
	if (NULL == last)						// item added to empty list
		last = sn;
	sn->data = item;						// set the data
	size++;									// increment our size counter
	return FALSE;							// return no error
	}

//	============================================================================
//	Append
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Appends an element of type T to a list.
//	========================================|===================================
template<class T>
bool
CSequence<T>::Append(T item)
	{
	SeqNode<T>*	sn = new SeqNode<T>;
	if (NULL == sn)
		return TRUE;						// couldn't allocate new SeqNode
	sn->prev = last;						// new node points to old last
	if (NULL != last)						// old last node points
		last->next = sn;					//   to new node
	if (NULL == first)						// item added to empty list
		first = sn;
	last = sn;								// update last node pointer
	sn->data = item;						// set the data
	size++;									// increment our size counter
	return FALSE;							// return no error
	}

//	============================================================================
//	InsertNode
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Inserts an element of type T before an existing SeqNode.
//	========================================|===================================
template<class T>
bool
CSequence<T>::InsertNode(T item, SeqNode<T>* before)
	{
	if ((NULL == before) ||					// before not specified, or
	    (before = first) )					// insert @ first is the same
		return Prepend(item);				//   thing as a prepend
	SeqNode<T>*	sn = new SeqNode<T>;
	if (NULL == sn)
		return TRUE;						// couldn't allocate new SeqNode
	sn->prev = before->prev;				// new node -> its predecessor
	if (NULL != sn->prev)
		sn->prev->next = sn;				// predecessor -> new node
	sn->next = before;						// new node -> its successor
	before->prev = sn;						// successor -> new node
	sn->data = item;						// set the data
	size++;									// increment our size counter
	return FALSE;							// return no error
	}

//	============================================================================
//	RemoveNode
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Excises a specified SeqNode.
//	========================================|===================================
template<class T>
bool
CSequence<T>::RemoveNode(SeqNode<T>* node)
	{
	if (NULL == node)
		return TRUE;

	if (first == node)						// update first pointer if nec.
		first = node->next;
	if (last == node)						// ditto for last
		last = node->prev;

	if (NULL != node->prev)					// predecessor -> successor
		node->prev->next = node->next;
	if (NULL != node->next)					// successor -> predecessor
		node->next->prev = node->prev;

	size--;

	delete node;							// delete the node
	return FALSE;							// return no error
	}


