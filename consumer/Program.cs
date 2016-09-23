using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace consumer
{
    public class Program
    {
        public static void Main(string[] args)
        {
            KafkaConsumer consumer = new KafkaConsumer("10.1.76.251:9092", "test");
            KafkaConsumer consumer2 = new KafkaConsumer("10.1.76.251:9092", "test2");
            Console.WriteLine("Started consumer. Press enter to stop consuming.");

            Console.ReadLine();
        }
    }
}


