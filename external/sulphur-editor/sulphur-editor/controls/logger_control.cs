using System;
using System.Collections.ObjectModel;
using System.Windows.Controls;
using System.Diagnostics;
using System.Windows.Media;


namespace sulphur
{
  namespace editor
  {
    public class LogList : ObservableCollection<ListBoxItem>
    {
      /**
       *@brief constructor 
       */
      public LogList() : base() { }


    }

    /**
     * @class sulphur.editor.Log 
     * @brief Interface for logging messages in the editor.
     * @author Stan Pepels.
     */
    public static class Log
    {
      /**
       * @brief Verbosity types
       */
      public enum Verbosity
      {
        kFatal, //!< An unrecoverable event has occured. These logs are displayed in red.
        kError, //!< An error has occured in the editor.These logs are displayed in red.
        kInfo, //!< Verbosity used to give info about a certain event. 
        kWarning //!< Something went wrong in the editor but the editor can continue. These logs are displayed in yellow.
      }

      /**
       * @brief Adds a log entry to the sulphur.editor.controls.Logger
       * @param[in] verbosity (sulphur.editor.Log.Verbosity) The way the log should be formatted.
       * @param[in] message (string) Message to be displayed in the logger.
       */
      public static void Write(Verbosity verbosity, string message)
      {
        App.Current.Dispatcher.Invoke((Action)delegate
        {
          TextBlock log = new TextBlock();
          log.Text = message;
          switch (verbosity)
          {
            case Verbosity.kWarning:
              log.Foreground = new SolidColorBrush(Color.FromRgb(245, 211, 75));
              break;
            case Verbosity.kFatal:
            case Verbosity.kError:
              log.Foreground = new SolidColorBrush(Colors.Red);
              break;
          }

          ListBoxItem log_entry = new ListBoxItem();
          log_entry.Content = log;
          controls.Logger.logs_.Add(log_entry);
        });
      }
    }

    /**
       * @class sulphur.editor.EngineOutputWriter
       * @brief Captures standard output from the attached engine process and formats it into a list.
       * @author Stan Pepels
       */
    public static class EngineOutputWriter
    {
      public static LogList logs_ { get; private set; } = new LogList(); //!< The list of logs captured from the engine. Each entry in here coresponds to a call to std::cout or printf()

      public static DataReceivedEventHandler engine_output_func = WriteEngineOutput; //!< Function used for processing the engine process output. 

      /**
       * @brief processes the engine output and adds it to the log list.
       * @param[in] sender (object) The engine process the output is captured from.
       * @param[in] e (DataReceivedEventArgs) event arguments containing the output captured from the engine.
       */
      private static void WriteEngineOutput(object sender, DataReceivedEventArgs e)
      {
        App.Current.Dispatcher.Invoke((Action)delegate
        {
          TextBlock log = new TextBlock();
          log.Text = e.Data;

          ListBoxItem log_entry = new ListBoxItem();
          log_entry.Content = log;
          logs_.Add(log_entry);
        });

      }
    }

    namespace controls
    {
      
      /**
       * @class sulphur.editor.controls.Logger : TabControl
       * @brief Control that displays the logs written by the editor and captured from the engine.
       * @author Stan Pepels
       */
      public class Logger : TabControl
      {
        

        TabItem editor_tab; //!< Tab that displays the editor logs.
        TabItem engine_tab; //!< Tab that displays the engine logs.
        public static LogList logs_ { get; private set; } = new LogList(); //!< List of logs to be displayed.

        /**
         * @brief Constructor that sets up the different tabs used to display the editor and engine logs.
         */
        public Logger()
        {
          editor_tab = new TabItem();
          engine_tab = new TabItem();

          // editor output tab
          ListBox listbox = new ListBox();
          listbox.ItemsSource = logs_;
          editor_tab.Header = "editor";
          editor_tab.Content = listbox;

          // engine output tab
          listbox = new ListBox();
          listbox.ItemsSource = EngineOutputWriter.logs_;
          engine_tab.Header = "engine";
          engine_tab.Content = listbox;

          AddChild(editor_tab);
          AddChild(engine_tab);

        }
      }
    }
  }
}
