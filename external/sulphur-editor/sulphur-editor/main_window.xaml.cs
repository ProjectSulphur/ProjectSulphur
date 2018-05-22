using System;
using System.Windows;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace sulphur
{
  namespace editor
  {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
      public MainWindow()
      {
        InitializeComponent();
        SubscriptionHandler.Register(engine);
        SubscriptionHandler.Register(asset_browser);
      }
    }
  }
}
