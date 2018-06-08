using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace sulphur.editor.controls
{ 
    /// <summary>
    /// Interaction logic for network_debugger_control.xaml
    /// </summary>
    public partial class NetworkDebugger : UserControl, ISubscribeable
    {

        public NetworkDebugger()
        {
            InitializeComponent();

            setup_state.Content = "...";
            connection_state.Content = "...";
            client_name.Content = "...";
            client_id.Content = "...";
            tickrate.Content = "...";
            no_clients.Content = "...";

            for (int i = 0; i < 40; ++i)
            {
                AddClient("name", 0);
            }
            for (int i = 0; i < 40; ++i)
            {
                AddValue(i, "type", "value");
            }
            Random r = new Random();
            for (int i = 0; i < 40; ++i)
            {
                List<string> args = new List<string>();
                int count = r.Next(1, 6);
                for (int j = 0; j < count; ++j)
                {
                    args.Add("Type" + j);
                }
                AddRPC(i, args);
            }

            for (int i = 0; i < 40; ++i)
            {
                List<string> args = new List<string>();
                int count = r.Next(1, 6);
                for (int j = 0; j < count; ++j)
                {
                    args.Add("Value" + j);
                }
                AddRPCBufferEntry(i, args);
            }

            packets_sent_pt.Content = "...";
            packets_sent_ttl.Content = "...";
            packets_sent_avg.Content = "...";

            messages_sent_pt.Content = "...";
            messages_sent_ttl.Content = "...";
            messages_sent_avg.Content = "...";

            packets_recieved_pt.Content = "...";
            packets_recieved_ttl.Content = "...";
            packets_recieved_avg.Content = "...";

            messages_recieved_pt.Content = "...";
            messages_recieved_ttl.Content = "...";
            messages_recieved_avg.Content = "...";

            packets_lost_pt.Content = "...";
            packets_lost_ttl.Content = "...";
            packets_lost_avg.Content = "...";

            outgoing_bytes_pt.Content = "...";
            outgoing_bytes_ttl.Content = "...";
            outgoing_bytes_avg.Content = "...";

            incoming_bytes_pt.Content = "...";
            incoming_bytes_ttl.Content = "...";
            incoming_bytes_avg.Content = "...";

            rpc_buffer_size.Content= "...";
        }

        void AddClient(string name, int id)
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            Label l1 = new Label();
            l1.Width = 145;
            Label l2 = new Label();
            l2.Width = 150;
            l2.Content = name;
            Label l3 = new Label();
            l3.Width = 150;
            l3.Content = id;
            panel.Children.Add(l1);
            panel.Children.Add(l2);
            panel.Children.Add(l3);
            player_list.Items.Add(panel);
        }

        void AddValue(int id, string type, string value)
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            Label l1 = new Label();
            l1.Width = 145;
            l1.Content = id;
            Label l2 = new Label();
            l2.Width = 150;
            l2.Content = type;
            Label l3 = new Label();
            l3.Width = 150;
            l3.Content = value;
            panel.Children.Add(l1);
            panel.Children.Add(l2);
            panel.Children.Add(l3);
            value_list.Items.Add(panel);
        }

        void AddRPC(int id, List<string> args)
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            Label l1 = new Label();
            l1.Width = 145;
            l1.Content = id;
            panel.Children.Add(l1);

            for (int i = 0; i < args.Count; ++i)
            {
                Label l2 = new Label();
                l2.Width = 100;
                l2.Content = args[i];
                panel.Children.Add(l2);

            }
            rpc_list.Items.Add(panel);
        }

        void AddRPCBufferEntry(int id, List<string> args)
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            Label l1 = new Label();
            l1.Width = 145;
            l1.Content = id;
            panel.Children.Add(l1);

            for (int i = 0; i < args.Count; ++i)
            {
                Label l2 = new Label();
                l2.Width = 100;
                l2.Content = args[i];
                panel.Children.Add(l2);
            }
            rpc_buffer_list.Items.Add(panel);
        }

        event OnNotification ISubscribeable.notify_event
        {
            add
            {
                notify_subscribers_ += value;
            }
            remove
            {
                notify_subscribers_ -= value;
            }
        }//<! event fired for notifying subscribed system that something important happend

        /**
         *@brief actual event handler used to fire the notification event from the sulphur.editor.ISubscribeable interface
         */
        private event OnNotification notify_subscribers_;

        Subscription[] ISubscribeable.GetSubscriptions()
        {
            return null;
        }
    }
}
