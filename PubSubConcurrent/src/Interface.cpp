//
//  Interface.cpp
//  PubSubConcurrent
//
//  Created by Varun Varshney on 31/05/22.
//

#include <stdio.h>
#include <iostream>
#include "Interface.h"
#include "../include/PubSubManager.h"

using namespace std;

static bool gManagerInit = false;
pubsub::manager::PubSubManager* gManager = NULL;


PubSubLibStatusCode PubSubInit(PubSubInitConfiguration inInitConfiguration)
{
    PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;

    // Initialize pubsub manager if not initialized
    if (gManagerInit == false) {
        gManager = new pubsub::manager::PubSubManager(inInitConfiguration);
        if (gManager == NULL) {
            bStatusCode = PUBSUBLIB_STATUSCODE_MEM_FAIL;
            cout << "ERROR | Memory allocation failed" << endl;
        } // if (gManager == NULL) {
    } // if (gManagerInit == false)
    
    return bStatusCode;
}

PubSubLibStatusCode PublishTopic(std::string inTopic, std::string inTopicPageData)
{
    PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;

    if (gManager) {
        bStatusCode = gManager->HandlePublishTopic(inTopic, inTopicPageData);
    }

    return bStatusCode;
}

PubSubLibStatusCode Subscribe(std::string inTopic,
                              std::string inSubscriberName,
                              SubscriberCallback inCallback)
{
    PubSubLibStatusCode bStatusCode = PUBSUBLIB_STATUSCODE_SUCCESS;
    
    if (gManager) {
        bStatusCode = gManager->HandleSubscription(inTopic, inSubscriberName, inCallback);
    }

    return bStatusCode;
}

void PubSubTerminate()
{
    // delete pubsub manager
    if (gManager) {
        delete gManager;
        gManager = NULL;
    }
}
