#pragma once

#include <utility>
#include "PriorityQueue.h"

namespace uniza_fri {

	/**
		< Complexities >

		insert		-> O(1)
		findMin		-> O(1)
		decreaseKey	-> O(1)
		meld		-> O(1)
		deleteMin	-> O(lg n)
	*/

	template<typename N, typename E>
	class StrictFibonacciHeap final : public PriorityQueue<N, E>
	{
		/// Forward declarations of nested classes
		class StrictFibNode;
		class StrictFibEntry;
		class ActiveRecord;
		class ListRecord;
		class FixListRecord;
		class RankListRecord;

		/// Private fields of the heap / heap record
		size_t dataSize;
		StrictFibNode  * root;
		ActiveRecord   * activeRecord;
		StrictFibNode  * nonLinkable;
		StrictFibNode  * queueHead;
		RankListRecord * rankList;
		FixListRecord  * fixListActRoots;
		FixListRecord  * fixListLoss;

	public:

		// These two would be complicated to implement
		StrictFibonacciHeap(const StrictFibonacciHeap & other) = delete;
		StrictFibonacciHeap & operator=(const StrictFibonacciHeap & other) = delete;

		/// Constructor, destructor
		StrictFibonacciHeap();
		virtual ~StrictFibonacciHeap();
		StrictFibonacciHeap(StrictFibonacciHeap<N, E> && other) noexcept;
		StrictFibonacciHeap & operator=(StrictFibonacciHeap && other) noexcept;

		/// Implementation of the PriorityQueue interface
		QueueEntry<N, E>* insert(const E & data, N prio)      override;
		void decreaseKey(QueueEntry<N, E>& entry, N newPrio)  override;
		PriorityQueue<N, E>* meld(PriorityQueue<N, E>* other) override;
		E deleteMin()                                         override;
		E & findMin()                                         override;
		size_t size()                                         override;
		void clear()                                          override;

	private:

		StrictFibonacciHeap(StrictFibNode * pRoot, size_t pSize, ActiveRecord * pActRecord, FixListRecord * pFlActRoot, FixListRecord * pFlLoss, RankListRecord * pRankList, StrictFibNode * pNonLink, StrictFibNode * pQueueHead);

		/// Heap operations
		void prependQueue(StrictFibNode * node);
		void removeFromQueue(StrictFibNode * node);
		void removeRootChild(StrictFibNode * node);
		void addRootChild(StrictFibNode * node);

		/// DeleteMin utils
		void rootify(StrictFibNode * min);
		void makePassive(StrictFibNode * min);
		void moveOldChildrenTo(StrictFibNode * min);
		void moveActiveChildren(StrictFibNode * min);
		void setNewNonlinkable(StrictFibNode * min);
		StrictFibNode * findNewRoot();

		/// Meld utils
		StrictFibonacciHeap<N, E> * meldPrivate(StrictFibonacciHeap<N, E> * x, StrictFibonacciHeap<N, E> * y);
		StrictFibNode * concatQueues(StrictFibonacciHeap<N, E> * x, StrictFibonacciHeap<N, E> * y, StrictFibNode * glue);

		/// Clear utils
		void clearFixLists();
		void clearRankList();

		/// Transformations
		bool doActiveRootReduce();
		bool doRootDegreeReduce();
		bool doLossReduce();
		void activeRootReduce(StrictFibNode * x, StrictFibNode * y);
		void rootDegreeReduce(StrictFibNode * x, StrictFibNode * y, StrictFibNode * z);
		void oneNodeLossReduce(StrictFibNode * x);
		void twoNodeLossReduce(StrictFibNode * x, StrictFibNode * y);

		/// FixLists
		void rmFlChecked(FixListRecord * record, FixListRecord ** fixListPtr, FixListRecord ** rlflptr);
		void appendFixList(FixListRecord * record, FixListRecord ** fixListPtr);
		void prependFixList(FixListRecord * record, FixListRecord ** fixListPtr);
		void addToFixList(StrictFibNode * node, FixListRecord ** rankToFlPtr, FixListRecord ** fixListPtr);
		void addToFixList(FixListRecord * flr, FixListRecord ** rankToFlPtr, FixListRecord ** fixListPtr);
		void increaseLoss(StrictFibNode * node);
		
		static void rmFlUnchcked(FixListRecord * record, FixListRecord ** fixListPtr);
		static bool isSingle(FixListRecord * record);
		static void addAfterFlRecord(FixListRecord * after, FixListRecord * record);

		/// RankList
		void justIncRank(StrictFibNode * node);
		void decreaseRank(StrictFibNode * node);
		static void justDecRank(StrictFibNode * node);

		/// Utils
		void sort(StrictFibNode ** x, StrictFibNode ** y, StrictFibNode ** z);
		void sort(StrictFibNode ** x, StrictFibNode ** y);

		/// Declarations of nested classes
		class StrictFibNode
		{
		public:

			StrictFibEntry * entry;

			StrictFibNode * parent;
			StrictFibNode * left;
			StrictFibNode * right;
			StrictFibNode * child;

			StrictFibNode * qprev;
			StrictFibNode * qnext;

			ActiveRecord * active;

			RankListRecord * rll;
			FixListRecord * flr;

			int loss;

		public:

			StrictFibNode(StrictFibNode & other)  = delete;
			StrictFibNode(StrictFibNode && other) = delete;
			StrictFibNode & operator=(StrictFibNode & other)  = delete;
			StrictFibNode & operator=(StrictFibNode && other) = delete;

			StrictFibNode(const E & data, N prio);
			~StrictFibNode();
			void swapEntries(StrictFibNode * other);
			void addActiveChild(StrictFibNode * newChild);
			void addPassiveChild(StrictFibNode * newChild);
			void removeChild(StrictFibNode * oldChild);
			void addAfter(StrictFibNode * sibling);

			void makeActive(ActiveRecord * record, RankListRecord * rlptr);
			void makePassive();

			bool isRoot();
			bool isSonOfRoot();
			bool isViolating();
			bool isActive();
			bool isPassive();
			bool isActiveRoot();
			bool isPassiveLinkable();
			bool operator<(StrictFibNode & other);

			StrictFibNode * disconnectChildren();
			StrictFibNode * disconnectPassiveChild();

			size_t subtreeSize1();
			size_t subtreeSize2();

		private:

			void addChld(StrictFibNode * newChild);

		};

		class StrictFibEntry final : public QueueEntry<N, E>
		{
		public:

			StrictFibNode * node;

		public:

			StrictFibEntry(StrictFibEntry & other)  = delete;
			StrictFibEntry(StrictFibEntry && other) = delete;
			StrictFibEntry & operator=(StrictFibEntry & other)  = delete;
			StrictFibEntry & operator=(StrictFibEntry && other) = delete;

			StrictFibEntry(const E & data, N prio, StrictFibNode * pNode);
			virtual ~StrictFibEntry() = default;

			void setPrio(N newPrio);

		};

		class ActiveRecord
		{
		public:

			bool active;
			size_t refCount;

		public:

			ActiveRecord(ActiveRecord & other)  = delete;
			ActiveRecord(ActiveRecord && other) = delete;
			ActiveRecord & operator=(ActiveRecord & other)  = delete;
			ActiveRecord & operator=(ActiveRecord && other) = delete;

			explicit ActiveRecord(bool pactive);
			~ActiveRecord() = default;

			bool decRefCount();
			void incRefCount();

		};

		class FixListRecord
		{
		public:

			StrictFibNode  * node;
			FixListRecord  * left;
			FixListRecord  * right;
			RankListRecord * rank;

		public:

			FixListRecord(FixListRecord & other)  = delete;
			FixListRecord(FixListRecord && other) = delete;
			FixListRecord & operator=(FixListRecord & other)  = delete;
			FixListRecord & operator=(FixListRecord && other) = delete;

			FixListRecord(StrictFibNode * pnode, RankListRecord * prank);
			~FixListRecord();

			void addAfter(FixListRecord * other);
			void addBefor(FixListRecord * other);

		};

		class RankListRecord
		{
		public:

			RankListRecord * inc;
			RankListRecord * dec;
			FixListRecord * loss;
			FixListRecord * activeRoots;

		public:

			RankListRecord(RankListRecord & other)  = delete;
			RankListRecord(RankListRecord && other) = delete;
			RankListRecord & operator=(RankListRecord & other)  = delete;
			RankListRecord & operator=(RankListRecord && other) = delete;

			RankListRecord();
			~RankListRecord();

		};
	};

	/// Definitions of everything that was declared above

	//
	// StrictFibonacciHeap
	//
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibonacciHeap(StrictFibNode * pRoot, const size_t pSize, ActiveRecord * pActRecord, FixListRecord * pFlActRoot, FixListRecord * pFlLoss, RankListRecord * pRankList, StrictFibNode * pNonLink, StrictFibNode * pQueueHead) :
		dataSize(pSize),
		root(pRoot),
		activeRecord(pActRecord),
		nonLinkable(pNonLink),
		queueHead(pQueueHead),
		rankList(pRankList),
		fixListActRoots(pFlActRoot),
		fixListLoss(pFlLoss)
	{
	}
	
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibonacciHeap() :
		dataSize(0),
		root(nullptr),
		activeRecord(new ActiveRecord(true)),
		nonLinkable(nullptr),
		queueHead(nullptr),
		rankList(new RankListRecord()),
		fixListActRoots(nullptr),
		fixListLoss(nullptr)
	{
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibonacciHeap(StrictFibonacciHeap<N, E>&& other) noexcept :
		dataSize(other.dataSize),
		root(other.root),
		activeRecord(other.activeRecord),
		nonLinkable(other.nonLinkable),
		queueHead(other.queueHead),
		rankList(other.rankList),
		fixListActRoots(other.fixListActRoots),
		fixListLoss(other.fixListLoss)
	{
		other.dataSize = 0;
		other.root = nullptr;
		other.queueHead = nullptr;
		other.nonLinkable = nullptr;
		other.fixListActRoots = nullptr;
		other.fixListLoss = nullptr;
		other.rankList = new RankListRecord();
		other.activeRecord = new ActiveRecord(true);
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::~StrictFibonacciHeap()
	{
		this->StrictFibonacciHeap<N, E>::clear();

		delete this->rankList;
		this->rankList = nullptr;
		
		if (this->activeRecord->decRefCount())
		{
			delete this->activeRecord;
		}

		this->activeRecord = nullptr;
	}

	template <typename N, typename E>
	StrictFibonacciHeap<N, E>& StrictFibonacciHeap<N, E>::operator=(StrictFibonacciHeap&& other) noexcept
	{
		if (this != &other)
		{
			this->clear();
			delete this->rankList;

			this->dataSize = other.dataSize;
			this->root = other.root;
			this->activeRecord = other.activeRecord;
			this->nonLinkable = other.nonLinkable;
			this->queueHead = other.queueHead;
			this->rankList = other.rankList;
			this->fixListActRoots = other.fixListActRoots;
			this->fixListLoss = other.fixListLoss;

			other.dataSize = 0;
			other.root = nullptr;
			other.queueHead = nullptr;
			other.nonLinkable = nullptr;
			other.fixListActRoots = nullptr;
			other.fixListLoss = nullptr;
			other.rankList = new RankListRecord();
			other.activeRecord = new ActiveRecord(true);
		}

		return *this;
	}

	template<typename N, typename E>
	QueueEntry<N, E>* StrictFibonacciHeap<N, E>::insert(const E & data, N prio)
	{
		auto * node(new StrictFibNode(data, prio));
		QueueEntry<N, E> * retEntry(node->entry);

		if (this->isEmpty())
		{
			this->root = node;
			node->left = nullptr;
			node->right = nullptr;
		}
		else
		{
			if (*node < *(this->root))
			{
				node->addPassiveChild(this->root);
				this->nonLinkable = (this->root->isPassiveLinkable()) ? nullptr : this->root;
				this->root->qprev = this->root;
				this->root->qnext = this->root;
				this->prependQueue(this->root);
				this->root = node;
			}
			else
			{
				this->root->addPassiveChild(node);
				this->prependQueue(node);
			}
		}

		while (this->doActiveRootReduce()) {};
		while (this->doRootDegreeReduce()) {};

		++this->dataSize;

		return retEntry;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::decreaseKey(QueueEntry<N, E>& entry, N newPrio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, newPrio);

		auto& entrySf = dynamic_cast<StrictFibEntry&>(entry);
		entrySf.setPrio(newPrio);
		StrictFibNode * x = entrySf.node;

		if (x->isRoot()) return;

		if (*x < *(this->root))
		{
			this->root->swapEntries(x);
		}

		if (x->isViolating())
		{
			StrictFibNode * y = x->parent;

			if (x->isActive() && !x->isActiveRoot())
			{
				if (x->loss > 0)
				{
					FixListRecord * flr(x->flr);
					this->rmFlChecked(flr, &(this->fixListLoss), &(flr->rank->loss));
					delete flr;
					x->loss = 0;
				}
				y->removeChild(x);
				this->addRootChild(x);
				this->addToFixList(x, &(x->rll->activeRoots), &(this->fixListActRoots));
				this->decreaseRank(y);
			}
			else
			{
				y->removeChild(x);
				this->addRootChild(x);
			}

			if (y->isActive() && !y->isActiveRoot())
			{
				this->increaseLoss(y);
				this->doLossReduce();
			}

			auto counter1(0);
			auto counter2(0);
			while (++counter1 <= 6 && this->doActiveRootReduce()) {};
			while (++counter2 <= 4 && this->doRootDegreeReduce()) {};
		}
	}

	template<typename N, typename E>
	E StrictFibonacciHeap<N, E>::deleteMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		E retData(this->root->entry->getData());
		StrictFibNode * oldRoot(this->root);

		if (this->dataSize > 1)
		{
			StrictFibNode * x = this->findNewRoot();
			this->makePassive(x);
			this->removeRootChild(x);
			this->moveOldChildrenTo(x);
			this->removeFromQueue(x);

			for (size_t i = 0; i < 2; i++)
			{
				if (!this->queueHead) break;

				StrictFibNode * passive1 = this->queueHead->disconnectPassiveChild();
				StrictFibNode * passive2 = this->queueHead->disconnectPassiveChild();

				if (passive1)
				{
					this->addRootChild(passive1);
				}

				if (passive2)
				{
					this->addRootChild(passive2);
				}

				this->queueHead = this->queueHead->right;
				this->doLossReduce();
			}

			while (this->doActiveRootReduce()) {};
			while (this->doRootDegreeReduce()) {};

		}
		else
		{
			this->root = nullptr;
		}

		delete oldRoot;
		--this->dataSize;		

		return retData;
	}

	template<typename N, typename E>
	E & StrictFibonacciHeap<N, E>::findMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->root->entry->getData();
		}
	}

	template<typename N, typename E>
	size_t StrictFibonacciHeap<N, E>::size()
	{
		return this->dataSize;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::clear()
	{
		this->clearRankList();
		this->clearFixLists();

		if (this->root)
		{
			delete this->root;
			this->root = nullptr;
		}
		
		this->rankList = new RankListRecord();
		this->nonLinkable = nullptr;
		this->queueHead = nullptr;
		this->dataSize = 0;
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* StrictFibonacciHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		auto * otherFib = dynamic_cast<StrictFibonacciHeap<N, E>*>(other);

		if (this->isEmpty() && other->isEmpty())
		{
			return new StrictFibonacciHeap<N, E>();
		}
		else if (this->isEmpty())
		{
			return new StrictFibonacciHeap<N, E>(std::move(*otherFib));
		}
		else if (other->isEmpty())
		{
			return new StrictFibonacciHeap<N, E>(std::move(*this));
		}
		else
		{
			StrictFibonacciHeap<N, E> * x = this;
			StrictFibonacciHeap<N, E> * y = otherFib;

			if (x->size() > y->size())
			{
				std::swap(x, y);
			}

			return this->meldPrivate(x, y);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::prependQueue(StrictFibNode * node)
	{
		if (this->queueHead)
		{
			StrictFibNode * last(this->queueHead->qprev);
			node->qprev = last;
			node->qnext = this->queueHead;
			last->qnext = node;
			this->queueHead->qprev = node;
			this->queueHead = node;
		}
		else
		{
			this->queueHead = node;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::removeFromQueue(StrictFibNode * node)
	{
		if (node == node->qnext)
		{
			this->queueHead = nullptr;
		}
		else
		{
			if (this->queueHead == node)
			{
				this->queueHead = node->qnext;
			}

			node->qprev->qnext = node->qnext;
			node->qnext->qprev = node->qprev;
		}

		node->qprev = nullptr;
		node->qnext = nullptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::removeRootChild(StrictFibNode * node)
	{
		if (node == node->left)
		{
			this->nonLinkable = nullptr;
		} 
		else if (node == this->nonLinkable)
		{
			if (node->right->isPassive() && !node->right->isPassiveLinkable())
			{
				this->nonLinkable = node->right;
			}
			else if (node->left->isActive())
			{
				this->nonLinkable = node->left;
			}
			else
			{
				this->nonLinkable = nullptr;
			}
		}

		this->root->removeChild(node);
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::addRootChild(StrictFibNode * node)
	{
		if (node->isPassiveLinkable())
		{
			this->root->addPassiveChild(node);
			return;
		}

		if (this->nonLinkable)
		{
			if (this->nonLinkable->isActive())
			{
				this->nonLinkable->addAfter(node);
				this->nonLinkable = node;
			}
			else
			{
				if (node->isActive())
				{
					this->root->addActiveChild(node);
				}
				else
				{
					this->nonLinkable->addAfter(node);
				}
			}
		}
		else
		{
			this->root->addActiveChild(node);
			this->nonLinkable = node;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::rootify(StrictFibNode * min)
	{
		this->setNewNonlinkable(min);
		this->root = min;
		this->root->left = nullptr;
		this->root->right = nullptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::makePassive(StrictFibNode * min)
	{
		if (!min->isActive())
		{
			return;
		}

		this->rmFlChecked(min->flr, &(this->fixListActRoots), &(min->flr->rank->activeRoots));
		delete min->flr;

		min->makePassive();

		StrictFibNode * it = min->child;
		StrictFibNode * endIt = min->child;

		while (it && it->isActive())
		{
			if (it->loss > 0)
			{
				this->rmFlChecked(it->flr, &(this->fixListLoss), &(it->flr->rank->loss));
				delete it->flr;
				it->loss = 0;
			}

			this->addToFixList(it, &(it->rll->activeRoots), &(this->fixListActRoots));
			it = it->right;

			if (it == endIt) break;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::moveOldChildrenTo(StrictFibNode * min)
	{
		if (!this->root->child)
		{
			this->rootify(min);
		}
		else
		{
			StrictFibNode * it(this->root->child);
			do
			{
				it->parent = min;
				it = it->right;
			} while (it != this->root->child);

			this->moveActiveChildren(min);

			StrictFibNode * oldchild(this->root->child);
			this->root->child = nullptr;
			this->rootify(min);

			if (!oldchild)
			{
				return;
			}

			oldchild->left->right = nullptr;
			while (oldchild)
			{
				StrictFibNode * nextit(oldchild->right);
				this->addRootChild(oldchild);
				oldchild = nextit;
			}
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::moveActiveChildren(StrictFibNode * min)
	{
		StrictFibNode * first(this->root->child);

		if (!first->isActive())
		{
			return;
		}

		StrictFibNode * last(this->nonLinkable->isActive() ? this->nonLinkable : this->nonLinkable->left);

		if (!this->root->child->left->isActive())
		{
			StrictFibNode * rightmost(this->root->child->left);
			this->root->child = last->right;

			first->left = last;
			last->right = first;

			this->root->child->left = rightmost;
			rightmost->right = this->root->child;
		}
		else
		{
			this->root->child = nullptr;
		}

		if (!min->child)
		{
			min->child = first;
		}
		else
		{
			StrictFibNode * rightmost(min->child->left);

			min->child->left = last;
			last->right = min->child;

			rightmost->right = first;
			first->left = rightmost;
			min->child = first;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::setNewNonlinkable(StrictFibNode * min)
	{
		if (!min->child)
		{
			this->nonLinkable = nullptr;
		}
		else if (min->child->left->isActive())
		{
			this->nonLinkable = min->child->left;
		}
		else
		{
			this->nonLinkable = min->child->left;
			while (this->nonLinkable->left->isPassive())
			{
				if (this->nonLinkable->left == min->child->left) break;
				this->nonLinkable = this->nonLinkable->left;
			}

			if (this->nonLinkable->isPassiveLinkable())
			{
				this->nonLinkable = this->nonLinkable->left->isActive() ? this->nonLinkable->left : nullptr;
			}
		}
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::doActiveRootReduce()
	{
		if (!this->fixListActRoots)
		{
			return false;
		}

		FixListRecord * fx(this->fixListActRoots);
		FixListRecord * fy(fx->right);

		if (fx == fy || fx->rank != fy->rank)
		{
			return false;
		}

		StrictFibNode * x(fx->node);
		StrictFibNode * y(fy->node);

		this->rmFlUnchcked(fy, &(this->fixListActRoots));
		this->rmFlChecked(fx, &(this->fixListActRoots), &(fx->rank->activeRoots));

		delete fx;
		delete fy;

		this->sort(&x, &y);
		this->activeRootReduce(x, y);
		this->addToFixList(x, &(x->rll->activeRoots), &(this->fixListActRoots));

		return true;
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::doRootDegreeReduce()
	{
		if (!this->root->child)
		{
			return false;
		}

		StrictFibNode * x(this->root->child->left);
		StrictFibNode * y(x->left);
		StrictFibNode * z(y->left);

		if (!(x->isPassiveLinkable() && y->isPassiveLinkable() && z->isPassiveLinkable()))
		{
			return false;
		}

		if (x == y || y == z || x == z)
		{
			return false;
		}

		this->removeRootChild(x);
		this->removeRootChild(y);
		this->removeRootChild(z);

		this->sort(&x, &y, &z);
		this->rootDegreeReduce(x, y, z);

		return true;
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::doLossReduce()
	{
		if (!this->fixListLoss)
		{
			return false;
		}

		FixListRecord * fx(this->fixListLoss);
		StrictFibNode * x(fx->node);

		if (x->loss >= 2)
		{
			this->rmFlChecked(fx, &(this->fixListLoss), &(fx->rank->loss));
			delete fx;
			this->oneNodeLossReduce(x);
			return true;
		}

		FixListRecord * fy(fx->right);
		StrictFibNode * y(fy->node);

		if (fy == fx || fx->rank != fy->rank)
		{
			return false;
		}

		if (y->loss >= 2)
		{
			this->rmFlChecked(fy, &(this->fixListLoss), &(fy->rank->loss));
			delete fy;
			this->oneNodeLossReduce(y);
			return true;
		}

		this->rmFlUnchcked(fy, &(this->fixListLoss));
		this->rmFlChecked(fx, &(this->fixListLoss), &(fx->rank->loss));

		delete fx;
		delete fy;

		this->sort(&x, &y);
		this->twoNodeLossReduce(x, y);

		return true;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::activeRootReduce(StrictFibNode * x, StrictFibNode * y)
	{
		if (y->isSonOfRoot())
		{
			this->removeRootChild(y);
		}
		else
		{
			y->parent->removeChild(y);
		}

		x->addActiveChild(y);
		this->justIncRank(x);
		StrictFibNode * z(x->disconnectPassiveChild());
		if (z)
		{
			this->addRootChild(z);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::rootDegreeReduce(StrictFibNode * x, StrictFibNode * y, StrictFibNode * z)
	{
		x->makeActive(this->activeRecord, this->rankList);
		y->makeActive(this->activeRecord, this->rankList);

		this->justIncRank(x);
		this->addToFixList(x, &(x->rll->activeRoots), &(this->fixListActRoots));

		x->loss = 0;
		y->loss = 0;

		x->addActiveChild(y);
		y->addPassiveChild(z);
		this->addRootChild(x);
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::oneNodeLossReduce(StrictFibNode * x)
	{
		StrictFibNode * y(x->parent);
		y->removeChild(x);
		x->loss = 0;
		this->addRootChild(x);
		this->addToFixList(x, &(x->rll->activeRoots), &(this->fixListActRoots));

		this->decreaseRank(y);
		if (!y->isActiveRoot())
		{
			this->increaseLoss(y);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::twoNodeLossReduce(StrictFibNode * x, StrictFibNode * y)
	{
		x->loss = 0;
		y->loss = 0;

		StrictFibNode * z(y->parent);

		z->removeChild(y);
		x->addActiveChild(y);
		this->justIncRank(x);
		this->decreaseRank(z);

		if (!z->isActiveRoot())
		{
			this->increaseLoss(z);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::rmFlUnchcked(FixListRecord * record, FixListRecord ** fixListPtr)
	{
		record->node->rll = record->rank;

		if (record->left == record)
		{
			*fixListPtr = nullptr;
		}
		else if (*fixListPtr == record)
		{
			*fixListPtr = record->right;
		}

		record->left->right = record->right;
		record->right->left = record->left;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::rmFlChecked(FixListRecord * record, FixListRecord ** fixListPtr, FixListRecord ** rlflptr)
	{
		RankListRecord * rank = record->rank;

		if (*rlflptr == record)
		{
			if (record->left == record)
			{
				*rlflptr = nullptr;
			}
			else if (record->right->rank == rank)
			{
				*rlflptr = record->right;
			}
			else
			{
				*rlflptr = nullptr;
			}
		}

		this->rmFlUnchcked(record, fixListPtr);

		if (*rlflptr)
		{
			if (this->isSingle(*rlflptr))
			{
				this->rmFlUnchcked(*rlflptr, fixListPtr);
				this->appendFixList(*rlflptr, fixListPtr);
			}
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::appendFixList(FixListRecord * record, FixListRecord ** fixListPtr)
	{
		if (*fixListPtr)
		{
			this->addAfterFlRecord((*fixListPtr)->left, record);
		}
		else
		{
			*fixListPtr = record;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::prependFixList(FixListRecord * record, FixListRecord ** fixListPtr)
	{
		if (*fixListPtr)
		{
			this->addAfterFlRecord((*fixListPtr)->left, record);
		}

		*fixListPtr = record;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::addAfterFlRecord(FixListRecord * after, FixListRecord * record)
	{
		record->left = after;
		record->right = after->right;
		after->right->left = record;
		after->right = record;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::addToFixList(StrictFibNode * node, FixListRecord ** rankToFlPtr, FixListRecord ** fixListPtr)
	{
		auto * flr(new FixListRecord(node, node->rll));
		node->flr = flr;
		this->addToFixList(flr, rankToFlPtr, fixListPtr);
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::addToFixList(FixListRecord * flr, FixListRecord ** rankToFlPtr, FixListRecord ** fixListPtr)
	{
		if (!(*rankToFlPtr))
		{
			*rankToFlPtr = flr;
			if (flr->node->loss >= 2)
			{
				this->prependFixList(flr, fixListPtr);
			}
			else
			{
				this->appendFixList(flr, fixListPtr);
			}
		}
		else if (this->isSingle(*rankToFlPtr))
		{
			this->rmFlUnchcked(*rankToFlPtr, fixListPtr);
			this->prependFixList(flr, fixListPtr);
			this->prependFixList(*rankToFlPtr, fixListPtr);
		}
		else
		{
			this->addAfterFlRecord(*rankToFlPtr, flr);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::increaseLoss(StrictFibNode * node)
	{
		if (node->loss == 0)
		{
			this->addToFixList(node, &(node->rll->loss), &(this->fixListLoss));
			++node->loss;
		}
		else if (node->loss == 1 && this->isSingle(node->flr))
		{
			this->rmFlUnchcked(node->flr, &(this->fixListLoss));
			++node->loss;
			this->prependFixList(node->flr, &(this->fixListLoss));
		}
		else
		{
			++node->loss;
		}
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::isSingle(FixListRecord * record)
	{
		if (record == record->left || record->node->loss >= 2)
		{
			return false;
		}
		else
		{
			return record->rank != record->left->rank && record->rank != record->right->rank;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::justIncRank(StrictFibNode * node)
	{
		RankListRecord * rll(node->rll);
		if (!rll->inc)
		{
			rll->inc = new RankListRecord();
			rll->inc->dec = rll;
		}
		node->rll = rll->inc;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::justDecRank(StrictFibNode * node)
	{
		node->rll = node->rll->dec;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::decreaseRank(StrictFibNode * node)
	{
		if (node->isActiveRoot())
		{
			FixListRecord * flr(node->flr);
			RankListRecord * rank(flr->rank);
			this->rmFlChecked(flr, &(this->fixListActRoots), &(rank->activeRoots));
			delete flr;
			this->justDecRank(node);
			this->addToFixList(node, &(node->rll->activeRoots), &(this->fixListActRoots));
		}
		else if (node->loss > 0)
		{
			FixListRecord * flr(node->flr);
			RankListRecord * rank(flr->rank);
			this->rmFlChecked(flr, &(this->fixListLoss), &(rank->loss));
			delete flr;
			this->justDecRank(node);
			this->addToFixList(node, &(node->rll->loss), &(this->fixListLoss));
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::sort(StrictFibNode ** x, StrictFibNode ** y, StrictFibNode ** z)
	{
		if (**y < **x)
		{
			std::swap(*x, *y);
		}

		if (**z < **y)
		{
			std::swap(*y, *z);
		}

		if (**y < **x)
		{
			std::swap(*x, *y);
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::sort(StrictFibNode ** x, StrictFibNode ** y)
	{
		if (**y < **x)
		{
			std::swap(*x, *y);
		}
	}

	template<typename N, typename E>
	auto StrictFibonacciHeap<N, E>::findNewRoot() -> StrictFibNode *
	{
		StrictFibNode * newRoot(this->root->child);
		StrictFibNode * endit(newRoot);
		StrictFibNode * iterated = newRoot->right;

		while (iterated != endit)
		{
			if (*iterated < *newRoot)
			{
				newRoot = iterated;
			}

			iterated = iterated->right;
		}

		return newRoot;
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>* StrictFibonacciHeap<N, E>::meldPrivate(StrictFibonacciHeap<N, E>* x, StrictFibonacciHeap<N, E>* y)
	{
		x->activeRecord->active = false;
		x->nonLinkable = nullptr;

		StrictFibonacciHeap<N, E> * u = x;
		StrictFibonacciHeap<N, E> * v = y;

		// u musí ma väèšiu prioritu
		if (*(v->root) < *(u->root))
		{
			std::swap(u, v);
		}

		u->addRootChild(v->root);

		size_t newSize = x->size() + y->size();
		StrictFibNode * newQueueHead = this->concatQueues(x, y, v->root);
		auto* newHeap = new StrictFibonacciHeap<N, E>(
			u->root,
			newSize,
			y->activeRecord,
			y->fixListActRoots,
			y->fixListLoss,
			y->rankList,
			u->nonLinkable,
			newQueueHead
		);

		x->root = nullptr;
		y->root = nullptr;
		x->dataSize = 0;
		y->dataSize = 0;
		x->clear(); // zmaže pomocné zoznamy v x
		y->fixListActRoots = nullptr;
		y->fixListLoss = nullptr;
		y->rankList = nullptr;

		while (newHeap->doActiveRootReduce()) {};
		while (newHeap->doRootDegreeReduce()) {};

		return newHeap;
	}

	template<typename N, typename E>
	auto StrictFibonacciHeap<N, E>::concatQueues(StrictFibonacciHeap<N, E>* x, StrictFibonacciHeap<N, E>* y, StrictFibNode * glue) -> StrictFibNode *
	{
		if (!x->queueHead && !y->queueHead)
		{
			glue->qprev = glue;
			glue->qnext = glue;
			return glue;
		}
		else if (!x->queueHead)
		{
			y->prependQueue(glue);
			return glue;
		}
		else if (!y->queueHead)
		{
			x->prependQueue(glue);
			return x->queueHead->qnext;
		}
		else
		{
			y->prependQueue(glue);
			
			StrictFibNode * lastX = x->queueHead->qprev;
			StrictFibNode * lastY = y->queueHead->qprev;

			lastX->qnext = y->queueHead;
			y->queueHead->qprev = lastX;

			lastY->qnext = x->queueHead;
			x->queueHead->qprev = lastY;

			return x->queueHead;
		}
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::clearFixLists()
	{
		if (this->fixListActRoots)
		{
			FixListRecord * it(this->fixListActRoots);
			it->left->right = nullptr;

			while (it)
			{
				FixListRecord * nextIt(it->right);
				delete it;
				it = nextIt;
			}
		}

		if (this->fixListLoss)
		{
			FixListRecord * it(this->fixListLoss);
			it->left->right = nullptr;

			while (it)
			{
				FixListRecord * nextIt(it->right);
				delete it;
				it = nextIt;
			}
		}

		this->fixListActRoots = nullptr;
		this->fixListLoss = nullptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::clearRankList()
	{
		RankListRecord * r = this->rankList;
		while (r)
		{
			RankListRecord * nr = r->inc;
			delete r;
			r = nr;
		}

		this->rankList = nullptr;
	}

	//
	// StrictFibNode
	//
	template<typename N, typename E>
	size_t StrictFibonacciHeap<N, E>::StrictFibNode::subtreeSize1()
	{
		size_t sz(1);

		StrictFibNode * it = this->child;
		while (it)
		{
			sz += it->subtreeSize1();
			it = it->right;
			if (it == this->child) break;
		}

		return sz;
	}

	template<typename N, typename E>
	size_t StrictFibonacciHeap<N, E>::StrictFibNode::subtreeSize2()
	{
		size_t sz(1);

		StrictFibNode * it = this->child;
		while (it)
		{
			sz += it->subtreeSize2();
			it = it->left;
			if (it == this->child) break;
		}

		return sz;
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibNode::StrictFibNode(const E & data, N prio) :
		entry(new StrictFibEntry(data, prio, this)),
		parent(nullptr),
		left(this),
		right(this),
		child(nullptr),
		qprev(this),
		qnext(this),
		active(new ActiveRecord(false)),
		rll(nullptr),
		flr(nullptr),
		loss(-1)
	{
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibNode::~StrictFibNode()
	{
		if (this->child)
		{
			this->child->left->right = nullptr;
			StrictFibNode * it(this->child);

			while (it)
			{
				StrictFibNode * nextIt(it->right);
				delete it;
				it = nextIt;
			}
		}

		if (this->active->decRefCount())
		{
			delete this->active;
		}
		this->active = nullptr;

		delete this->entry;
		this->entry = nullptr;

		this->left = nullptr;
		this->right = nullptr;
		this->parent = nullptr;
		this->child = nullptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::swapEntries(StrictFibNode * other)
	{
		StrictFibEntry * tmpEntry = other->entry;
		other->entry = this->entry;
		this->entry = tmpEntry;

		other->entry->node = other;
		this->entry->node = this;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::addActiveChild(StrictFibNode * newChild)
	{
		this->addChld(newChild);
		this->child = newChild;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::addPassiveChild(StrictFibNode * newChild)
	{
		this->addChld(newChild);
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::removeChild(StrictFibNode * oldChild)
	{
		oldChild->parent = nullptr;

		if (oldChild->right == oldChild)
		{
			this->child = nullptr;
		}
		else if (this->child == oldChild)
		{
			this->child = this->child->right;
		}

		oldChild->left->right = oldChild->right;
		oldChild->right->left = oldChild->left;
		oldChild->left = oldChild;
		oldChild->right = oldChild;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::addAfter(StrictFibNode * sibling)
	{
		sibling->left = this;
		sibling->right = this->right;
		this->right->left = sibling;
		this->right = sibling;

		sibling->parent = this->parent;
	};

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::makeActive(ActiveRecord * record, RankListRecord * rlptr)
	{
		if (this->active->decRefCount())
		{
			delete this->active;
		}

		this->active = record;
		record->incRefCount();
		this->rll = rlptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::makePassive()
	{
		if (this->active->decRefCount())
		{
			delete this->active;
		}

		this->active = new ActiveRecord(false);
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isRoot()
	{
		return this->parent == nullptr;
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isSonOfRoot()
	{
		return this->parent && !this->parent->parent;
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isViolating()
	{
		return this->parent && this->parent->parent && *this < *(this->parent);
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isActive()
	{
		return this->active->active;
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isPassive()
	{
		return !this->isActive();
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isActiveRoot()
	{
		return this->parent && !this->parent->isActive() && this->isActive();
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::isPassiveLinkable()
	{
		return this->isPassive() && (this->child == nullptr || this->child->isPassive());
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::StrictFibNode::operator<(StrictFibNode & other)
	{
		if (this->entry->getPrio() == other.entry->getPrio())
		{
			return this < &other;
		}
		else
		{
			return *(this->entry) < *(other.entry);
		}
	}

	template<typename N, typename E>
	auto StrictFibonacciHeap<N, E>::StrictFibNode::disconnectChildren() -> StrictFibNode *
	{
		StrictFibNode * ret = this->child;
		this->child = nullptr;
		return ret;
	}

	template<typename N, typename E>
	auto StrictFibonacciHeap<N, E>::StrictFibNode::disconnectPassiveChild() -> StrictFibNode *
	{
		if (!this->child)
		{
			return nullptr;
		}

		StrictFibNode * rightmost = this->child->left;
		if (rightmost->isPassive())
		{
			this->removeChild(rightmost);
			return rightmost;
		}
		else
		{
			return nullptr;
		}
	};
		
	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibNode::addChld(StrictFibNode * newChild)
	{
		if (!this->child)
		{
			this->child = newChild;
			newChild->left = newChild;
			newChild->right = newChild;
		}
		else
		{
			StrictFibNode * rightmost = this->child->left;
			newChild->left = rightmost;
			newChild->right = this->child;
			rightmost->right = newChild;
			this->child->left = newChild;
		}

		newChild->parent = this;
	}

	//
	// StrictFibEntry
	//
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::StrictFibEntry::StrictFibEntry(const E & data, N prio, StrictFibNode * pNode) :
		QueueEntry<N, E>(data, prio),
		node(pNode)
	{
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::StrictFibEntry::setPrio(N newPrio)
	{
		QueueEntry<N, E>::setPrioInternal(newPrio);
	}

	//
	// ActiveRecord
	//
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::ActiveRecord::ActiveRecord(bool pactive) :
		active(pactive),
		refCount(1)
	{
	}

	template<typename N, typename E>
	bool StrictFibonacciHeap<N, E>::ActiveRecord::decRefCount()
	{
		return --this->refCount <= 0;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::ActiveRecord::incRefCount()
	{
		++this->refCount;
	}

	//
	// FixListRecord
	//
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::FixListRecord::FixListRecord(StrictFibNode * pnode, RankListRecord * prank) :
		node(pnode),
		left(this),
		right(this),
		rank(prank)
	{
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::FixListRecord::~FixListRecord()
	{
		this->left = nullptr;
		this->right = nullptr;
		this->node = nullptr;
		this->rank = nullptr;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::FixListRecord::addAfter(FixListRecord * other)
	{
		other->left = this;
		other->right = this->right;
		this->right->left = other;
		this->right = other;
	}

	template<typename N, typename E>
	void StrictFibonacciHeap<N, E>::FixListRecord::addBefor(FixListRecord * other)
	{
		other->right = this;
		other->left = this->left;
		this->left->right = other;
		this->left = other;
	}

	//
	// RankListRecord
	//
	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::RankListRecord::RankListRecord() :
		inc(nullptr),
		dec(nullptr),
		loss(nullptr),
		activeRoots(nullptr)
	{
	}

	template<typename N, typename E>
	StrictFibonacciHeap<N, E>::RankListRecord::~RankListRecord()
	{
		this->activeRoots = nullptr;
		this->dec = nullptr;
		this->inc = nullptr;
	}

}