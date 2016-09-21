using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using common;

namespace producer
{
    public class Program
    {
        public static void Main(string[] args)
        {
            KafkaProducer producer = new KafkaProducer("localhost:9092");
            Stopwatch sw = Stopwatch.StartNew();
            for (int i = 0; i < 10000000; i++)
            {
                producer.Send(new Message { Title = "Hello", Body = "world!", DeviceId = i.ToString()});
            }
            while (!producer.IsQueueEmpty())
            {
                Thread.Sleep(50);
            }
            sw.Stop();
            Console.WriteLine($"Sending 10 million messages took {sw.ElapsedMilliseconds} ms");
            Console.ReadKey();
        }
    }
}


