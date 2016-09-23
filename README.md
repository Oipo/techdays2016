# techdays2016
Demo code for An Intro Into Scalable, High-Performance Messaging in .NET

# C#
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

# License

MIT License. See LICENSE file for full text.
