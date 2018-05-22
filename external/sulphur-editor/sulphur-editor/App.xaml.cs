using System.Windows;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

namespace sulphur
{
  namespace editor
  {
    public partial class App : Application
    {
      private MessageHandler handler_ = new MessageHandler();
      private MainWindow main_;
      private SubscriptionHandler subsciption_handler_ = new SubscriptionHandler();

      private void Application_Startup(object sender, StartupEventArgs e)
      {
        if (Directory.Exists(".\\resources") == false)
        {
          Directory.CreateDirectory(".\\resources");
        }

        if (Directory.Exists(".\\assets") == false)
        {
          Directory.CreateDirectory(".\\assets");
        }

        InitializeComponent();
        main_ = new MainWindow();

        native.Networking.SNetInitEditor();
        SubscriptionHandler.Register(handler_);

        native.AssetProcessor.Initialize();
        native.AssetProcessor.SetPackageOutputPath("./resources");
        SubscriptionHandler.Register(Resources["asset_database"] as AssetDatabase);
        SubscriptionHandler.Register(Resources["workspace"] as Workspace);
        handler_.Initialize();
        handler_.Start();
        main_.Show();
        main_.Focus();
      }

      private void Application_Exit(object sender, ExitEventArgs e)
      {
        native.AssetProcessor.Shutdown();
        handler_.Terminate();
      }
    }
  }
}
