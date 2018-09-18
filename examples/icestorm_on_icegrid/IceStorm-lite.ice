// -*- mode: c++; coding: utf-8 -*-

#pragma once

module IceStorm {

    interface Topic;
    dictionary<string, string> QoS;

    exception AlreadySubscribed {};
    exception InvalidSubscriber {
	string reason;
    };

    exception BadQoS {
	string reason;
    };

    interface Topic {
	["nonmutating", "cpp:const"] idempotent string getName();
	["nonmutating", "cpp:const"] idempotent Object* getPublisher();

	Object* subscribeAndGetPublisher(QoS theQoS, Object* subscriber)
	    throws AlreadySubscribed, InvalidSubscriber, BadQoS;

	idempotent void unsubscribe(Object* subscriber);
	void destroy();
    };

    exception TopicExists {
	string name;
    };

    exception NoSuchTopic {
	string name;
    };

    interface TopicManager {
	Topic* create(string name) throws TopicExists;
	["nonmutating", "cpp:const"] idempotent Topic* retrieve(string name)
	    throws NoSuchTopic;
    };

};
