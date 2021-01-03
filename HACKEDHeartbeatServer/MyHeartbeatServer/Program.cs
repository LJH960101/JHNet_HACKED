using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using static System.Console;
using System.Diagnostics;
using System.IO;

namespace HeartbeatServer
{
    class Program
    {
        static string path = "info.txt";
        static string serverPath = "";
        static string IP = "127.0.0.1";
        static int TCP_SERVER_PORT = 15470;
        static int UDP_SERVER_PORT = 15471;
        static int HEARTBEAT_MESSAGE_TYPE = 100;
        static Process process = null;
        static void Main(string[] args)
        {
            //Open the stream and read it back.
            using (FileStream fs = File.OpenRead(path))
            {
                // Get File String
                byte[] b = new byte[1024];
                UTF8Encoding temp = new UTF8Encoding(true);
                while (fs.Read(b, 0, b.Length) > 0) { }

                // Parse File
                string fileData = Encoding.Default.GetString(b);
                fileData = fileData.Replace(" ", string.Empty);
                fileData = fileData.Replace("\r", string.Empty);
                string[] datas = fileData.Split('\n');

                // 파싱된 값으로 변수를 수정
                for(int i=0; i<5; ++i)
                {
                    string[] inDatas = datas[i].Split('=');
                    switch(inDatas[0])
                    {
                        case "ServerPath":
                            serverPath = inDatas[1];
                            break;
                        case "IP":
                            IP = inDatas[1];
                            break;
                        case "TCP_SERVER_PORT":
                            TCP_SERVER_PORT = int.Parse(inDatas[1]);
                            break;
                        case "UDP_SERVER_PORT":
                            UDP_SERVER_PORT = int.Parse(inDatas[1]);
                            break;
                        case "HEARTBEAT_MESSAGE_TYPE":
                            HEARTBEAT_MESSAGE_TYPE = int.Parse(inDatas[1]);
                            break;
                        default:
                            throw new Exception("잘못된 info.txt 형식 입니다.");
                    }
                }
            }

            // 하트비트 무한 반복!
            int counter = 1;
            const int TRY_DELAY_MILLSECS = 500; // 다시 보내는 시간
            const int TCP_SEND_RATE = 20; // TCP는 자주 보내지 않기 위해 N번에 한번씩만 요청한다.
            while (true)
            {
                try
                {
                    ++counter;
                    if (counter % TCP_SEND_RATE == 0)
                    {
                        counter = 1;
                        if (!CheckTCP())
                        {
                            WriteLine("TCP 실패!!");
                            throw new Exception("CheckTCP Failed");
                        }
                    }
                    if (!CheckUDP())
                    {
                        WriteLine("UDP 실패!!");
                        throw new Exception("CheckUDP Failed");
                    }
                }
                catch (Exception ex)
                {
                    WriteLine("서버를 재실행합니다. : 이유 : " + ex.Message);
                    if (process != null && !process.HasExited) process.Kill();
                    process = Process.Start(serverPath);
                    Thread.Sleep(TRY_DELAY_MILLSECS);
                    continue;
                }
                Thread.Sleep(TRY_DELAY_MILLSECS);
            }
        }
        static bool CheckTCP()
        {
            var client = new TcpClient();
            var result = client.BeginConnect(IP, TCP_SERVER_PORT, null, null);
            
            // 1초만 기다린다.
            var success = result.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(1));
            if (!success)
            {
                return false;
            }
            // we have connected
            client.EndConnect(result);
            client.Close();
            return true;
        }
        static bool CheckUDP()
        {
            // UDP Check
            UdpClient cli = new UdpClient();

            // (2) 데이타 송신
            byte[] heartbeatData = new byte[sizeof(Int32) * 2];
            int bufSize = sizeof(Int32);
            int intValue = HEARTBEAT_MESSAGE_TYPE;
            heartbeatData[0] = (byte)(bufSize >> 24);
            heartbeatData[1] = (byte)(bufSize >> 16);
            heartbeatData[2] = (byte)(bufSize >> 8);
            heartbeatData[3] = (byte)bufSize;
            heartbeatData[4] = (byte)(intValue >> 24);
            heartbeatData[5] = (byte)(intValue >> 16);
            heartbeatData[6] = (byte)(intValue >> 8);
            heartbeatData[7] = (byte)intValue;

            cli.Send(heartbeatData, heartbeatData.Length, IP, UDP_SERVER_PORT);

            // (3) 데이타 수신
            IPEndPoint epRemote = new IPEndPoint(IPAddress.Any, 0);

            var timeToWait = TimeSpan.FromSeconds(1);
            var asyncResult = cli.BeginReceive(null, null);
            asyncResult.AsyncWaitHandle.WaitOne(timeToWait);
            if (asyncResult.IsCompleted)
            {
                try
                {
                    IPEndPoint remoteEP = null;
                    byte[] receivedData = cli.EndReceive(asyncResult, ref remoteEP);
                    if (receivedData.Length == 8)
                    {
                        cli.Close();
                        return true;
                    }
                    else
                    {
                        cli.Close();
                        return false;
                    }
                }
                catch (Exception ex)
                {
                    cli.Close();
                    return false;
                }
            }
            cli.Close();
            return false;
        }
    }
}
