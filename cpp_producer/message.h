class Message {
public:
    std::string Title;
    std::string Body;
    std::string DeviceId;

    Message() {

    }

    Message(std::string title, std::string body, std::string deviceId) {
        Title = title;
        Body = body;
        DeviceId = deviceId;
    }

    // Bad code. Don't do this, doesn't work with spaces in any of the variables nor is portable.
    void Serialize(std::ostream &out) {
        out << Title;
        out << " ";
        out << Body;
        out << " ";
        out << DeviceId;
    }

    // Bad code. Don't do this, doesn't work with spaces in any of the variables nor is portable.
    void Deserialize(std::istream &in) {
        in >> Title;
        in >> Body;
        in >> DeviceId;
    }
};
