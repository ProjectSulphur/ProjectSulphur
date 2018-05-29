using System.Windows;

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
       
      }

      public void Initialize()
      {
        InitializeComponent();
        SubscriptionHandler.Register(menu_bar);
        SubscriptionHandler.Register(dynamic_dock);
        SubscriptionHandler.Register(engine);
        SubscriptionHandler.Register(asset_browser);
        SubscriptionHandler.Register(hierarchy_viewer);
      }
    }
  }
}
