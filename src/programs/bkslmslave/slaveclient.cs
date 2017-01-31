using cmn;
using Google.ProtocolBuffers;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Net.Sockets;
using System.Reflection;

class SlaveClient
{
    Object commands;
    Stream stream;
    BinaryReader reader;
    BinaryWriter writer;

    public SlaveClient(Object commands, string serverName)
    {
        this.commands = commands;
#if __MonoCS__
        Socket socket = new Socket(AddressFamily.Unix, SocketType.Stream, ProtocolType.IP);
        socket.Connect(new Mono.Unix.UnixEndPoint("/tmp/" + serverName));
        this.stream = new NetworkStream(socket, true);
#else
        NamedPipeClientStream socket = new NamedPipeClientStream(serverName);
        socket.Connect();
        this.stream = socket;
#endif
        this.reader = new BinaryReader(this.stream);
        this.writer = new BinaryWriter(this.stream);
    }

    ~SlaveClient()
    {
        writer.Dispose();
        reader.Dispose();
        stream.Dispose();
    }

    public void serve()
    {
        while (true) {
            uint size = reader.ReadUInt32();

            ProtoSlaveRequest request = ProtoSlaveRequest.ParseFrom(reader.ReadBytes((int) size));
            MethodInfo methodInfo = this.commands.GetType().GetMethod(request.Function.ToStringUtf8());
            object[] parameters = new List<ByteString>(request.ParameterList)
                .ConvertAll(x => x.ToStringUtf8()).ToArray();
            Console.WriteLine(request.Function.ToStringUtf8());

            string result = null, error = null;
            try {
                result = (string) methodInfo.Invoke(this.commands, parameters);
            } catch (Exception e) {
                error = e.GetBaseException().Message;
            }
            reply(result, error);
        }
    }

    private void reply(string result, string error)
    {
        ProtoSlaveReply.Builder builder = new ProtoSlaveReply.Builder();
        if (result != null)
            builder.SetResult(ByteString.CopyFromUtf8(result));
        if (error != null)
            builder.SetError(ByteString.CopyFromUtf8(error));
        ProtoSlaveReply reply = builder.Build();

        writer.Write((uint)reply.SerializedSize);
        writer.Flush();
        reply.WriteTo(writer.BaseStream);
    }
}
