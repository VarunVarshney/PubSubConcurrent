//
//  main.cpp
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#include <iostream>
#include <thread>

#include "Interface.h"
using namespace std;


// mutex for cout, to clean print
std::mutex m_cout;

void GeneralSubscriberCallback(std::string inTopic, std::string inHTMLPage) {
    {
        std::lock_guard<std::mutex> lock(m_cout);
        cout << "\tCalling callback | Topic = " << inTopic << ", Topic PageData = " << inHTMLPage << endl;
    }
}

int main(int argc, const char * argv[]) {
    
    
    // Using only interface APIs
    // Simulating concurrent calls using publisher and subscription threads.
    // Lets asume there are two threads who are simultaneously calling

    // Set config parameteres
    PubSubInitConfiguration config;
    config.maxPublishInQueue = 5; // 5 job requests in the blocking queue
    
    // Initialize PubSub
    PubSubLibStatusCode errorCode = PubSubInit(config);

    // Simulate simultaneous calls using two threads
    // Using a mutex to print the submission or result

    PubSubLibStatusCode PublishTopic(std::string inTopic, std::string inTopicPageData);
    PubSubLibStatusCode Subscribe(std::string inTopic,
                                  std::string inSubscriberName,
                                  SubscriberCallback inCallback);

    // Thread to make calls as Publisher after every 1 second
    std::thread publisher([&](){
        for (int i = 1; i <= 20; i++) {

            // making loop number as topic name & html data as "HTML body of topic <topic name>"
            // So something like this :
            // 1, HTML body of topic 1
            // 2, HTML body of topic 2
            // Creating a random topic publish request
            int randomTopicNum = rand() % 3 + 1;
            std::string topic = std::to_string(randomTopicNum);
            
            int topicdataNum = rand() % 100 + 1;
            std::string topicdata = std::to_string(topicdataNum);
            
            // Random topic between 1 to 3, Random data in HTML body between 1- 100
            PublishTopic(topic, "HTML body " + topicdata);
            std::this_thread::sleep_for(chrono::seconds(1));
        }
    });

    // Thread to make calls as Subscriber after every 1 second
    std::thread subscriber([&](){
        for (int i = 1; i <= 20; i++) {
            int randomTopicNum = rand() % 3 + 1;
            std::string topic = std::to_string(randomTopicNum);

            int topicdataNum = rand() % 100 + 1;
            std::string topicdata = std::to_string(topicdataNum);

            // Random topic between 1 to 3
            Subscribe(topic, "<generic username>", GeneralSubscriberCallback);
            std::this_thread::sleep_for(chrono::seconds(1));
        }
    });

    // Wait for testing threads to complete
    publisher.join();
    subscriber.join();

    // Terminate PubSub
    PubSubTerminate();

    return 0;
}
