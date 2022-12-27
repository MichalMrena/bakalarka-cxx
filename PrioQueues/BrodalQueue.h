#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>

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
	class BrodalQueue : public PriorityQueue<N, E>
	{
	private:
		// Forward declarations of nested classes
		class BrodalEntry;
		class BrodalNode;
		class RootWrap;
		class RootWrapT1;
		class RootWrapT2;
		class BrodalWrapList;

	private:
		// Private fields of the queue
		RootWrapT1 * t1Wrap;
		RootWrapT2 * t2Wrap;
		size_t dataSize;

	public:

		BrodalQueue();
		virtual ~BrodalQueue();

		// Implementation of the PriorityQueue interface
		QueueEntry<N, E> * insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E> & entry, N newPrio)   override;
		PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) override;
		E deleteMin()                                           override;
		E & findMin()                                           override;
		size_t size()                                           override;
		void clear()                                            override;

	private:

		// Private constructor used during operation meld
		BrodalQueue(size_t pSize, RootWrapT1 * pT1, RootWrapT2 * pT2);

		// Meld utils
		RootWrapT1 * findNewT1(BrodalQueue<N, E> * other);
		RootWrap * findMaxRank(BrodalQueue<N, E> * other);
		void meldUnder(RootWrap * underRoot, BrodalQueue<N, E> * q1, BrodalQueue<N, E> * q2);
		void addUnderRoot(RootWrap * root, RootWrap * toAdd);
		void setRootReferences();
		void dumbMeld(BrodalQueue<N, E> * q1, BrodalQueue<N, E> * q2);
		void clearAfterMeld(RootWrapT1 * newT1, RootWrapT2 * newT2);

		// DeleteMin utils
		void moveT2UnderT1();
		void makeSonOfRoot(BrodalNode * newMin);
		BrodalNode * findNewMin();

		// Heap utils
		void increaseRankOfT1();

	private:

		/*
			Subclasses of this class do different reduce operations on the queue.
			Instance of one of these subclasses is passed to Guide constructor.
		*/
		class Reducer {
		private:

			RootWrap * root;

		public:

			Reducer(RootWrap * root);
			virtual ~Reducer();

			virtual void reduce(int rank) = 0;
			virtual int getValue(int index) = 0;

		protected:

			RootWrap * getRoot();

		};

		class ReducerUpper : public Reducer {
		public:

			ReducerUpper(RootWrap * root);
			virtual ~ReducerUpper();
			virtual void reduce(int rank);
			virtual int getValue(int index);

		};

		class ReducerLower : public Reducer {
		public:

			ReducerLower(RootWrap * root);
			virtual ~ReducerLower();
			virtual void reduce(int rank);
			virtual int getValue(int index);

		};

		class ReducerViolation : public Reducer {
		private:

			RootWrapT1 * root;

		public:

			ReducerViolation(RootWrapT1 * root);
			virtual ~ReducerViolation();
			virtual void reduce(int rank);
			virtual int getValue(int index);

		};

		/*
			This class is used inside the Guide in order to keep numbers organized in blocks.
		 */
		class FirstInBlock {
		private:

			size_t refCount;
			int index;

		public:

			FirstInBlock();
			~FirstInBlock();

			size_t getRefCount();
			void incRefCount();
			void decRefCount();
			bool isNull();
			void setNull();
			int getIndex();
			int setIndex(int i);

		};

		/*
			Maintains abstract sequence of numbers. These numbers are bound to number of children of the root node
			or to number of violating nodes. Guide performs reduce operations over this sequence and by using 
			reducer, these operations are also performed on a root node to which this Guide belongs to.
		 */
		class Guide {
		private:

			int domainSize;
			std::vector<int> numbers;
			std::vector<FirstInBlock *> blockPointers;
			Reducer * reducer;

		public:

			explicit Guide(Reducer * reducer);
			~Guide();

			void increaseDomain();
			void decreaseDomain();
			void setReducer(Reducer * newReducer);
			void possiblyIncrease(int index, int val);

		private:

			void incInBlock(int i);
			void incOutOfBlock(int i);
			void reduce(int i);
			void decRefCount(int i);
			bool isInBlock(int index);

		};

		/*
			This class wraps data and priority of the items that are inserted into the queue.
			There is one instance of this class stored in each node. Both Node and Entry keeps
			pointers to each other so that entries can be swapped between two node easily.
		 */
		class BrodalEntry : public QueueEntry<N, E> {
		private:

			BrodalNode * item;

		public:

			BrodalEntry(const E & data, N prio, BrodalNode * pNode);
			virtual ~BrodalEntry();
			BrodalNode * getItem();
			void setItem(BrodalNode * pItem);
			void setPrio(N newPrio);

		};

		/*
			Nodes are used to build and maintain the trees. This class is responsible for
			all the operations that involves manipulating the tree structure.
		 */
		class BrodalNode {
		public:

			int rank;
			BrodalEntry * entry;

			BrodalNode * parent;
			BrodalNode * prev;
			BrodalNode * next;
			BrodalNode * child;

			BrodalNode * setV;
			BrodalNode * setW;
			std::vector<BrodalNode*> auxW; 
			bool inVSet;				
										
			BrodalNode * prevInSet;
			BrodalNode * nextInSet;
			BrodalNode * inThisOnesSet;

		public:

			BrodalNode(const E & data, N prio);
			~BrodalNode();

			int getRank();
			int sameRankSiblingCount();

			BrodalNode * getParent();
			BrodalNode * getPrev();
			BrodalNode * getNext();
			BrodalNode * getChild();
			BrodalNode * getPrevInSet();
			BrodalNode * getNextInSet();
			BrodalNode * getSetW();
			BrodalNode * getSetV();
			BrodalNode * getWViolation(int rank);
			BrodalNode * getSameRankSibling();
			QueueEntry<N, E> * getEntry();

			bool isInThisSet(BrodalNode * setOwner);
			bool isSibling(BrodalNode * other);
			bool isSonOfRoot();
			bool isViolating();
			bool isInVSet();
			bool operator<(const BrodalNode & other);

			BrodalNode * disconectChildren();
			BrodalNode * disconnectSetW();
			BrodalNode * disconnectSetV();
			BrodalNode * remove2To3LargestChildren();
			BrodalNode * removeLargestSons();

			void addChildAtFront(BrodalNode * newChild);
			void addChildAfter(BrodalNode * after, BrodalNode * childToAdd);
			void addToVSet(BrodalNode * item);
			void addToWSet(BrodalNode * item);
			void removeNChildren(BrodalNode * first, int n);
			void removeFromViolationSet();
			void removeFromViolationSet(int trueRank);
			void replaceWith(BrodalNode * replacement);
			void setEntry(BrodalEntry * pEntry);
			void swapWith(BrodalNode * other);
			void swapEntries(BrodalNode * other);

		private:

			int lasrgestChildrenCount();
			void checkRank();
			void removeFromSet(BrodalNode * itemToRemove, int trueRank);

		};

		/*
			This class wraps the root node of the tree. With help of the Guide class it can perform
			different transformations on children of the root node in order to keep invariants satisfied.
		 */
		class RootWrap {
		private:

			BrodalNode * rootItem;
			Guide * guideUpper;
			Guide * guideLower;
			std::vector<int> childrenCount;
			std::vector<BrodalNode*> children;

		public:

			static const int T1;
			static const int T2;

		public:

			RootWrap(BrodalNode * item);
			RootWrap(RootWrap && other);
			virtual ~RootWrap();

			int getValUpperBound(int index);
			int getValLowerBound(int index);

			void reduceUpperBound(int rank);
			void reduceLowerBound(int rank);

			virtual void addChild(BrodalNode * chld, bool guideAdding);
			virtual void increaseRank(BrodalNode * first, BrodalNode * second);

			virtual void removeViolation(BrodalNode * item) = 0;
			virtual void addExtraItem(BrodalNode * item) = 0;
			virtual int getType() = 0;

			void addLinkedItems(BrodalNode * items, bool guideAdding);
			void removeChild(BrodalNode * child);

			BrodalNode * removeChild(int rank, bool guideRemoving);
			BrodalNode * getFirstZeroRankChild();
			BrodalNode * getItemOfRank(int rank);
			BrodalNode * getRootItem();

		protected:

			virtual void increaseDomains();
			virtual void decreaseDomains();
			void checkChildrenLower(int rank);
			void removeChild(BrodalNode * child, int trueRank);

			BrodalNode * removeChildUnchecked(int rank);

		private:

			int guideNumberUpper(int count);
			int guideNumberLower(int count);

			void checkLargeChildrenUpper(int rank);
			void checkLargeChildrenLower(int rank);

			BrodalNode * linkChildren(int rank);
			BrodalNode * maxPrio(BrodalNode * x1, BrodalNode * x2);

		};

		/*
			Specialization of the RootWrap class for the root of the first tree.
			This class keeps track of violating nodes and uses guide to maintain their count low enough.
		 */
		class RootWrapT1 : public RootWrap {
		private:

			RootWrapT2 * t2Wrap;
			Guide * guideViolation;
			std::vector<int> violationCount;

		public:

			RootWrapT1(BrodalNode * rootItem);
			virtual ~RootWrapT1();

			void addChild(BrodalNode * chld, bool guideAdding) override;
			void increaseRank(BrodalNode * first, BrodalNode * second) override;
			void removeViolation(BrodalNode * item) override;
			void addExtraItem(BrodalNode * item) override;

			int getType() override;
			int getValViolation(int index);

			void reduceViolation(int rank);
			void setOtherWrap(RootWrapT2 * wrap);
			void addPossibleViolation(BrodalNode * violItem, bool guideAdding);
			void totallyReduceWViolations();
			void mergeViolationSets(BrodalNode * newMin);
			void incRankAndAddRest(BrodalNode * linked);

		protected:

			void increaseDomains() override;
			void decreaseDomains() override;

		private:

			int t2ViolationsReduce(int rank);
			int getNumberViolation(int count);

			void normalViolationsReduce(int rank, int toRemoveCount);
			void getNormalViolations(int rank, int count, BrodalNode ** first, BrodalNode ** second);
			void normalViolReduceSingle(BrodalNode * x1);
			void cutWithSibling(BrodalNode * x1);
			void makeSiblings(BrodalNode * x1, BrodalNode * x2);
			void addViolationSmall(BrodalNode * violItem, bool guideAdding);
			void addViolationLarge(BrodalNode * violItem);
			void mergeViolationSetIntoW(BrodalNode * set);
			void removeViolation(BrodalNode * item, int trueRank);

		};

		/*
			Specialization of the RootWrap class for the root of the second tree.
			If there is violating node or some extra node created during stransformations on this root
			then these nodes are processed at the first tree. Thats why root wraps keep pointers to each other.
		 */
		class RootWrapT2 : public RootWrap {
		private:

			RootWrapT1 * t1Wrap;

		public:

			RootWrapT2(BrodalNode * rootItem);
			RootWrapT2(RootWrap && wrap);
			virtual ~RootWrapT2();

			void addChild(BrodalNode * child, bool guideAdding) override;
			void removeViolation(BrodalNode * item) override;
			void addExtraItem(BrodalNode * item) override;
			int getType() override;

			void setOtherWrap(RootWrapT1 * wrap);

		};
		
	};

	//
	// BrodalQueue
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalQueue() :
		BrodalQueue<N, E>(0, nullptr, nullptr)
	{
		
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalQueue(size_t pSize, RootWrapT1 * pT1, RootWrapT2 * pT2) :
		t1Wrap(pT1),
		t2Wrap(pT2),
		dataSize(pSize)
	{

	}

	template<typename N, typename E>
	BrodalQueue<N, E>::~BrodalQueue()
	{
		this->BrodalQueue<N, E>::clear();
	}

	template<typename N, typename E>
	QueueEntry<N, E>* BrodalQueue<N, E>::insert(const E & data, N prio)
	{
		auto* newItem = new BrodalNode(data, prio);
		QueueEntry<N, E> * retEntry = newItem->entry;

		++this->dataSize;

		if (this->dataSize == 1)
		{
			this->t1Wrap = new RootWrapT1(newItem);
		}
		else
		{
			BrodalNode * t1Root = this->t1Wrap->getRootItem();
			if (*newItem < *t1Root)
			{
				t1Root->swapEntries(newItem);				
			}

			if (this->dataSize == 2)
			{
				this->t2Wrap = new RootWrapT2(newItem);
			}
			else if (this->dataSize == 3)
			{
				BrodalNode * oldt2 = this->t2Wrap->getRootItem();
				delete this->t2Wrap;
				this->t2Wrap = nullptr;
				this->t1Wrap->setOtherWrap(nullptr);
				this->t1Wrap->increaseRank(oldt2, newItem);
			}
			else
			{
				this->t1Wrap->addChild(newItem, false);
			}
		}

		return retEntry;
	}

	template<typename N, typename E>
	E BrodalQueue<N, E>::deleteMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		E ret = this->findMin();
		--this->dataSize;

		BrodalNode * nodeToDelete;

		if (this->dataSize == 0)
		{
			nodeToDelete = this->t1Wrap->getRootItem();
			delete this->t1Wrap;
			this->t1Wrap = nullptr;
		}
		else if (this->dataSize == 1)
		{
			this->t1Wrap->getRootItem()->swapEntries(this->t2Wrap->getRootItem());
			nodeToDelete = this->t2Wrap->getRootItem();
			delete this->t2Wrap;
			this->t2Wrap = nullptr;
		}
		else if (this->dataSize == 2)
		{
			BrodalNode * child1 = this->t1Wrap->getRootItem()->getChild();
			BrodalNode * child2 = child1->getNext();
			this->t1Wrap->getRootItem()->disconectChildren();
			nodeToDelete = this->t1Wrap->getRootItem();
			delete this->t1Wrap;

			if (*child1 < *child2)
			{
				this->t1Wrap = new RootWrapT1(child1);
				this->t2Wrap = new RootWrapT2(child2);
			}
			else
			{
				this->t1Wrap = new RootWrapT1(child2);
				this->t2Wrap = new RootWrapT2(child1);
			}

			this->t1Wrap->setOtherWrap(this->t2Wrap);
			this->t2Wrap->setOtherWrap(this->t1Wrap);
		}
		else
		{
			this->moveT2UnderT1();

			BrodalNode * newMin = this->findNewMin();
			if (!newMin->isSonOfRoot())
			{
				this->makeSonOfRoot(newMin);
			}

			this->t1Wrap->removeChild(newMin);
			BrodalNode * newChildren = newMin->disconectChildren();
			this->t1Wrap->addLinkedItems(newChildren, false);
			this->t1Wrap->mergeViolationSets(newMin);
			this->t1Wrap->totallyReduceWViolations();
			this->t1Wrap->getRootItem()->swapEntries(newMin);
			nodeToDelete = newMin;
		}

		delete nodeToDelete;

		return ret;
	}

	template<typename N, typename E>
	E & BrodalQueue<N, E>::findMin()
	{
		this->increaseRankOfT1();

		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->t1Wrap->getRootItem()->getEntry()->getData();
		}
	}

	template<typename N, typename E>
	size_t BrodalQueue<N, E>::size()
	{
		return this->dataSize;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::decreaseKey(QueueEntry<N, E>& entry, N newPrio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, newPrio);

		this->increaseRankOfT1();

		BrodalEntry * brodalEntry = &dynamic_cast<BrodalEntry &>(entry);
		BrodalNode * item = brodalEntry->getItem();

		brodalEntry->setPrio(newPrio);

		BrodalNode * rootItem = this->t1Wrap->getRootItem();
		if (*item < *rootItem)
		{
			rootItem->swapEntries(item);
		}

		if (item->isViolating())
		{
			this->t1Wrap->addPossibleViolation(item, false);
		}
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* BrodalQueue<N, E>::meld(PriorityQueue<N, E>* otherQueue)
	{
		if (this->isEmpty() && otherQueue->isEmpty())
		{
			return new BrodalQueue<N, E>();
		}

		BrodalQueue<N, E> * other = dynamic_cast<BrodalQueue<N, E>*>(otherQueue);
		BrodalQueue<N, E> * newQueue;
		size_t newSize = this->dataSize + other->dataSize;

		RootWrapT2 * newT2 = nullptr;
		RootWrapT1 * newT1 = this->findNewT1(other);
		RootWrap * maxRankRoot = this->findMaxRank(other);

		if (maxRankRoot->getRootItem()->rank == 0) 
		{
			newQueue = new BrodalQueue<N, E>();
			newQueue->dumbMeld(this, other);
		}
		else 
		{
			if (maxRankRoot->getRootItem()->rank == newT1->getRootItem()->rank) 
			{
				maxRankRoot = newT1;
			}

			if (maxRankRoot == newT1) 
			{
				newQueue = new BrodalQueue<N, E>(newSize, newT1, nullptr);
				newQueue->meldUnder(newQueue->t1Wrap, this, other);
			}
			else 
			{
				newT2 = new RootWrapT2(std::move(*maxRankRoot));
				newQueue = new BrodalQueue<N, E>(newSize, newT1, newT2);
				newQueue->meldUnder(newQueue->t2Wrap, this, other);
			}
		}

		this->clearAfterMeld(newT1, newT2);
		other->clearAfterMeld(newT1, newT2);

		return newQueue;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::clear()
	{
		if (this->t1Wrap)
		{
			delete this->t1Wrap->getRootItem();
			delete this->t1Wrap;
			this->t1Wrap = nullptr;
		}

		if (this->t2Wrap)
		{
			delete this->t2Wrap->getRootItem();
			delete this->t2Wrap;
			this->t2Wrap = nullptr;
		}

		this->dataSize = 0;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::findNewT1(BrodalQueue<N, E>* other) -> RootWrapT1 *
	{
		if (!this->t1Wrap)
		{
			return other->t1Wrap;
		}
		else if (!other->t1Wrap)
		{
			return this->t1Wrap;
		}
		else
		{
			return (*(this->t1Wrap->getRootItem())) < (*(other->t1Wrap->getRootItem())) ? this->t1Wrap : other->t1Wrap;
		}
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::findMaxRank(BrodalQueue<N, E>* other) -> RootWrap *
	{
		RootWrap * max1;
		RootWrap * max2;

		if (!this->t2Wrap)
		{
			max1 = this->t1Wrap;
		}
		else
		{
			max1 = this->t2Wrap;
		}

		if (!other->t2Wrap)
		{
			max2 = other->t1Wrap;
		}
		else
		{
			max2 = other->t2Wrap;
		}

		if (!max1)
		{
			return max2;
		}
		else if (!max2)
		{
			return max1;
		}
		else
		{
			return max1->getRootItem()->rank > max2->getRootItem()->rank ? max1 : max2;
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::meldUnder(RootWrap * underRoot, BrodalQueue<N, E>* q1, BrodalQueue<N, E>* q2)
	{
		this->addUnderRoot(underRoot, q1->t1Wrap);
		this->addUnderRoot(underRoot, q1->t2Wrap);
		this->addUnderRoot(underRoot, q2->t1Wrap);
		this->addUnderRoot(underRoot, q2->t2Wrap);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::addUnderRoot(RootWrap * root, RootWrap * toAdd)
	{
		if (toAdd == this->t1Wrap || toAdd == this->t2Wrap || !toAdd || !toAdd->getRootItem()) 
		{
			return;
		}

		BrodalNode * nodeToAdd = toAdd->getRootItem();

		if (root->getRootItem()->rank > nodeToAdd->rank) 
		{
			root->addChild(nodeToAdd, false);
		}
		else 
		{
			BrodalNode * largeChildren = nodeToAdd->removeLargestSons();
			root->addLinkedItems(largeChildren, false);
			root->addChild(nodeToAdd, false);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::setRootReferences()
	{
		if (this->t1Wrap) 
		{
			this->t1Wrap->setOtherWrap(this->t2Wrap);
		}

		if (this->t2Wrap) 
		{
			this->t2Wrap->setOtherWrap(this->t1Wrap);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::dumbMeld(BrodalQueue<N, E>* q1, BrodalQueue<N, E>* q2)
	{
		while (!q1->isEmpty()) 
		{
			QueueEntry<N, E> * entry = q1->t1Wrap->getRootItem()->entry;
			this->insert(entry->getData(), entry->getPrio());
			q1->deleteMin();
		}

		while (!q2->isEmpty()) 
		{
			QueueEntry<N, E> * entry = q2->t1Wrap->getRootItem()->entry;
			this->insert(entry->getData(), entry->getPrio());
			q2->deleteMin();
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::clearAfterMeld(RootWrapT1 * newT1, RootWrapT2 * newT2)
	{
		if (this->t1Wrap != newT1)
		{
			delete this->t1Wrap;
		}
		this->t1Wrap = nullptr;

		if (this->t2Wrap != newT2)
		{
			delete this->t2Wrap;
		}
		this->t2Wrap = nullptr;

		this->dataSize = 0;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::moveT2UnderT1()
	{
		if (!this->t2Wrap)
		{
			return;
		}

		BrodalNode * t2root = this->t2Wrap->getRootItem();
		BrodalNode * t1root = this->t1Wrap->getRootItem();

		if (t2root->getRank() > 0)
		{

			BrodalNode * zeroRank = this->t2Wrap->getFirstZeroRankChild();
			t2root->disconectChildren();
			BrodalNode * item = zeroRank->getPrev();

			if (t1root->getRank() == 0)
			{
				BrodalNode * newZeroRank = zeroRank->getNext()->getNext();
				this->t1Wrap->increaseRank(zeroRank, zeroRank->getNext());
				zeroRank = newZeroRank;
			}

			while (zeroRank != nullptr)
			{
				BrodalNode * nextZeroRank = zeroRank->getNext();
				this->t1Wrap->addChild(zeroRank, false);
				zeroRank = nextZeroRank;
			}

			while (item != nullptr)
			{
				if (item->getRank() == t1root->getRank())
				{
					BrodalNode * newItem = item->getPrev()->getPrev();
					this->t1Wrap->increaseRank(item, item->getPrev());
					item = newItem;
				}

				if (item == nullptr)
				{
					break;
				}

				if (item->getRank() == t1root->getRank())
				{
					continue;
				}

				BrodalNode * nextItem = item->getPrev();
				this->t1Wrap->addChild(item, false);
				item = nextItem;
			}

		}

		this->t1Wrap->addChild(t2root, false);
		this->t1Wrap->setOtherWrap(nullptr);
		delete this->t2Wrap;
		this->t2Wrap = nullptr;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::makeSonOfRoot(BrodalNode * newMin)
	{
		BrodalNode * swaped = this->t1Wrap->getItemOfRank(newMin->getRank());

		swaped->swapWith(newMin);
		this->t1Wrap->removeViolation(newMin);
		this->t1Wrap->addPossibleViolation(swaped, false);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::increaseRankOfT1()
	{
		if (!this->t2Wrap || this->dataSize < 4)
		{
			return;
		}

		int t1Rank = this->t1Wrap->getRootItem()->getRank();
		int t2Rank = this->t2Wrap->getRootItem()->getRank();

		if (t2Rank <= t1Rank + 2)
		{
			BrodalNode * t2Root = this->t2Wrap->getRootItem();
			delete this->t2Wrap;
			this->t2Wrap = nullptr;
			this->t1Wrap->setOtherWrap(nullptr);

			BrodalNode * delinked1 = t2Root->removeLargestSons();
			if (t2Root->getRank() > t1Rank)
			{
				BrodalNode * delinked2 = t2Root->removeLargestSons();
				this->t1Wrap->incRankAndAddRest(delinked2);
			}
			this->t1Wrap->incRankAndAddRest(delinked1);
			this->t1Wrap->addChild(t2Root, false);
		}
		else
		{
			BrodalNode * item = this->t2Wrap->removeChild(t1Rank + 1, false);
			BrodalNode * delinked = item->removeLargestSons();
			this->t1Wrap->incRankAndAddRest(delinked);
			this->t1Wrap->addChild(item, false);
		}
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::findNewMin() -> BrodalNode *
	{
		BrodalNode * t1Root = this->t1Wrap->getRootItem();
		BrodalNode * min = t1Root->getChild();

		BrodalNode * item = min->getNext();
		while (item)
		{
			if (*item < *min)
			{
				min = item;
			}
			item = item->getNext();
		}

		item = t1Root->getSetW();
		while (item)
		{
			if (*item < *min)
			{
				min = item;
			}
			item = item->getNextInSet();
		}

		item = t1Root->getSetV();
		while (item)
		{
			if (*item < *min)
			{
				min = item;
			}
			item = item->getNextInSet();
		}

		this->t1Wrap->removeViolation(min);

		return min;
	}
	
	//
	// BrodalEntry
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalEntry::BrodalEntry(const E & data, N prio, BrodalNode * pNode) :
		QueueEntry<N, E>(data, prio),
		item(pNode)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalEntry::~BrodalEntry()
	{
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalEntry::getItem() -> BrodalNode *
	{
		return this->item;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalEntry::setItem(BrodalNode * pItem)
	{
		this->item = pItem;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalEntry::setPrio(N newPrio)
	{
		QueueEntry<N, E>::setPrioInternal(newPrio);
	}

	//
	// BrodalNode
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalNode::BrodalNode(const E & data, N prio) :
		rank(0),
		entry(new BrodalEntry(data, prio, this)),
		parent(nullptr),
		prev(nullptr),
		next(nullptr),
		child(nullptr),
		setV(nullptr),
		setW(nullptr),
		inVSet(false),
		prevInSet(nullptr),
		nextInSet(nullptr),
		inThisOnesSet(nullptr)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::BrodalNode::~BrodalNode()
	{
		delete this->entry;

		BrodalNode * itm = this->child;
		while (itm)
		{
			BrodalNode * nxt = itm->next;
			delete itm;
			itm = nxt;
		}
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::BrodalNode::getRank()
	{
		return this->rank;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::BrodalNode::sameRankSiblingCount()
	{
		int count = 0;
		BrodalNode * itemPrev = this->prev;
		BrodalNode * itemNext = this->next;

		while (itemPrev != nullptr && itemPrev->rank == this->rank)
		{
			count++;
			itemPrev = itemPrev->prev;
		}

		while (itemNext != nullptr && itemNext->rank == this->rank)
		{
			count++;
			itemNext = itemNext->next;
		}

		return count;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getParent() -> BrodalNode *
	{
		return this->parent;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getPrev() -> BrodalNode *
	{
		return this->prev;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getNext() -> BrodalNode *
	{
		return this->next;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getChild() -> BrodalNode *
	{
		return this->child;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getPrevInSet() -> BrodalNode *
	{
		return this->prevInSet;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getNextInSet() -> BrodalNode *
	{
		return this->nextInSet;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getSetW() -> BrodalNode *
	{
		return this->setW;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getSetV() -> BrodalNode *
	{
		return this->setV;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getWViolation(int rank) -> BrodalNode *
	{
		return this->auxW[rank];
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::getSameRankSibling() -> BrodalNode *
	{
		return this->prev != nullptr && this->prev->rank == this->rank ? this->prev : this->next;
	}

	template<typename N, typename E>
	QueueEntry<N, E> * BrodalQueue<N, E>::BrodalNode::getEntry()
	{
		return this->entry;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::isInThisSet(BrodalNode * setOwner)
	{
		return this->inThisOnesSet == setOwner;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::isSibling(BrodalNode * other)
	{
		return this->parent == other->parent;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::isSonOfRoot()
	{
		return this->parent != nullptr && this->parent->parent == nullptr;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::isViolating()
	{
		return this->parent && *this < *this->parent;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::isInVSet()
	{
		return this->inVSet;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::BrodalNode::operator<(const BrodalNode & other)
	{
		return (*(this->entry)) < (*(other.entry));
	}
	
	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::disconectChildren() -> BrodalNode *
	{
		BrodalNode * ret = this->child;
		this->rank = 0;
		this->child = nullptr;

		return ret;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::disconnectSetW() -> BrodalNode *
	{
		BrodalNode * ret = this->setW;
		this->setW = nullptr;
		this->auxW.clear();
		this->auxW.reserve(this->rank);
		return ret;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::disconnectSetV() -> BrodalNode *
	{
		BrodalNode * ret = this->setV;
		this->setV = nullptr;
		return ret;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::remove2To3LargestChildren() -> BrodalNode *
	{
		BrodalNode * ret = this->child;

		int largeChildCount = this->lasrgestChildrenCount();
		if (largeChildCount == 2 || largeChildCount == 3) {
			this->removeNChildren(this->child, largeChildCount);
		}
		else {
			this->removeNChildren(this->child, 2);
		}

		return ret;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::BrodalNode::removeLargestSons() -> BrodalNode *
	{
		BrodalNode * ret = this->child;
		this->removeNChildren(this->child, this->lasrgestChildrenCount());
		return ret;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::swapWith(BrodalNode * other)
	{
		BrodalNode * oldThisPrev = this->prev;
		BrodalNode * oldThisNext = this->next;
		BrodalNode * oldThisParent = this->parent;

		if (other->prev != nullptr)
		{
			other->prev->next = this;
		}
		if (other->next != nullptr)
		{
			other->next->prev = this;
		}
		this->prev = other->prev;
		this->next = other->next;
		this->parent = other->parent;
		if (other->parent->child == other)
		{
			other->parent->child = this;
		}

		if (oldThisPrev != nullptr)
		{
			oldThisPrev->next = other;
		}
		if (oldThisNext != nullptr) {
			oldThisNext->prev = other;
		}
		other->prev = oldThisPrev;
		other->next = oldThisNext;
		other->parent = oldThisParent;
		if (oldThisParent->child == this) {
			oldThisParent->child = other;
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::swapEntries(BrodalNode * other)
	{
		BrodalEntry * tmpEntry = other->entry;
		other->entry = this->entry;
		this->entry = tmpEntry;

		other->entry->setItem(other);
		this->entry->setItem(this);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::addToVSet(BrodalNode * item)
	{
		if (this->setV == nullptr)
		{
			this->setV = item;
		}
		else
		{
			this->setV->prevInSet = item;
			item->nextInSet = this->setV;
			this->setV = item;
		}

		item->inThisOnesSet = this;
		item->inVSet = true;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::addToWSet(BrodalNode * item)
	{
		if (this->auxW[item->rank] == nullptr)
		{
			if (this->setW == nullptr)
			{
				this->setW = item;
			}
			else
			{
				this->setW->prevInSet = item;
				item->nextInSet = this->setW;
				this->setW = item;
			}

			this->auxW[item->rank] = item;
		}
		else {
			BrodalNode * afterThis = this->auxW[item->rank];
			item->prevInSet = afterThis;
			item->nextInSet = afterThis->nextInSet;

			if (afterThis->nextInSet != nullptr)
			{
				afterThis->nextInSet->prevInSet = item;
			}
			afterThis->nextInSet = item;
		}

		item->inThisOnesSet = this;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::setEntry(BrodalEntry * pEntry)
	{
		this->entry = pEntry;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::removeNChildren(BrodalNode * first, int n)
	{
		BrodalNode * last = first;
		first->parent = nullptr;
		for (int i = 1; i < n; i++)
		{
			last = last->next;
			last->parent = nullptr;
		}

		if (this->child == first)
		{
			this->child = last->next;
			this->rank = (this->child != nullptr) ? this->child->rank + 1 : 0;
		}

		if (first->prev != nullptr)
		{
			first->prev->next = last->next;
		}

		if (last->next != nullptr)
		{
			last->next->prev = first->prev;
		}

		first->prev = nullptr;
		last->next = nullptr;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::removeFromViolationSet()
	{
		this->removeFromViolationSet(this->rank);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::removeFromViolationSet(int trueRank)
	{
		if (this->inThisOnesSet != nullptr)
		{
			this->inThisOnesSet->removeFromSet(this, trueRank);
		}

		if (this->nextInSet != nullptr)
		{
			this->nextInSet->prevInSet = this->prevInSet;
		}

		if (this->prevInSet != nullptr)
		{
			this->prevInSet->nextInSet = this->nextInSet;
		}

		this->prevInSet = nullptr;
		this->nextInSet = nullptr;
		this->inThisOnesSet = nullptr;
		this->inVSet = false;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::addChildAtFront(BrodalNode * newChild)
	{
		newChild->parent = this;

		if (this->child == nullptr)
		{
			this->child = newChild;
			newChild->prev = nullptr;
			newChild->next = nullptr;
		}
		else
		{
			newChild->prev = nullptr;
			newChild->next = this->child;
			this->child->prev = newChild;
			this->child = newChild;
		}

		this->checkRank();
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::addChildAfter(BrodalNode * after, BrodalNode * childToAdd)
	{
		childToAdd->prev = after;
		childToAdd->next = after->next;
		if (after->next != nullptr)
		{
			after->next->prev = childToAdd;
		}
		after->next = childToAdd;
		childToAdd->parent = this;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::replaceWith(BrodalNode * replacement)
	{
		replacement->prev = this->prev;
		replacement->next = this->next;
		replacement->parent = this->parent;

		if (this->prev != nullptr)
		{
			this->prev->next = replacement;
		}

		if (this->next != nullptr)
		{
			this->next->prev = replacement;
		}

		if (this->parent->child == this)
		{
			this->parent->child = replacement;
		}

		this->prev = nullptr;
		this->next = nullptr;
		this->parent = nullptr;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::BrodalNode::lasrgestChildrenCount()
	{
		int count(0);

		BrodalNode * item = this->child;
		int thisrank = this->child->getRank();
		while (item != nullptr && item->getRank() == thisrank)
		{
			count++;
			item = item->next;
		}

		return count;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::checkRank()
	{
		this->rank = this->child->getRank() + 1;
		if (static_cast<size_t>(this->rank) >= this->auxW.size())
		{
			size_t newsize(this->auxW.size() >= 4 ? this->auxW.size() << 1 : 4);
			this->auxW.resize(newsize, nullptr);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::BrodalNode::removeFromSet(BrodalNode * itemToRemove, int trueRank)
	{
		if (this->setV == itemToRemove)
		{
			this->setV = itemToRemove->nextInSet;
		}
		else if (this->setW == itemToRemove)
		{
			this->setW = itemToRemove->nextInSet;
		}

		if (this->auxW.size() <= static_cast<size_t>(trueRank))
		{
			return;
		}

		if (this->auxW[trueRank] == itemToRemove)
		{
			if (itemToRemove->nextInSet != nullptr && (trueRank == itemToRemove->nextInSet->rank))
			{
				this->auxW[trueRank] = itemToRemove->nextInSet;
			}
			else
			{
				this->auxW[trueRank] = nullptr;
			}
		}
	}

	//
	// Reducer
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::Reducer::Reducer(RootWrap * root) :
		root(root)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::Reducer::~Reducer()
	{
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::Reducer::getRoot() -> RootWrap *
	{
		return this->root;
	}

	//
	// ReducerUpper
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerUpper::ReducerUpper(RootWrap * root) :
		Reducer(root)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerUpper::~ReducerUpper()
	{
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::ReducerUpper::reduce(int rank)
	{
		this->getRoot()->reduceUpperBound(rank);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::ReducerUpper::getValue(int index)
	{
		return this->getRoot()->getValUpperBound(index);
	}

	//
	// ReducerLower
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerLower::ReducerLower(RootWrap * root) :
		Reducer(root)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerLower::~ReducerLower()
	{
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::ReducerLower::reduce(int rank)
	{
		this->getRoot()->reduceLowerBound(rank);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::ReducerLower::getValue(int index)
	{
		return this->getRoot()->getValLowerBound(index);
	}

	//
	// ReducerViolation
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerViolation::ReducerViolation(RootWrapT1 * root) :
		Reducer(nullptr),
		root(root)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::ReducerViolation::~ReducerViolation()
	{
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::ReducerViolation::reduce(int rank)
	{
		this->root->reduceViolation(rank);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::ReducerViolation::getValue(int index)
	{
		return this->root->getValViolation(index);
	}

	//
	// FirstInBlock
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::FirstInBlock::FirstInBlock() :
		refCount(1),
		index(-1)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::FirstInBlock::~FirstInBlock()
	{
	}

	template<typename N, typename E>
	size_t BrodalQueue<N, E>::FirstInBlock::getRefCount()
	{
		return this->refCount;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::FirstInBlock::incRefCount()
	{
		this->refCount++;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::FirstInBlock::decRefCount()
	{
		this->refCount--;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::FirstInBlock::isNull()
	{
		return this->index == -1;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::FirstInBlock::setNull()
	{
		this->index = -1;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::FirstInBlock::getIndex()
	{
		return this->index;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::FirstInBlock::setIndex(int i)
	{
		return this->index = i;
	}

	//
	// Guide
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::Guide::Guide(Reducer * reducer) :
		domainSize(0),
		reducer(reducer)
	{
		this->numbers.resize(4, 0);
		this->blockPointers.resize(4, nullptr);
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::Guide::~Guide()
	{
		for (int i(0); i < this->domainSize; i++)
		{
			this->decRefCount(i);
		}

		delete this->reducer;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::increaseDomain()
	{
		int currSize = static_cast<int>(this->numbers.size());
		if (currSize == this->domainSize)
		{
			this->numbers.resize(currSize << 1, 0);
			this->blockPointers.resize(currSize << 1, nullptr);
		}

		this->blockPointers[this->domainSize] = new FirstInBlock();
		this->domainSize++;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::decreaseDomain()
	{
		if (this->domainSize < 1)
		{
			return;
		}

		this->domainSize--;
		FirstInBlock * pointer = this->blockPointers[this->domainSize];
		if (pointer != nullptr)
		{
			pointer->setNull();
			this->decRefCount(this->domainSize);
			this->numbers[this->domainSize] = 0;
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::setReducer(Reducer * newReducer)
	{
		delete this->reducer;
		this->reducer = newReducer;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::possiblyIncrease(int index, int val)
	{
		if (this->numbers[index] >= val)
		{
			return;
		}

		if (this->isInBlock(index))
		{
			this->incInBlock(index);
		}
		else
		{
			this->incOutOfBlock(index);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::incInBlock(int i)
	{
		if (this->numbers[i] >= this->reducer->getValue(i))
		{
			return;
		}

		this->numbers[i] = this->reducer->getValue(i);

		if (this->numbers[i] == 1 || this->numbers[i] == 2)
		{
			int firstBlock = this->blockPointers[i]->getIndex();
			this->blockPointers[i]->setNull();
			this->incOutOfBlock(firstBlock);
			if (this->numbers[i] == 2)
			{
				this->incOutOfBlock(i);
			}
		}
		else
		{
			this->reduce(i);

			if (i >= this->domainSize)
			{
				// REDO toto je dôsledok zlého dizajnu manuálneho checkovania poètu potomov
				// ideálne bude to celé prerobi aby sa nemiešalo manuálne s Guide
				//
				// problém pol že this->reduce(i) zmení this->domainSize takže je iná ako na
				// zaèiatku tejto metódy ... neprípustné SHAME SHAME SHAME SHAME SHAME SHAME 
				return;
			}

			this->blockPointers[i]->setNull();

			if (this->isInBlock(i + 1))
			{
				this->incInBlock(i + 1);
			}
			else if (i != this->domainSize - 1)
			{
				this->incOutOfBlock(i + 1);
			}
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::incOutOfBlock(int i)
	{
		this->numbers[i] = this->reducer->getValue(i);

		if (this->numbers[i] < 2)
		{
			return;
		}

		this->reduce(i);

		if (this->isInBlock(i + 1) && this->reducer->getValue(i + 1) == 1)
		{
			this->numbers[i + 1] = 1;
			this->decRefCount(i);
			this->blockPointers[i] = this->blockPointers[i + 1];
			this->blockPointers[i]->incRefCount();
		}
		else if (i < this->domainSize - 1 && this->reducer->getValue(i + 1) == 2)
		{
			this->numbers[i + 1] = 2;
			FirstInBlock * newBlock = new FirstInBlock();
			newBlock->setIndex(i + 1);
			this->decRefCount(i);
			this->decRefCount(i + 1);
			this->blockPointers[i] = newBlock;
			this->blockPointers[i + 1] = newBlock;
			newBlock->incRefCount();
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::reduce(int i)
	{
		this->reducer->reduce(i);
		this->numbers[i] = this->reducer->getValue(i);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::Guide::decRefCount(int i)
	{
		this->blockPointers[i]->decRefCount();
		if (this->blockPointers[i]->getRefCount() == 0)
		{
			delete this->blockPointers[i];
		}
		this->blockPointers[i] = nullptr;
	}

	template<typename N, typename E>
	bool BrodalQueue<N, E>::Guide::isInBlock(int index)
	{
		return index < this->domainSize && !this->blockPointers[index]->isNull();
	}

	//
	// RootWrap
	//
	template<typename N, typename E>
	const int BrodalQueue<N, E>::RootWrap::T1 = 1;

	template<typename N, typename E>
	const int BrodalQueue<N, E>::RootWrap::T2 = 2;

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrap::RootWrap(BrodalNode * item) :
		rootItem(item),
		guideUpper(new Guide(new ReducerUpper(this))), // TODO no need to allocate using new...
		guideLower(new Guide(new ReducerLower(this)))
	{
		this->children.resize(4, nullptr);
		this->childrenCount.resize(4, 0);
		item->parent = nullptr;
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrap::RootWrap(RootWrap && other) :
		childrenCount(std::move(other.childrenCount)),
		children(std::move(other.children))
	{
		this->rootItem = other.rootItem;
		this->guideUpper = other.guideUpper;
		this->guideLower = other.guideLower;

		other.rootItem = nullptr;
		other.guideUpper = nullptr;
		other.guideLower = nullptr;
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrap::~RootWrap()
	{
		delete this->guideUpper;
		delete this->guideLower;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::increaseRank(BrodalNode * first, BrodalNode * second)
	{
		this->rootItem->addChildAtFront(first);
		this->rootItem->addChildAtFront(second);
		this->childrenCount[first->getRank()] = 2;
		this->children[first->getRank()] = second;
		this->increaseDomains();
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::addChild(BrodalNode * chld, bool guideAdding)
	{
		int chldRank = chld->getRank();

		BrodalNode * afterThis = this->children[chldRank];
		this->rootItem->addChildAfter(afterThis, chld);
		this->childrenCount[chldRank]++;

		int rootRank = this->rootItem->getRank();
		if (chldRank < rootRank - 2 && !guideAdding)
		{
			int guideVal = this->guideNumberUpper(this->childrenCount[chldRank]);
			this->guideUpper->possiblyIncrease(chldRank, guideVal);
		}
		else
		{
			this->checkLargeChildrenUpper(chldRank);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::addLinkedItems(BrodalNode * items, bool guideAdding)
	{
		while (items != nullptr)
		{
			BrodalNode * newNext = items->getNext();
			this->addChild(items, guideAdding);
			items = newNext;
		}
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::removeChild(int rank, bool guideRemoving) -> BrodalNode *
	{
		BrodalNode * ret = this->removeChildUnchecked(rank);

		if (rank < this->rootItem->getRank() - 2 && !guideRemoving)
		{
			int guideVal = this->guideNumberLower(this->childrenCount[rank]);
			this->guideLower->possiblyIncrease(rank, guideVal);
		}
		else
		{
			this->checkLargeChildrenLower(rank);
		}

		return ret;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::getFirstZeroRankChild() -> BrodalNode *
	{
		return this->children[0];
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::getItemOfRank(int rank) -> BrodalNode *
	{
		return this->children[rank]->getNext();
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::removeChild(BrodalNode * child)
	{
		this->removeChild(child, child->getRank());
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::reduceUpperBound(int rank)
	{
		BrodalNode * newChild = this->linkChildren(rank);
		this->addChild(newChild, true);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::reduceLowerBound(int rank)
	{
		bool removingAtEdge = (rank == this->rootItem->getRank() - 3);
		BrodalNode * childToDelink = this->removeChildUnchecked(rank + 1);
		this->removeViolation(childToDelink); //TODO treba toto?
		BrodalNode * items = childToDelink->remove2To3LargestChildren();
		this->addLinkedItems(items, true);
		this->addExtraItem(childToDelink);
		if (removingAtEdge)
		{
			this->checkLargeChildrenLower(rank + 1);
		}
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrap::getValUpperBound(int index)
	{
		return this->guideNumberUpper(this->childrenCount[index]);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrap::getValLowerBound(int index)
	{
		return this->guideNumberLower(this->childrenCount[index]);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::increaseDomains()
	{
		int childrenSize = static_cast<int>(this->children.size());
		int rootRank = this->rootItem->getRank();

		if (rootRank == childrenSize)
		{
			this->children.resize(childrenSize << 1, nullptr);
			this->childrenCount.resize(childrenSize << 1, 0);
		}

		if (rootRank > 2)
		{
			this->guideUpper->increaseDomain();
			this->guideLower->increaseDomain();

			int newIndexToGuide = rootRank - 3;
			this->guideUpper->possiblyIncrease(newIndexToGuide, this->guideNumberUpper(this->childrenCount[newIndexToGuide]));
			this->guideLower->possiblyIncrease(newIndexToGuide, this->guideNumberLower(this->childrenCount[newIndexToGuide]));
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::decreaseDomains()
	{
		this->guideUpper->decreaseDomain();
		this->guideLower->decreaseDomain();
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::checkChildrenLower(int rank)
	{
		if (rank < this->rootItem->getRank() - 2)
		{
			int guideValue = this->getValLowerBound(rank);
			this->guideLower->possiblyIncrease(rank, guideValue);
		}
		else
		{
			this->checkLargeChildrenLower(rank);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::removeChild(BrodalNode * child, int trueRank)
	{
		this->childrenCount[trueRank]--;

		if (this->children[trueRank] == child)
		{
			this->children[trueRank] = child->getNext();
		}

		this->rootItem->removeNChildren(child, 1);
		this->checkChildrenLower(trueRank);
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::getRootItem() -> BrodalNode *
	{
		return this->rootItem;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::removeChildUnchecked(int rank) -> BrodalNode *
	{
		this->childrenCount[rank]--;
		BrodalNode * ret = this->children[rank];
		this->children[rank] = ret->getNext();
		this->rootItem->removeNChildren(ret, 1);
		return ret;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrap::guideNumberUpper(int count)
	{
		if (count <= 5)
		{
			return 0;
		}
		else
		{
			return count - 5;
		}
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrap::guideNumberLower(int count)
	{
		return this->guideNumberUpper(-count + 9);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::checkLargeChildrenUpper(int rank)
	{
		if (this->childrenCount[rank] <= 7)
		{
			return;
		}

		if (rank == this->rootItem->getRank() - 2)
		{
			BrodalNode * newChild = this->linkChildren(rank);
			this->addChild(newChild, false);
		}
		else
		{
			BrodalNode * firstNew = this->linkChildren(rank);
			BrodalNode * secondNew = this->linkChildren(rank);
			this->increaseRank(firstNew, secondNew);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrap::checkLargeChildrenLower(int rank)
	{
		if (this->childrenCount[rank] >= 2)
		{
			return;
		}

		if (rank == this->rootItem->getRank() - 2)
		{
			BrodalNode * delinkedItem = this->removeChildUnchecked(rank + 1);
			BrodalNode * items = delinkedItem->remove2To3LargestChildren();
			this->addLinkedItems(items, true);
			this->addExtraItem(delinkedItem);
			this->checkLargeChildrenLower(rank + 1);
		}
		else
		{
			BrodalNode * lastOne = this->children[rank];
			BrodalNode * delinkedOnes = lastOne->remove2To3LargestChildren();
			bool rankDecreased = false;
			if (lastOne->getRank() < rank)
			{
				rankDecreased = true;
				this->decreaseDomains();
				this->children[rank] = nullptr;
				this->childrenCount[rank] = 0;
				this->rootItem->removeNChildren(lastOne, 1);
				this->addChild(lastOne, false);
			}

			this->addLinkedItems(delinkedOnes, false);
			if (!rankDecreased)
			{
				this->checkLargeChildrenLower(rank);
			}
		}
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::linkChildren(int rank) -> BrodalNode *
	{
		BrodalNode * x1 = this->children[rank];
		BrodalNode * x2 = x1->getNext();
		BrodalNode * x3 = x2->getNext();

		this->children[rank] = x3->getNext();
		this->rootItem->removeNChildren(x1, 3);
		this->childrenCount[rank] -= 3;

		BrodalNode * newRoot = this->maxPrio(x1, this->maxPrio(x2, x3));
		this->removeViolation(newRoot);

		if (x1 != newRoot)
		{
			newRoot->addChildAtFront(x1);
		}
		if (x2 != newRoot)
		{
			newRoot->addChildAtFront(x2);
		}
		if (x3 != newRoot)
		{
			newRoot->addChildAtFront(x3);
		}

		return newRoot;
	}

	template<typename N, typename E>
	auto BrodalQueue<N, E>::RootWrap::maxPrio(BrodalNode * x1, BrodalNode * x2) -> BrodalNode *
	{
		if (*x1 < *x2)
		{
			return x1;
		}
		else
		{
			return x2;
		}
	}

	//
	// RootWrapT1
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrapT1::RootWrapT1(BrodalNode * rootItem) :
		RootWrap(rootItem),
		guideViolation(new Guide(new ReducerViolation(this))),
		t2Wrap(nullptr)
	{
		this->violationCount.resize(4, 0);
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrapT1::~RootWrapT1()
	{
		delete this->guideViolation;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::reduceViolation(int rank)
	{
		int t2ViolsRemoved = this->t2ViolationsReduce(rank);

		if (t2ViolsRemoved < 2)
		{
			this->normalViolationsReduce(rank, 2 - t2ViolsRemoved);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::addChild(BrodalNode * chld, bool guideAdding)
	{
		this->removeViolation(chld);
		RootWrap::addChild(chld, guideAdding);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::increaseRank(BrodalNode * first, BrodalNode * second)
	{
		this->removeViolation(first);
		this->removeViolation(second);
		RootWrap::increaseRank(first, second);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::removeViolation(BrodalNode * item)
	{
		int rank = item->getRank();
		this->removeViolation(item, rank);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::addExtraItem(BrodalNode * item)
	{
		this->addChild(item, false);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrapT1::getValViolation(int index)
	{
		return this->getNumberViolation(this->violationCount[index]);
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrapT1::getType()
	{
		return RootWrap::T1;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::setOtherWrap(RootWrapT2 * wrap)
	{
		this->t2Wrap = wrap;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::addPossibleViolation(BrodalNode * violItem, bool guideAdding)
	{
		this->removeViolation(violItem);

		if (!violItem->isViolating())
		{
			return;
		}

		if (violItem->getRank() < this->getRootItem()->getRank())
		{
			this->addViolationSmall(violItem, guideAdding);
		}
		else {
			this->addViolationLarge(violItem);
		}
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrapT1::t2ViolationsReduce(int rank)
	{
		BrodalNode * t1 = this->getRootItem();
		BrodalNode * wItem = t1->getWViolation(rank);
		int removedCount(0);

		while (wItem != nullptr && wItem->getRank() == rank && removedCount < 2)
		{
			BrodalNode * toRemoveItem = wItem;
			wItem = wItem->getNextInSet();

			if (toRemoveItem->isSonOfRoot())
			{
				removedCount++;
				this->t2Wrap->removeChild(toRemoveItem);
				this->addChild(toRemoveItem, false);
			}
		}

		return removedCount;
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrapT1::getNumberViolation(int count)
	{
		if (count < 5)
		{
			return 0;
		}
		else
		{
			return count - 4;
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::normalViolationsReduce(int rank, int toRemoveCount)
	{
		BrodalNode * x1;
		BrodalNode * x2;

		if (toRemoveCount == 1)
		{
			this->getNormalViolations(rank, 1, &x1, &x2);
			this->normalViolReduceSingle(x1);
		}
		else
		{
			this->getNormalViolations(rank, 2, &x1, &x2);

			if (!x1->isSibling(x2))
			{
				this->makeSiblings(x1, x2);
			}

			this->normalViolReduceSingle(x1);
			this->normalViolReduceSingle(x2);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::normalViolReduceSingle(BrodalNode * x1)
	{
		this->removeViolation(x1);

		if (!x1->isViolating())
		{
			return;
		}

		if (x1->sameRankSiblingCount() > 2)
		{
			x1->getParent()->removeNChildren(x1, 1);
			this->addChild(x1, false);
		}
		else
		{
			this->cutWithSibling(x1);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::cutWithSibling(BrodalNode * x1)
	{
		BrodalNode * parent = x1->getParent();
		BrodalNode * sibling = x1->getSameRankSibling();

		int originalParentRank = parent->getRank();

		parent->removeNChildren(x1, 1);
		parent->removeNChildren(sibling, 1);

		int actualParentRank = parent->getRank();

		if (actualParentRank < originalParentRank)
		{
			if (parent->isSonOfRoot())
			{
				this->removeChild(parent, originalParentRank);
			}
			else
			{
				BrodalNode * parentReplacement = this->removeChildUnchecked(originalParentRank);
				parent->replaceWith(parentReplacement);
				this->addPossibleViolation(parentReplacement, true);
				this->checkChildrenLower(originalParentRank);
			}

			this->removeViolation(parent, originalParentRank);
			this->addChild(parent, false);
		}

		this->addChild(x1, false);
		this->addChild(sibling, false);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::makeSiblings(BrodalNode * x1, BrodalNode * x2)
	{
		if (x1->getParent()->getEntry()->getPrio() < x2->getParent()->getEntry()->getPrio())
		{
			BrodalNode * sibling = x2->getSameRankSibling();
			x1->swapWith(sibling);
		}
		else
		{
			BrodalNode * sibling = x1->getSameRankSibling();
			x2->swapWith(sibling);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::addViolationSmall(BrodalNode * violItem, bool guideAdding)
	{
		this->getRootItem()->addToWSet(violItem);
		int rank = violItem->getRank();
		this->violationCount[rank]++;
		if (!guideAdding)
		{
			this->guideViolation->possiblyIncrease(rank, this->getValViolation(rank));
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::addViolationLarge(BrodalNode * violItem)
	{
		this->getRootItem()->addToVSet(violItem);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::mergeViolationSetIntoW(BrodalNode * set)
	{
		while (set != nullptr)
		{
			BrodalNode * next = set->getNextInSet();
			this->addPossibleViolation(set, false);
			set = next;
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::removeViolation(BrodalNode * item, int trueRank)
	{
		BrodalNode * t1Root = this->getRootItem();

		if (!item->isInVSet() && item->isInThisSet(t1Root))
		{
			this->violationCount[trueRank]--;
		}

		item->removeFromViolationSet(trueRank);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::totallyReduceWViolations()
	{
		BrodalNode * rootItem = this->getRootItem();
		for (int i = 0; i < rootItem->getRank(); i++)
		{
			while (this->violationCount[i] > 1)
			{
				this->normalViolReduceSingle(rootItem->getWViolation(i));
			}
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::mergeViolationSets(BrodalNode * newMin)
	{
		this->mergeViolationSetIntoW(this->getRootItem()->disconnectSetV());
		this->mergeViolationSetIntoW(newMin->disconnectSetV());
		this->mergeViolationSetIntoW(newMin->disconnectSetW());
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::incRankAndAddRest(BrodalNode * linked)
	{
		BrodalNode * x1 = linked;
		linked = linked->getNext();
		BrodalNode * x2 = linked;
		linked = linked->getNext();

		this->increaseRank(x1, x2);

		if (linked != nullptr)
		{
			this->addLinkedItems(linked, false);
		}
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::increaseDomains()
	{
		RootWrap::increaseDomains();

		int newIndexToGuide = this->getRootItem()->getRank() - 1;

		if (this->violationCount.size() == static_cast<size_t>(newIndexToGuide + 1))
		{
			this->violationCount.resize(this->violationCount.size() << 1, 0);
		}

		this->guideViolation->increaseDomain();
		this->guideViolation->possiblyIncrease(
			newIndexToGuide,
			this->getNumberViolation(this->violationCount[newIndexToGuide])
		);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::decreaseDomains()
	{
		RootWrap::decreaseDomains();
		this->guideViolation->decreaseDomain();
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT1::getNormalViolations(int rank, int count, BrodalNode ** first, BrodalNode ** second)
	{
		BrodalNode * ret[2];
		BrodalNode * wItem = this->getRootItem()->getWViolation(rank);
		BrodalNode * t2Root = (this->t2Wrap != nullptr) ? this->t2Wrap->getRootItem() : nullptr;
		int foundCount = 0;

		while (foundCount < count)
		{
			if (wItem->getParent() != t2Root)
			{
				ret[foundCount++] = wItem;
			}
			wItem = wItem->getNextInSet();
		}

		*first = ret[0];
		*second = ret[1];
	}

	//
	// RootWrapT2
	//
	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrapT2::RootWrapT2(BrodalNode * rootItem) :
		RootWrap(rootItem)
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrapT2::RootWrapT2(RootWrap && wrap) :
		RootWrap(std::move(wrap))
	{
	}

	template<typename N, typename E>
	BrodalQueue<N, E>::RootWrapT2::~RootWrapT2()
	{
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT2::addChild(BrodalNode * child, bool guideAdding)
	{
		RootWrap::addChild(child, guideAdding);
		this->t1Wrap->addPossibleViolation(child, false);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT2::removeViolation(BrodalNode * item)
	{
		this->t1Wrap->removeViolation(item);
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT2::addExtraItem(BrodalNode * item)
	{
		if (item->getRank() < this->t1Wrap->getRootItem()->getRank())
		{
			this->t1Wrap->addExtraItem(item);
		}
		else
		{
			this->addChild(item, false);
		}
	}

	template<typename N, typename E>
	int BrodalQueue<N, E>::RootWrapT2::getType()
	{
		return RootWrap::T2;
	}

	template<typename N, typename E>
	void BrodalQueue<N, E>::RootWrapT2::setOtherWrap(RootWrapT1 * wrap)
	{
		this->t1Wrap = wrap;
	}

}