using Microsoft.Glee.Drawing;
using Microsoft.Glee.GraphViewerGdi;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Parc
{
  public partial class GraphViewerUserControl : UserControl
  {
    public class UserControlController
    {
      private readonly GraphViewerUserControl _f;
      private readonly Dictionary<string, Graph> _dict = new Dictionary<string, Graph>();

      public UserControlController(GraphViewerUserControl f)
      {
        _f = f;
      }

      public void AddGraph(string key, Graph g)
      {
        if (_f.InvokeRequired)
        {
          _f.Invoke(new Action<string, Graph>(AddGraph), key, g);
        }
        else
        {
          _dict[key] = g;
          if (!_f.comboBox1.Items.Contains(key))
          {
            _f.comboBox1.Items.Add(key);
          }
          if (_f.comboBox1.SelectedIndex == -1)
          {
            _f.comboBox1.SelectedIndex = 0;
          }
          var keyIndex = _f.comboBox1.Items.IndexOf(key);
          if (_f.comboBox1.SelectedIndex == keyIndex)
          {
            SetGraph(key);
          }
        }
      }

      public void SetGraph(string key)
      {
        if (_f.InvokeRequired)
        {
          _f.Invoke(new Action<string>(SetGraph), key);
        }
        else
        {
          if (key != null)
          {
            Graph g;
            if (_dict.TryGetValue(key, out g))
            {
              _f._viewer.Graph = g;
            }
          }
          else
          {
            _f._viewer.Graph = null;
          }
        }
      }
    }

    private readonly GViewer _viewer;
    public UserControlController Controller { get; private set; }

    public GraphViewerUserControl()
    {
      InitializeComponent();
      // Graph viewer 
      var viewer = new GViewer {
        Dock = DockStyle.Fill,
      };
      Controls.Add(viewer);
      _viewer = viewer;
      Controller = new UserControlController(this);
    }

    private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
    {
      Controller.SetGraph((string)comboBox1.SelectedItem);
    }
  }
}
