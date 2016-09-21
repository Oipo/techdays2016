using System.IO;

namespace common
{
    public class Message
    {
        public string Title { get; set; }
        public string Body { get; set; }
        public string DeviceId { get; set; }

        public byte[] Serialize()
        {
            using (var stream = new MemoryStream())
            using (var writer = new BinaryWriter(stream))
            {
                writer.Write(Title);
                writer.Write(Body);
                writer.Write(DeviceId);
                return stream.ToArray();
            }
        }

        public static Message Deserialize(byte[] data)
        {
            Message msg = new Message();
            using (var stream = new MemoryStream(data))
            using (var reader = new BinaryReader(stream))
            {
                msg.Title = reader.ReadString();
                msg.Body = reader.ReadString();
                msg.DeviceId = reader.ReadString();
            }

            return msg;
        }
    }
}