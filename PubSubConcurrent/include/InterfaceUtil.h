//
//  InterfaceUtil.h
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#ifndef InterfaceUtil_h
#define InterfaceUtil_h

/**
 * An enum for various error codes returned by pubsublib
 */
typedef enum PubSubLibStatusCode
{
    /**
     * No error
     */
    PUBSUBLIB_STATUSCODE_SUCCESS = 0,
    
    /**
     * Topic Limit reached
     */
    PUBSUBLIB_STATUSCODE_TOPIC_LIMIT,
    
    /**
     * Memory allocation failed
     */
    PUBSUBLIB_STATUSCODE_MEM_FAIL,
    /**
     * Other error codes can be added here
     */
    //..//
    /**
     * Last value
     */
    PUBSUBLIB_STATUSCODE_LASTVALUE = 0x7FFFFFFF    //ensures 32 bit int allocation
    
} PubSubLibStatusCode;

// Function pointer definition for a subsciber callback
// Eventually when this is called, it will print the topic and HTML body
typedef void (*SubscriberCallback)(std::string topic, std::string strHTML);

struct PubSubInitConfiguration {
    int maxPublishInQueue;
};
typedef PubSubInitConfiguration PubSubInitConfiguration;
#endif /* InterfaceUtil_h */
