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
            Stopwatch sw = Stopwatch.StartNew();
            Console.WriteLine("Filling list...");
            List<Message> messages = new List<Message>(10000000);
            for (int i = 0; i < 10000000; i++)
            {
                messages.Add(new Message { Title = "Hello", Body = "world!", DeviceId = i.ToString()});
            }
            sw.Stop();
            Console.WriteLine($"Filling took {sw.ElapsedMilliseconds} ms");
            sw.Reset();
            var thread1 = new Thread(() =>
            {
                Stopwatch sw1 = Stopwatch.StartNew();
                KafkaProducer producer = new KafkaProducer("10.1.76.251:9092", "test");
                Console.WriteLine("Sending messages in thread1...");
                for (int i = 0; i < 10000000; i++)
                {
                    producer.Send(messages[i]);

                    if(i % 50000 == 0)
                    {
                        Console.WriteLine($"Send {i} messages on thread 1");
                    }
                }
                while (!producer.IsQueueEmpty())
                {
                    Thread.Sleep(50);
                }
                sw1.Stop();
                Console.WriteLine($"Sending 10 million messages took {sw1.ElapsedMilliseconds} ms for thread 1");
            });
            var thread2 = new Thread(() =>  
            { 
                Stopwatch sw1 = Stopwatch.StartNew();
                KafkaProducer producer = new KafkaProducer("10.1.76.251:9092", "test2");
                Console.WriteLine("Sending messages in thread2...");
                for (int i = 0; i < 10000000; i++)
                {
                    producer.Send(messages[i]);

                    if(i % 50000 == 0)
                    {
                        Console.WriteLine($"Send {i} messages on thread 2");
                    }
                }
                while (!producer.IsQueueEmpty())
                {
                    Thread.Sleep(50);
                }
                sw1.Stop();
                Console.WriteLine($"Sending 10 million messages took {sw1.ElapsedMilliseconds} ms for thread 2");
            });

            thread1.Start();
            thread2.Start();

            thread1.Join();
            thread2.Join();

            Console.ReadKey();
        }
    }
}


