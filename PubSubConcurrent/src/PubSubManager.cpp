//
//  PubSubManager.cpp
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#include "PubSubManager.h"
#include <iostream>
#include <string>
using namespace std;

// for sync in couts
extern std::mutex m_cout;

// for enabling submission logs
#define LOG_LEVEL 0

namespace pubsub {
    namespace manager {
        
        PubSubManager::~PubSubManager()
        {
            // So executor can also exit
            fAbort = true;
            
            // Trigger signals so abort can be checked
            mBlockingQueueFullCV.notify_all();
            mBlockingQueueEmptyCV.notify_all();
            
            // Wait for future if executor still working
            if (fExecutorFuture.valid())
            {
                fExecutorFuture.wait();
            }
        }
        
        void PubSubManager::StartJobRequestExecutorIfNotStarted()
        {
            PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;

            // Start pubsub executor on a background thread, can be extended to threadpool
            if (mBoolExecutorStarted == false)
            {
                fExecutorFuture = std::async(std::launch::async, &PubSubManager::ExecuteJobRequests, this);
                mBoolExecutorStarted = true;
            } // if (mBoolExecutorStarted == false)
            
            return;
        }

        PubSubLibStatusCode PubSubManager::HandlePublishTopic(std::string& inTopic, std::string& inTopicPageData){
            PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;
            
            util::PublishORSubscriptionInfo temp;
            temp.mStrTopic = inTopic;
            temp.mStrTopicPageData = inTopicPageData;
            temp.jobtype = util::JOB_TYPE_PUBLISH;

            StartJobRequestExecutorIfNotStarted();
            PutJobRequestInQueue(temp);

            return bStatusCode;
        }
        
        PubSubLibStatusCode PubSubManager::HandleSubscription(std::string& inTopic,
                                                              std::string& inSubscriberName,
                                                              SubscriberCallback inCallback)
        {
            PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;
            
            util::PublishORSubscriptionInfo temp;
            temp.mStrTopic = inTopic;
            temp.mStrSubscriberName = inSubscriberName;
            temp.mSubscriptionCallback = inCallback;
            temp.jobtype = util::JOB_TYPE_SUBSCRIBE;

            PutJobRequestInQueue(temp);

            return bStatusCode;
        }
        
        void PubSubManager::PutJobRequestInQueue(const util::PublishORSubscriptionInfo& inJobRequest)
        {
            std::unique_lock<std::mutex> lock(mutexBlockingQueue);
            mBlockingQueueFullCV.wait(lock, [&](){return mBlockingQueue.size() != mInitConfiguration.maxPublishInQueue;});

            // Push request directly, processing is done when job is pulled out
            mBlockingQueue.push(inJobRequest);
            
            if (LOG_LEVEL > 0)
            {
                std::lock_guard<std::mutex> lock(m_cout);
                if (inJobRequest.jobtype == util::JOB_TYPE_PUBLISH)
                    cout << "Submission Job | Publish | Topic = " << inJobRequest.mStrTopic << ", Topic PageData = " << inJobRequest.mStrTopicPageData << endl;
                else
                    cout << "Submission Job | Subscribe | Topic = " << inJobRequest.mStrTopic << endl;
            }

            // Notifies all threads waiting for queue to be empty
            mBlockingQueueEmptyCV.notify_all();
        }
        
        void PubSubManager::ExecuteLogicForPublishJob(util::PublishORSubscriptionInfo &outJobRequest)
        {
            // Fill topic to topic data map
            if (mTopicToTopicPageDataMap.find(outJobRequest.mStrTopic) == mTopicToTopicPageDataMap.end()) {
                pubsub::topic::TopicPageData topicPageData(outJobRequest.mStrTopic, outJobRequest.mStrTopicPageData);
                mTopicToTopicPageDataMap.insert(make_pair(outJobRequest.mStrTopic, topicPageData));
            } else {
                mTopicToTopicPageDataMap[outJobRequest.mStrTopic].PushPageData(outJobRequest.mStrTopic, outJobRequest.mStrTopicPageData);
            }

            {
                std::lock_guard<std::mutex> lock(m_cout);
                cout << "Execute Job | Publish | Topic = " << outJobRequest.mStrTopic << ", Topic PageData = " << outJobRequest.mStrTopicPageData << endl;
            }
            
            // Call all callback functions
            // Ideally we should be submitting to a threadpool class so context creation/destroy of threads is avoided.
            // For the timebeing lets just create a few threads based on OS concurrency and distribute the callback calls to them for quick
            // execution

            int hardwareconc = std::thread::hardware_concurrency();
            std::vector<SubscriberCallback> callbacks = mTopicToSubscriptionCallbackMap[outJobRequest.mStrTopic];
            std::vector<std::thread> workers;

            int nLeft = callbacks.size();;
            int curindex = 0;
            while (nLeft > 0) {
                int nthreads = std::min(hardwareconc, nLeft);
                for (int j = 0; j < nthreads ; j++) {
                    // send new payload or new HTML
                    workers.push_back(std::thread(callbacks[curindex++], outJobRequest.mStrTopic, outJobRequest.mStrTopicPageData));
                }

                for (std::thread &t : workers) {
                    if (t.joinable()) t.join();
                }

                nLeft = nLeft - nthreads;
            } // while (nleft > 0)
        }
        
        void PubSubManager::ExecuteLogicForSubscribeJob(util::PublishORSubscriptionInfo &outJobRequest)
        {
            if (mTopicToSubscriptionCallbackMap.find(outJobRequest.mStrTopic) == mTopicToSubscriptionCallbackMap.end()) {
                std::vector<SubscriberCallback> temp;
                temp.push_back(outJobRequest.mSubscriptionCallback);
                mTopicToSubscriptionCallbackMap.insert(make_pair(outJobRequest.mStrTopic, temp));
            } else {
                mTopicToSubscriptionCallbackMap[outJobRequest.mStrTopic].push_back(outJobRequest.mSubscriptionCallback);
            }
            
            {
                std::lock_guard<std::mutex> lock(m_cout);
                cout << "Execute Job | Subscribe | Topic = " << outJobRequest.mStrTopic << ", total subs now = " << std::to_string(mTopicToSubscriptionCallbackMap[outJobRequest.mStrTopic].size()) << endl;
            }
        }
        
        void PubSubManager::ExecuteJobRequests()
        {
            while (!fAbort)
            {
                std::unique_lock<std::mutex> lock(mutexBlockingQueue);
                mBlockingQueueEmptyCV.wait(lock, [&](){return !mBlockingQueue.empty();});

                // Pop job
                util::PublishORSubscriptionInfo outJobRequest = mBlockingQueue.front();

                // in case of publish job request,update the topic to page mapping
                // Also call all callback functions
                if (outJobRequest.jobtype == util::JOB_TYPE_PUBLISH) {
                    ExecuteLogicForPublishJob(outJobRequest);
                } else if (outJobRequest.jobtype == util::JOB_TYPE_SUBSCRIBE) {
                    ExecuteLogicForSubscribeJob(outJobRequest);
                }
                
                // TO REMOVE : JUST FOR TEST PURPOSE, making this sleep for 2 seconds to simulate that execution takes time
                std::this_thread::sleep_for(chrono::seconds(2));

                mBlockingQueue.pop();
                mBlockingQueueFullCV.notify_all();
            } //  while (!fAbort)
        } // GetJobRequestFromQueue

    } // end namespace topic
} // end namespace pubsub
