/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2014, Magnus Edenhill (librdkafka example I gratuitously stole from), Michael de Lang (techdays 2016 example)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <sys/time.h>
#include <ctime>

#include <rdkafkacpp.h>

#include "message.h"

uint64_t get_time() {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    uint64_t ret = tv.tv_usec;
    /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
    ret /= 1000;

    /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
    ret += (tv.tv_sec * 1000);

    return ret;
}

bool run = true;

class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {
    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
          run = false;
        break;

      case RdKafka::Event::EVENT_STATS:
        std::cerr << "\"STATS\": " << event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};

/* Use of this partitioner is pretty pointless since no key is provided
 * in the produce() call. */
class MyHashPartitionerCb : public RdKafka::PartitionerCb {
 public:
  int32_t partitioner_cb (const RdKafka::Topic *topic, const std::string *key,
                          int32_t partition_cnt, void *msg_opaque) {
    return djb_hash(key->c_str(), key->size()) % partition_cnt;
  }
 private:

  static inline unsigned int djb_hash (const char *str, size_t len) {
    unsigned int hash = 5381;
    for (size_t i = 0 ; i < len ; i++)
      hash = ((hash << 5) + hash) + str[i];
    return hash;
  }
};

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb (RdKafka::Message &message) {
      if(message.err() != RdKafka::ERR_NO_ERROR) {
          std::cout << "Message delivery for (" << message.len() << " bytes): " << message.errstr() << std::endl;
      }
  }
};

int main() {
    std::string brokers = "localhost:9092";
    std::string errstr;
    std::string topic_str = "test";
    std::string mode;
    std::string debug;
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    MyHashPartitionerCb hash_partitioner;
    ExampleDeliveryReportCb ex_dr_cb;

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    ExampleEventCb ex_event_cb;

    //Which brokers to connect to
    conf->set("metadata.broker.list", brokers, errstr);

    //Set callback upon getting events such as brokers going down or statistics
    conf->set("event_cb", &ex_event_cb, errstr);

    // Delivery report callback
    conf->set("dr_cb", &ex_dr_cb, errstr);

    // partitioner_cb ?
    if (tconf->set("partitioner_cb", &hash_partitioner, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if(!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;

    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str,
        tconf, errstr);
    if (!topic) {
          std::cerr << "Failed to create topic: " << errstr << std::endl;
          exit(1);
    }

    // Make and fill a vector of messages to send
    std::vector<Message*> messages;
    messages.resize(10000000);

    std::cout << "% Filling list" << std::endl;
    for(int i = 0; i < 10000000; i++) {
        messages[i] = new Message("Title", "Body", std::to_string(i));
    }
    std::cout << "% Done filling list" << std::endl;

    std::cout << "% Sending messages" << std::endl;
    uint64_t stopwatch = get_time();
    for(int i = 0; i < 10000000 && run; i++) {
        std::stringstream stream;
        messages[i]->Serialize(stream);
        const std::string& tmp = stream.str();
        const char* cstr = tmp.c_str();
        RdKafka::ErrorCode resp =
            producer->produce(topic, partition,
        			  RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
        			  (char*)cstr, tmp.size(),
                      NULL, NULL);

        if (resp != RdKafka::ERR_NO_ERROR && resp != RdKafka::ERR__QUEUE_FULL) {
            std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
            run = false;
        }

        if(i % 50000 == 0) {
            std::cout << "Send " << i << " messages" << std::endl;
        }

        producer->poll(0);
    }

    while (run && producer->outq_len() > 0) {
          producer->poll(100);
    }

    uint64_t total_time = get_time() - stopwatch;
    std::cout << "% Done sending messages in " << total_time << " ms" << std::endl;

    for(int i = 0; i < 10000000; i++) {
        delete messages[i];
    }

    delete topic;
    delete producer;

    return 0;
}
