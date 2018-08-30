// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * The basic queue does not count the number of items... We do it here.
 */
template<typename ItemType, EQueueMode Mode = EQueueMode::Spsc>
class NOTIFICATIONBACKBONE_API TQueueCustom : public TQueue<ItemType, Mode>
{
public:

	bool Dequeue(ItemType& OutItem)
	{
		bool retVal = TQueue::Dequeue(OutItem);
		if (retVal)
		{
			--numElements;
		}
		return retVal;
	}

	void Empty()
	{
		TQueue::Empty();
		numElements = 0;
	}

	bool Enqueue(const ItemType& Item)
	{
		bool retVal = TQueue::Enqueue(Item);
		if (retVal)
		{
			++numElements;
		}
		return retVal;
	}

	bool Enqueue(ItemType&& Item)
	{
		bool retVal = TQueue::Enqueue(Item);
		if (retVal)
		{
			++numElements;
		}
		return retVal;
	}

	bool Pop()
	{
		bool retVal = TQueue::Pop();
		if (retVal)
		{
			--numElements;
		}
		return retVal;
	}

	uint32 Num() const 
	{
		check((IsEmpty() && numElements == 0) || (!IsEmpty() && numElements != 0));
		return numElements;
	}

private:

	uint32 numElements = 0;
};
