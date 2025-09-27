using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net;
using System.Net.Sockets;
using System.Windows.Threading;
using System.Xml;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Timers;

namespace TonexControllerLocator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static string serverip = "0";
        public static int LOCATOR_PORT = 12106;
        private static ObservableCollection<DataObject> server_list;
        private System.Timers.Timer aTimer;
        public const int LOCATOR_TIMEOUT = 22;      // seconds

        public MainWindow()
        {
            InitializeComponent();

            
            UdpClient udpClient = new UdpClient();

            // allow re-use of the socket
            udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);

            udpClient.Client.Bind(new IPEndPoint(IPAddress.Any, LOCATOR_PORT));
            var from = new IPEndPoint(0, 0);

            // create list for the servers
            server_list = new ObservableCollection<DataObject>();

            // Create a timer
            aTimer = new System.Timers.Timer();
            aTimer.Interval = 1000;

            // Hook up the Elapsed event for the timer. 
            aTimer.Elapsed += OnTimedEvent;

            // Have the timer fire repeated events (true is the default)
            aTimer.AutoReset = true;

            // Start the timer
            aTimer.Enabled = true;

            // bind list to grid
            this.serversGrid.ItemsSource = server_list;

            Task.Run(() =>
            {
                while (true)
                {
                    // wait for UDP RX
                    var recvBuffer = udpClient.Receive(ref from);
                    var locatorpacket = Encoding.UTF8.GetString(recvBuffer);
                    string serverIP = "";

                    XmlDocument xd = new XmlDocument();
                    xd.LoadXml(locatorpacket);

                    // is it in LIB packet?
                    if (xd.DocumentElement.Name == "locater")
                    {
                        // loop all XML elements
                        foreach (XmlNode xmlNode in xd.DocumentElement)
                        {
                            // check if its a server IP announcement
                            if (xmlNode.Name == "server")
                            {
                                // get the server IP
                                foreach (XmlNode xmlChildNode in xmlNode)
                                {
                                    if (xmlChildNode.Name == "ip")
                                    {
                                        serverIP = xmlChildNode.InnerText;
                                    }
                                }

                                // did we find a server?
                                if (serverIP != "")
                                {
                                    DateTime localDate = DateTime.Now;

                                    // inform UI
                                    Dispatcher.Invoke(DispatcherPriority.Background, new Action(() => FoundServer(serverIP, localDate)));
                                }
                            }
                        }
                    }
                }
            }
           );
        }

        public class DataObject
        {
            public string ServerIP { get; set; }
            public string URL { get; set; }
            public DateTime LastSeen { get; set; }
        }

        public void FoundServer(string IP, DateTime localDate)
        {
            // do we already have this combo?
            if (!server_list.Any(p => p.ServerIP == IP))
            {
                // add to list
                server_list.Add(new DataObject() { ServerIP = IP, URL = "http://" + IP, LastSeen = localDate });
            }
            else
            {
                // just update the time stamp on existing item
                for (int i = 0; i < MainWindow.server_list.Count; i++)
                {
                    if (MainWindow.server_list[i].ServerIP == IP)
                    {
                        // found it
                        MainWindow.server_list[i].LastSeen = localDate;
                    }
                }
            }
        }

        private void DG_Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            Hyperlink link = (Hyperlink)e.OriginalSource;
            Process.Start(link.NavigateUri.AbsoluteUri);
        }

        private void OnTimedEvent(Object source, System.Timers.ElapsedEventArgs e)
        {
            try
            {
                Dispatcher.Invoke(new Action(() =>
                {
                    // get current time
                    DateTime localDate = DateTime.Now;

                    // loop all entries
                    for (int i = 0; i < MainWindow.server_list.Count; i++)
                    {
                        // get time since last update
                        TimeSpan timeDifference = localDate.Subtract(MainWindow.server_list[i].LastSeen);

                        if (timeDifference.Seconds > LOCATOR_TIMEOUT)
                        {
                            // remove entry
                            MainWindow.server_list.RemoveAt(i);
                        }
                    }
                }));
            }

            catch
            {

            }
        }

    }
}
