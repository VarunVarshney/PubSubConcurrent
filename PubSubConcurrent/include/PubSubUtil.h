//
//  Subscriber.h
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#ifndef Subscriber_h
#define Subscriber_h

#include <stdio.h>
#include <string>

#include "InterfaceUtil.h"

namespace pubsub {
    namespace util {

        // Type of job request
        enum JobRequestType {
            JOB_TYPE_PUBLISH = 0,
            JOB_TYPE_SUBSCRIBE
        };
        typedef JobRequestType JobRequestType;

        // Ideally we can make do with something like std::any which can define multiple
        // types of requests in a queue, but for the timebeing using enum
        struct PublishORSubscriptionInfo {
            // Common data
            std::string mStrTopic;
            JobRequestType jobtype;

            // Publisher related data
            std::string mStrTopicPageData;

            // Subscription related data
            std::string mStrSubscriberName;
            SubscriberCallback mSubscriptionCallback;
            // Can have user types, or geo location etc. for audience targetting
        };
        typedef struct PublishORSubscriptionInfo PublishORSubscriptionInfo;

    } // end namespace util
} // end namespace pubusb

#endif /* Subscriber_h */
