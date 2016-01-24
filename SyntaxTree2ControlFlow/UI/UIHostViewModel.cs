using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Parc.UI
{
  public class UIHostViewModel : ObservableObjectFor<UIHost>
  {
    public ObservableCollection<string> Components { get; private set; }

    private string _selectedComponent;
    public string SelectedComponent
    {
      get
      {
        return _selectedComponent;
      }
      set
      {
        if (_selectedComponent != value)
        {
          _selectedComponent = value;
          OnPropertyChanged();
        }
      }
    }

    public UIHostViewModel()
    {
      Components = new ObservableCollection<string> {
        UIHostComponent.TextEditor,
        UIHostComponent.NfaViz,
        UIHostComponent.DfaViz,
      };

      // subscribe
      this.PropertyChanged += (sender, e) => {
        if (e.PropertyName == nameof(SelectedComponent))
        {
          SetContent(SelectedComponent);
        }
      };
    }

    private void SetContent(string component)
    {
      var contentPresenter = (ContentPresenter)View.FindName("ContentPresenter");

      if (component == UIHostComponent.TextEditor)
      {
        var textEditor = new TextEditor();
        Container.Setup(textEditor);
        contentPresenter.Content = textEditor;
      }

      if (component == UIHostComponent.NfaViz)
      {
        var nfaViz = new GraphViewerComponent();
        var nfaVizM = Container.Setup(nfaViz);

        var cc = Container.Resolve<CompilerInfrastructure>();
        nfaVizM.Graph = cc.NfaGraph;
        cc.AsObservable()
          .Where(x => x == nameof(cc.NfaGraph))
          .Subscribe(x => {
            nfaVizM.Graph = cc.NfaGraph;
          });

        contentPresenter.Content = nfaViz;
      }

      if (component == UIHostComponent.DfaViz)
      {
        var dfaViz = new GraphViewerComponent();
        var dfaVizM = Container.Setup(dfaViz);

        var cc = Container.Resolve<CompilerInfrastructure>();
        dfaVizM.Graph = cc.DfaGraph;
        cc.AsObservable()
          .Where(x => x == nameof(cc.DfaGraph))
          .Subscribe(x => {
            dfaVizM.Graph = cc.DfaGraph;
          });

        contentPresenter.Content = dfaViz;
      }
    }
  }
}
