# PubSubConcurrent
A thread safe CPP library for providing Publish/Subscribe interface

 * Interface to use PubSub lib by any client.
 * PubSub - Is a multithreaded publisher-subscriber library which manages thread management on its own
 * Supports multiple concurrent calls as pulishers/subscribers with multiple topics
 * A bound on request submission is configurable
 *
 * Current internals:
 * 1. Uses 1 background thread for Pub/Sub job execution
 * 2. Uses maximum number of threads as per hardware concurrency for distributing subsciber callbacks
 * 3. For simulation in main function, I have simulated using two subscriber/publisher threads. This cab be tested with more threads

## Required Packages

To install this project, please ensure that you have installed the following (install guides are provided on the respective websites):

- [Xcode](https://developer.apple.com/xcode/)
- Setup GitHub on your local machine

## Installation

XCode : Use the xcode proj named PubSubConcurrent.xcodeproj


## Major files

1. Interface.h  : Header file to be used by clients. Hosts the major APIs (Init, Terminate, Publish, Subscribe) 
2. PubSubManager.h : Main manager - This is a thread safe  class is responsible for Blocking queue management which takes in Pub/Sub requests . This class also maintains its on async executor which gets signalled in case of new job submissions
3. Topic.h : Encapsulated class containing basic operations on a Topic (basically addition of more HTML pages)
4. PubSubUtil.h : Util file containing basic structs for overall usage

## Contributors

This project is made with ❤️ by Varun Varshney (varun.varshney.cse@gmail.com)
