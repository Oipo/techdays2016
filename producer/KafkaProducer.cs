using System;
using RdKafka;
using common;

namespace producer
{
    public class KafkaProducer : IDisposable
    {
        private bool _disposed { get; set; }
        private Producer _producer { get; set; }
        private Topic _topic { get; set; }

        public KafkaProducer(string brokerList, string topic)
        {
            _producer = new Producer(brokerList);
            _topic = _producer.Topic(topic);
        }

        public void Send(common.Message msg)
        {
            _topic.Produce(msg.Serialize());
        }

        public bool IsQueueEmpty()
        {
            return _producer.OutQueueLength == 0;
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
                if (_topic != null)
                {
                    _topic.Dispose();
                    _topic = null;
                }

                if (_producer != null)
                {
                    _producer.Dispose();
                    _producer = null;
                }
            }

            _disposed = true;
        }
    }
}
