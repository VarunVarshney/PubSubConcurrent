//
//  Interface.h
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#ifndef Interface_h
#define Interface_h

#include "InterfaceUtil.h"

/**
 * Interface to use PubSub lib by any client.
 * PubSub - Is a multithreaded publisher-subscriber library which manages thread management on its own
 * Supports multiple concurrent calls as pulishers/subscribers with multiple topics
 * A bound on request submission is configurable
 *
 * Currently :
 * 1. Uses 1 background thread for Pub/Sub job execution
 * 2. Uses maximum number of threads as per hardware concurrency for distributing subsciber callbacks
 * 3. For simulation in main function, I have simulated using two subscriber/publisher threads. This cab be tested with more threads
 */

/**
 * This API is for initializing the PubSub library. Internally it sets up the managers required fro threadmanagement
 * This is a MUST HAVE step before kickstarting any PubSub operations
 *
 *
 * -------
 * @param inInitConfiguration struct data currently containing the maximum size of Blocking Bounded buffer
  * @return PubSubLibStatusCode
 */
PubSubLibStatusCode PubSubInit(PubSubInitConfiguration inInitConfiguration);

/**
 * This API is to be called by Publishers to publish a new HTML page for a topic
 *
 *
 * -------
 * @param inTopic currently a topic is represented by a string
 * @param inTopicPageData currently a topic's HTML page is assumed to be a string
  * @return PubSubLibStatusCode
 */
PubSubLibStatusCode PublishTopic(std::string inTopic, std::string inTopicPageData);

/**
 * This API is to be called by Subscribers to subscribe to a topic
 *
 *
 * -------
 * @param inTopic topic to be subscribed
 * @param inSubscriberName name of the subscriber
 * @param inCallback callback to be called when a new page is published for that topic
  * @return PubSubLibStatusCode
 */
PubSubLibStatusCode Subscribe(std::string inTopic,
                              std::string inSubscriberName,
                              SubscriberCallback inCallback);
void PubSubTerminate();

#endif /* Interface_h */
