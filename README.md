# techdays2016
Demo code for An Intro Into Scalable, High-Performance Messaging in .NET

# C&#35;
Needs dotnet core v1.0.

Compile with:
```
cd common
dotnet restore && dotnet build
cd ../unit_tests
dotnet restore && dotnet test
cd ../producer
dotnet restore && dotnet run
```

Client performance on a scaleway C2S (Avoton 2550, 8G RAM, networked SSD):
```
20 bytes payload (average)

10m strcopy 1 server 1 proc each 1 thread each; 80s
125k/sec or 19,1 mbit/sec

20m strcopy 1 server 1 proc each 2 thread each; 120s
166k/sec or 25,3 mbit/sec

180m strcopy 3 server 3 proc each 2 thread each; 190s
947k/sec or 144,5 mbit/sec
```

or if we increase the payload:
```
250 bytes payload (average)

10m strcopy 1 server 1 proc each 1 thread each 93s
108k/sec or 206,0 mbit/s

20m strcopy 1 server 1 proc each 2 thread each 138s
145k/sec or 276,6 mbit/s
```

# C++
Needs librdkafka.

Clone the specific commit in a separate dir and include (-I and -L for g++) librdkafka like so:
```
sudo apt install build-essential
cd tech_days_2016/cpp_producer
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
git checkout 67b0f8c1a53c891f3b5b64877b21711563e0505a
./configure && make #doesn't install! Just compiles
cd ..
g++ -std=c++11 test_serialization.cpp -o serialization_test #optional
g++ -std=c++11 main.cpp -I librdkafka/src-cpp -L librdkafka/src-cpp -L librdkafka/src -lrdkafka++ -lrdkafka -o cpp_producer
LD_LIBRARY_PATH="./librdkafka/src-cpp:./librdkafka/src" ./cpp_producer
```

The C++ program is linux specific because it's purely used to compare performance on a linux server and serves no purpose when using the rdkafka-dotnet package.

Client performance on a scaleway C2S (Avoton 2550, 8G RAM, networked SSD):
```
20 bytes payload (average)

10m strcopy 1 server 1 proc each 1 thread each; 49s
204k/sec or 31,1 mbit/sec

90m strcopy 3 server 3 proc each 1 thread each; 43s
2093k/sec or 319,4 mbit/sec
```

or if we increase the payload:
```
250 bytes payload (average)

10m strcopy 1 server 1 proc each 1 thread each 61s
164k/sec or 312,8 mbit/s
```
# Performance notes

These "performance tests" are probably done reasonably badly. The goal of these tests were to get a general grasp of performance on a cheap dedicated VPS-like server. Having said that, for small message and one threaded producers, the C# version is about 2/3rds slower and for multi-process runs about 3/4ths. As soon as we turn up the payload however, the difference gets smaller and would probably be insignificant once the network card is saturated.

It would definitely be interesting profiling the differences, but two things that are undoubtedly influencing the numbers is 1) marshalling and interop in general and 2) simply having more callbacks registered than the C++ version. This mainly gives the C# version a higher CPU usage than the C++ version, leading to the behaviour we see: significantly lower messages/s for small messages but similar messages/s for bigger payloads.

# License

MIT License. See LICENSE file for full text.
