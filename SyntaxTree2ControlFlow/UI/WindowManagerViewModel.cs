using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Parc.UI
{
  public class WindowManagerViewModel : ObservableObjectFor<WindowManager>
  {
    public override void Initialize()
    {
      this.View.Loaded += (sender, e) => {
        var g = (Grid)View.FindName("Grid");
        g.ColumnDefinitions.Add(new ColumnDefinition { Width = new System.Windows.GridLength(1, System.Windows.GridUnitType.Star) });
        g.ColumnDefinitions.Add(new ColumnDefinition { Width = new System.Windows.GridLength(2, System.Windows.GridUnitType.Star) });
        g.RowDefinitions.Add(new RowDefinition { });
        g.RowDefinitions.Add(new RowDefinition { });

        AddUIHost(UIHostComponent.TextEditor, rowSpan: 2);
        AddUIHost(UIHostComponent.NfaViz, column: 1);
        AddUIHost(UIHostComponent.DfaViz, column: 1, row: 1);
      };
    }

    public void AddUIHost(string component, int? column = null, int? columnSpan = null, int? row = null, int? rowSpan = null)
    {
      var uiHost = new UIHost();
      var uiHostViewModel = Container.Setup(uiHost);

      uiHostViewModel.SelectedComponent = component;

      if (column.HasValue)
      {
        Grid.SetColumn(uiHost, column.Value);
      }
      if (columnSpan.HasValue)
      {
        Grid.SetColumnSpan(uiHost, columnSpan.Value);
      }
      if (row.HasValue)
      {
        Grid.SetRow(uiHost, row.Value);
      }
      if (rowSpan.HasValue)
      {
        Grid.SetRowSpan(uiHost, rowSpan.Value);
      }

      var g = (Grid)View.FindName("Grid");
      g.Children.Add(uiHost);
    }
  }
}
