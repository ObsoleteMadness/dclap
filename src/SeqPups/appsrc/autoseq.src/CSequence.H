//	============================================================================
//	CSequence.H														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	CSequence is a template which uses a doubly-linked list to represent a lists
//	of any type of class.  This class grew from the need for a simple list
//	structure, and as such, is not a particularly robust implementation.  It
//	currently provides no iterator functions, serial access is used for indexed
//	references (ie. linear search).
//	Note that this class stores the list object type explicitly, rather than a
//	pointer to the object.  This has advantages and disadvantages, but suffice
//	it to say that the user assumes responsibility for delete'ing any space s/he
//	allocates and references in the list object, although the list object
//	itself will be deleted correctly.
//	Users may wish to override the assingment and comparison operators for
//	specialized types.
//
//	SLATED IMPROVEMENTS
//	*	For now, there is no error handling other than returning TRUE in the
//		case of an error.  I will extend to exceptions or error signals when the
//		code becomes more stable (ie. when I'm finished designing).
//	*	add support (via fx pointer in list header) to a binary rel'n operator
//	*	filter method
//
//	MODIFICATION HISTORY
//	93.07.31	Reece	Original coding
//	93.11.11	Reece	First release
//	========================================|===================================

#ifndef _H_CSequence						// include this file only once
#define _H_CSequence

#include	<iostream.h>
#include	<fstream.h>
#include	"RInclude.H"

template<class T>
struct SeqNode								// sequence node
	{
	SeqNode<T>*	next;
	SeqNode<T>*	prev;
	T			data;

				SeqNode(void):				// constructor; no arguments
					next(NULL),
					prev(NULL)
					{}

				SeqNode(					// constructor with args:
					T d,					//   data
					SeqNode<T>* p=NULL,		//   predecessor
					SeqNode<T>* n=NULL):	//   successor
					data(d),
					next(n),
					prev(p)
					{
					if (NULL != n) n->prev = this;			// insert the node
					if (NULL != p) p->next = this;
					}

				~SeqNode(void)				// destructor
					{
					if (NULL != next) next->prev = prev;	// excise the node
					if (NULL != prev) prev->next = next;
					}
	};

template<class T>
class	CSequence
	{
	private:
	static vrsn	version;

	protected:
	ulong		size;
	SeqNode<T>*	first;
	SeqNode<T>*	last;

	public:
	vrsn&		Version()
					{ return version; }

				CSequence(void):			// constructor
					first(0),
					last(0),
					size(0)
					{}

				~CSequence(void)			// destructor
					{
					for(SeqNode<T>* sn=first;NULL != sn;sn=sn->next)
						delete sn;
					}

	ulong		Size(void)					// return # elements in list
					{
					return size;
					}

	bool		Prepend(T item);
	bool		Append(T item);
	bool		Insert(						// insert item @ before
					T item,
					ulong before)
					{
					return InsertNode(item,(*this)%(before));
					}

	bool		Remove(ulong index)			// excise item @ index
					{
					return RemoveNode((*this)%(index));
					}

	ulong		operator()(T item)			// returns index of 1st occurrence
					{
					SeqNode<T>*	sn = first;	
					ulong		index = 1;
					while (NULL != sn)
					if (sn->data == item)
						return index;
					else
						{ sn = sn->next; index++; }
					return 0;
					}	

	T&			operator[](ulong index)		// returns item at 0<index<=Size()-1
					{
					SeqNode<T>* sn = (*this)%(index);
					return sn->data;
					}

	friend
	ostream&	operator<<(					// put endl delimt'd list on stream
					ostream&	os,
					CSequence<T>& seq)
					{
					for(SeqNode<T>* sn=seq.first;NULL!=sn;sn=sn->next)
						os << sn->data << endl;
					return os;
					}

	private:
	SeqNode<T>* operator%(ulong index)		// ret'n SeqNode* @ 0<=index<=size-1
					{
					SeqNode<T>*	sn = first;
					if (index > size-1)		// index must be >=0 (ulong)
						return NULL;
					while (index != 0)
						{ sn = sn->next; index--; }
					return sn;
					}

	bool	InsertNode(T item,SeqNode<T>* before);
	bool	RemoveNode(SeqNode<T>* node);
	};

#endif										// conditional inclusion

