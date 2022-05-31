//
//  PubSubManager.h
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#ifndef PubSubManager_h
#define PubSubManager_h

#include <stdio.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <future>
#include <thread>

#include "InterfaceUtil.h"
#include "PubSubUtil.h"
#include "Topic.h"

namespace pubsub {
    namespace manager {
        
        /**
         * This is a thread safe  class is responsible for Blocking queue management which takes in Pub/Sub requests
         * This class also maintains its on async executor which gets signalled in case of new job submissions
         *
         */
        class PubSubManager {
        public:
            // CTors taking in configuration (blocking queue max size)
            PubSubManager() {};
            PubSubManager(PubSubInitConfiguration inInitConfiguration) : mInitConfiguration(inInitConfiguration),
                mBoolExecutorStarted(false){};
            
            // Job submittor functions exposed to Interface via gManager(global manager object)
            PubSubLibStatusCode HandlePublishTopic(std::string& inTopic, std::string& inTopicPageData);
            
            PubSubLibStatusCode HandleSubscription(std::string& inTopic,
                                                   std::string& inSubscriberName,
                                                   SubscriberCallback inCallback);
            
            ~PubSubManager();
        private:
            
            // Job submitter
            void PutJobRequestInQueue(const util::PublishORSubscriptionInfo& inJobRequest);

            // Job executor function
            void ExecuteJobRequests();
            
            // Kickstart internal executor thread
            void StartJobRequestExecutorIfNotStarted();
            
            // Executor helper functions
            void ExecuteLogicForSubscribeJob(util::PublishORSubscriptionInfo &outJobRequest);
            void ExecuteLogicForPublishJob(util::PublishORSubscriptionInfo &outJobRequest);
            
            // HashMap to maintain all Pages to topic data
            std::unordered_map<std::string, pubsub::topic::TopicPageData> mTopicToTopicPageDataMap;
            
            // HashMap to maintain all subscribers to a particular topic
            std::unordered_map<std::string, std::vector<SubscriberCallback>> mTopicToSubscriptionCallbackMap;
            
            // Publish related synchronization variables
            std::mutex mutexBlockingQueue;
            std::condition_variable mBlockingQueueFullCV;
            std::condition_variable mBlockingQueueEmptyCV;

            // Blocking queue to handle requests
            std::queue<pubsub::util::PublishORSubscriptionInfo> mBlockingQueue;

            // Initial configuration data. For now it keeps the max size of Blocking queue
            PubSubInitConfiguration  mInitConfiguration;
            
            // Executor thread variables
            // This can be extended to pool of threads later on
            // For now lets assume we have just one thread working on blocking queue requests
            std::atomic_bool mBoolExecutorStarted;
    
            // need a long future variable as async task will get blocked on a local future destroy
            std::future<void> fExecutorFuture;
            
            // for aborting - If terminate is called
            std::atomic_bool fAbort;
        };
    } // end namespace manager
}// end namespace pubsub

#endif /* PubSubManager_h */
