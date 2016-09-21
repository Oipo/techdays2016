using tech_days_2016;
using Xunit;
using FluentAssertions;

namespace unit_tests
{
    public class Class1
    {
        [Fact]
        public void SerializationTest()
        {
            Message msg = new Message {Body = "body", DeviceId = "deviceid", Title = "title"};
            var newMsg = Message.Deserialize(msg.Serialize());
            newMsg.Title.Should().Be(msg.Title);
            newMsg.Body.Should().Be(msg.Body);
            newMsg.DeviceId.Should().Be(msg.DeviceId);
        }
    }
}


