using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive.Subjects;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.Integration;
using Microsoft.Glee.Drawing;
using Microsoft.Glee.GraphViewerGdi;

namespace Parc.UI
{
  public class GraphViewerViewModel : ObservableObjectFor<GraphViewerComponent>
  {
    private Microsoft.Glee.Drawing.Graph _graph;
    public Microsoft.Glee.Drawing.Graph Graph
    {
      get
      {
        return _graph;
      }
      set
      {
        if (_graph != value)
        {
          _graph = value;
          OnPropertyChanged();
        }
      }
    }

    public override void Initialize()
    {
      var viewer = new Microsoft.Glee.GraphViewerGdi.GViewer {
        Dock = System.Windows.Forms.DockStyle.Fill,
      };
      this.PropertyChanged += (sender, e) => {
        if (e.PropertyName == nameof(Graph))
        {
          viewer.Graph = Graph;
        }
      };
      var windowsFormsHost = (WindowsFormsHost)View.FindName("WindowsFormsHost");
      windowsFormsHost.Child = viewer;
    }
  }
}
