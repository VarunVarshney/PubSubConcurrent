//
//  Topic.h
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#ifndef Topic_h
#define Topic_h

#include <stdio.h>
#include <string>
#include <queue>

// Assumptions :
// 1. Topic identifier is a string
// 2. Topic payload/pagedata is a HTML string as well

namespace pubsub {
    namespace topic {
        /**
         * Core class to store Topic data
         * Maintains a queue to store topic pages
         */
        class TopicPageData {
        public:
            TopicPageData() {};
            virtual ~TopicPageData() {};
            inline TopicPageData(const std::string& inStrTopic, const std::string& inTopicPageData) {mTopicPageList.push(inTopicPageData);}
            inline void PushPageData(const std::string& inStrTopic, const std::string& inTopicPageData) {mTopicPageList.push(inTopicPageData);}
        private:
            std::queue<std::string> mTopicPageList;
            std::string mStrTopicName;
        };
    } // end namespace topic
} // end namespace pubusb

#endif /* Topic_h */
