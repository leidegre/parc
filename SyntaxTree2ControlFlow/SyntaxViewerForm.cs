using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Parc
{
  public partial class SyntaxViewerForm : Form
  {
    // so that the controller can access private members
    public class SyntaxViewerFormController
    {
      private readonly SyntaxViewerForm _f;

      public SyntaxViewerFormController(SyntaxViewerForm f)
      {
        this._f = f;
      }

      public void ParseText(string source)
      {
        if (_f.InvokeRequired)
        {
          _f.Invoke(new Action<string>(ParseText), source);
        }
        else
        {
          string result;
          try
          {
            var syntax = Program.ParseText(source);
            result = syntax.ToString();
            var compiler = new Compiler();
            var compilation = compiler.Compile(syntax);
            var g = Program.CreateVisualGraph(compilation);
            _f.graphViewerUserControl1.Controller.AddGraph("NFA", g);
            var g2 = GraphVisual.CreateVisual(compilation);
            _f.graphViewerUserControl1.Controller.AddGraph("DFA", g2);
          }
          catch (Exception ex)
          {
            result = ex.Message;
          }
          _f.textBox2.Text = result;
        }
      }
    }

    public SyntaxViewerFormController Controller { get; private set; }

    public SyntaxViewerForm()
    {
      InitializeComponent();
      Controller = new SyntaxViewerFormController(this);
      Observable.FromEventPattern(this.textBox1, nameof(textBox1.TextChanged))
        .Throttle(TimeSpan.FromSeconds(1))
        .Subscribe(x => {
          var source = textBox1.Text;
          Controller.ParseText(source);
        });
      textBox1.Text = "\"parc\" {\r\n\tX = a(b|c)+ ;\r\n}\r\n";
    }
  }
}
