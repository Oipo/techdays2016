using System;
using System.Collections.Generic;
using RdKafka;
using common;

namespace consumer
{
    public class KafkaConsumer : IDisposable
    {
        private bool _disposed { get; set; }
        private EventConsumer _consumer { get; set; }
        private int _i { get; set; }
        private string _topicName { get; set; }

        public KafkaConsumer(string brokerlist, string topic)
        {
            var config = new Config { GroupId = "group" };
            _consumer = new EventConsumer(config, brokerlist);
            _topicName = topic;
            _i = 0;

            _consumer.OnMessage += (sender, message) =>
            {
                if(message.Payload.Length <= 0)
                {
                    return;
                }

                _i++;

                if(_i % 5000 == 0)
                {
                    var msg = common.Message.Deserialize(message.Payload);
                    Console.WriteLine($"Topic {_topicName} received msg: {msg.Title} - {msg.Body} - {msg.DeviceId}");
                }
            };

            //_consumer.Subscribe(new List<string>{topic});
            _consumer.Assign(new List<TopicPartitionOffset>{new TopicPartitionOffset(topic, 0, Offset.End)});
            _consumer.Start();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
            {
                return;
            }

            if (disposing)
            {
                if (_consumer != null)
                {
                    _consumer.Dispose();
                    _consumer = null;
                }
            }

            _disposed = true;
        }
    }
}
