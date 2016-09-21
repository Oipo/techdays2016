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
            KafkaConsumer consumer = new KafkaConsumer("localhost:9092");
            Console.WriteLine("Started consumer. Press enter to stop consuming.");

            Console.ReadLine();
        }
    }
}


