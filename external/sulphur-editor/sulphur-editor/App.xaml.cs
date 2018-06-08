using System.Windows;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using System.Windows.Interop;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.App : Application
     *@bief global application class
     *@author Stan Pepels   
     */
    public partial class App : Application
    {
      private MessageHandler handler_ = new MessageHandler(); //<! message handler for handling the network connection with the engine
      private MainWindow main_; //<! the main waindow of the editor
      private SubscriptionHandler subsciption_handler_ = new SubscriptionHandler(); //<! subscription handler to mannage subscriptions between application systems
      public static string app_directory;
      /**
       *@brief handle starup event 
       *@param[in] sender (object) object that invoked the command
       *@param[in] e (StartupEventArgs) event arguments passed when the event was invoked
       *@remark this is the entry point of the application 
       */
      private void Application_Startup(object sender, StartupEventArgs e)
      {
        app_directory = System.Reflection.Assembly.GetExecutingAssembly().Location;
        app_directory = app_directory.Remove(app_directory.LastIndexOf("\\"));
        native.AssetProcessor.Initialize();
        native.Networking.SNetInitEditor();

        if (e.Args.Length == 0)
        {
          controls.ProjectWindow window = new controls.ProjectWindow();
          window.on_project_ok += HandleProjectOk;
          window.Show();
        }
        else
        {
          Project.Load(e.Args[0]);
        }
      }

      private void HandleProjectOk(object sender, EventArgs e)
      {
        main_ = new MainWindow();
        MainWindow = main_;
        ((Window)sender).Close();

        InitializeComponent();
        main_.Initialize();

        SubscriptionHandler.Register(main_);
        SubscriptionHandler.Register(handler_);
        SubscriptionHandler.Register(Resources["asset_database"] as AssetDatabase);
        SubscriptionHandler.Register(Resources["workspace"] as Workspace);
        SubscriptionHandler.Register(Resources["world_hierarchy"] as WorldHierarchy);

        handler_.Initialize();
        handler_.Start();

        main_.Show();
        main_.Focus();
        main_.AddWndProc();
      }

      /**
      *@brief handle application exit event
      *@param[in] sender (object) original object that invoked the command
      *@param[in] e (ExitEventArgs) arguments passed when the command was invoked.  
      */
      private void Application_Exit(object sender, ExitEventArgs e)
      {
        main_?.Close();
        handler_?.Terminate();

        native.AssetProcessor.Shutdown();
        native.Networking.SNetDestroy();
      }
    }
  }
}
