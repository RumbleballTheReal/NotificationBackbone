#pragma once

#include "NotificationBackboneListener.h"
#include "NotificationBackboneManager.h"


class NOTIFICATIONBACKBONE_API NBNativeTest : public TSharedFromThis<NBNativeTest>, public INotificationBackboneListenerRaw
{
public:
	NBNativeTest()
	{
	}

	void Init()
	{
		// We cannot do this in the contructor!!
		// AsShared requires this object instance to be assigned to a SharedPtr/Ref before calling it.
		// Doing it in the constructor, it is the first think that happens, thus we can't be assign to a shared holder.
		FNotificationBackboneManager::Get().RegisterForQueries(StaticCastSharedRef<INotificationBackboneListenerRaw>(AsShared()));
		FNotificationBackboneManager::Get().RegisterForNotifications(StaticCastSharedRef<INotificationBackboneListenerRaw>(AsShared()), FName("NativeTestNotification"));
	}

	virtual ~NBNativeTest() {}

#pragma region INotificationBackboneListener
	virtual void OnNotification(const FNotificationBackboneNotification& notification) override
	{
		MF_LOG(Warning, false, "DEBUG: Received notification: %s \t %s \t %s", *notification.feed.ToString(), *notification.title.ToString(), *notification.message.ToString());
	}

	virtual void OnQuery(const FNotificationBackboneQuery& query) override
	{
		MF_LOG(Warning, false, "DEBUG: Received query: %s \t %s", *query.data.title.ToString(), *query.data.message.ToString());
		// At this point you should for sure create a popup for the user. 
		FNotificationBackboneManager::Get().AnswerQuery(query.id, ENotificationBackboneQueryOptions::QO_OK);
	}

	virtual void OnQueryRegistrationStolen() override
	{
		MF_LOG(Warning, false, "DEBUG: QueryRegistrationStolen");
	}

	virtual FName GetNotificationBackboneListenerName() override
	{
		return FName("MFNativeTest");
	}
#pragma endregion INotificationBackboneListener
};