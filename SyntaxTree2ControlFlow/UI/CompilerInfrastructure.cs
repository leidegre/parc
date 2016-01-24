using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive.Subjects;
using System.Text;
using System.Threading.Tasks;

namespace Parc.UI
{
  public class CompilerInfrastructure : ObservableObject
  {
    private string _text;
    public string Text { get { return _text; } set { if (_text != value) { _text = value; OnPropertyChanged(); } } }

    private SyntaxTree _syntax;
    public SyntaxTree Syntax { get { return _syntax; } set { if (_syntax != value) { _syntax = value; OnPropertyChanged(); } } }

    private Compilation _compilation;
    public Compilation Compilation { get { return _compilation; } set { if (_compilation != value) { _compilation = value; OnPropertyChanged(); } } }

    private Microsoft.Glee.Drawing.Graph _nfaGraph;
    public Microsoft.Glee.Drawing.Graph NfaGraph { get { return _nfaGraph; } set { if (_nfaGraph != value) { _nfaGraph = value; OnPropertyChanged(); } } }

    private Microsoft.Glee.Drawing.Graph _dfaGraph;
    public Microsoft.Glee.Drawing.Graph DfaGraph { get { return _dfaGraph; } set { if (_dfaGraph != value) { _dfaGraph = value; OnPropertyChanged(); } } }

    public CompilerInfrastructure()
    {
      this.PropertyChanged += (sender, e) => {
        if (e.PropertyName == nameof(Text))
        {
          try
          {
            Syntax = Program.ParseText(Text);
          }
          catch (Exception ex)
          {
            // ...
          }
        }
        else if (e.PropertyName == nameof(Syntax))
        {
          try
          {
            var compiler = new Compiler();
            var compilation = compiler.Compile(Syntax);
            Compilation = compilation;
          }
          catch (Exception ex)
          {
            // ...
          }
        }
        else if (e.PropertyName == nameof(Compilation))
        {
          // NFA
          try
          {
            var g = Program.CreateVisualGraph(Compilation);
            NfaGraph = g;
          }
          catch (Exception ex)
          {
            // ...
          }
          // DFA
          try
          {
            var g = GraphVisual.CreateVisual(Compilation);
            DfaGraph = g;
          }
          catch (Exception ex)
          {
            // ...
          }
        }
      };
    }
  }
}
