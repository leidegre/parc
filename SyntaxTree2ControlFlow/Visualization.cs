using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Parc
{
  public class Visualization
  {
    private readonly Form _form;
    private readonly Panel _dockPanel;
    private readonly GraphViewerUserControl _flowLayout;

    class HotKeyForm : Form
    {
      public Func<Message, Keys, bool> CanProcessCommandKey { get; set; }
      public Action<Message, Keys> ProcessCommandKey { get; set; }

      protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
      {
        var canProcessCommandKey = CanProcessCommandKey;
        if (canProcessCommandKey != null && canProcessCommandKey(msg, keyData))
        {
          ProcessCommandKey(msg, keyData);
          return true;
        }
        return base.ProcessCmdKey(ref msg, keyData);
      }
    }

    public Visualization()
    {
      var screen = Screen.AllScreens.Except(new[] { Screen.PrimaryScreen }).Concat(new[] { Screen.PrimaryScreen }).First();
      var f = new HotKeyForm {
        StartPosition = FormStartPosition.Manual,
        //Left = screen.WorkingArea.X + screen.WorkingArea.Width / 2,
        //Top = screen.WorkingArea.Y,
        //Width = screen.WorkingArea.Width / 2,
        //Height = screen.WorkingArea.Height,
        Left = screen.WorkingArea.X,
        Top = screen.WorkingArea.Y,
        Width = screen.WorkingArea.Width,
        Height = screen.WorkingArea.Height,
      };
      var dockPanel = new Panel {
        Dock = DockStyle.Top,
        Height = f.ClientSize.Height / 2
      };
      //
      {
        var textBox = new TextBox() {
          Multiline = true,
          AcceptsReturn = true,
          AcceptsTab = true,
          Font = new Font("Consolas", 10, FontStyle.Regular),
          Dock = DockStyle.Left,
          Width = f.ClientSize.Width / 2,
        };
        var textBox2 = new TextBox() {
          Multiline = true,
          AcceptsReturn = true,
          AcceptsTab = true,
          Font = new Font("Consolas", 10, FontStyle.Regular),
          Dock = DockStyle.Right,
          Width = f.ClientSize.Width / 2,
        };

        Observable.FromEventPattern(textBox, nameof(textBox.TextChanged))
          .Throttle(TimeSpan.FromSeconds(1))
          .Subscribe(x => {
            var source = textBox.Text;
            string result;
            try
            {
              var syntax = Program.ParseText(source);
              result = syntax.ToString();
              var compiler = new Compiler();
              var compilation = compiler.Compile(syntax);
              //Program.Optimize(compilation);
              var g = Program.CreateVisualGraph(compilation);
              AddGraph("NFA", g);
              var g2 = GraphVisual.CreateVisual(compilation);
              AddGraph("DFA", g2);
            }
            catch (Exception ex)
            {
              result = ex.Message;
            }
            if (textBox2.InvokeRequired)
            {
              textBox2.Invoke(new Action<string>(text => textBox2.Text = text), result);
            }
            else
            {
              textBox2.Text = result;
            }
          });

        textBox.Text = "\"parc\" {\r\n\tX = a(b|c)* ;\r\n}\r\n";

        dockPanel.Controls.Add(textBox);
        dockPanel.Controls.Add(textBox2);
      }
      //
      var flowLayout = new GraphViewerUserControl {
        Dock = DockStyle.Bottom,
        Height = f.ClientSize.Height / 2
      };
      f.CanProcessCommandKey += (msg, keyData) => {
        if (keyData == (Keys.Control | Keys.C))
        {
          var mpt = Control.MousePosition;
          var localPos = f.PointToClient(mpt);
          var localPos2 = new Point(localPos.X - flowLayout.Location.X, localPos.Y - flowLayout.Location.Y);
          var viewer = flowLayout.GetChildAtPoint(localPos2) as Microsoft.Glee.GraphViewerGdi.GViewer;
          return viewer != null;
        }
        return false;
      };
      f.ProcessCommandKey += (msg, keyData) => {
        if (keyData == (Keys.Control | Keys.C))
        {
          var mpt = Control.MousePosition;
          var localPos = f.PointToClient(mpt);
          var localPos2 = new Point(localPos.X - flowLayout.Location.X, localPos.Y - flowLayout.Location.Y);
          var viewer = flowLayout.GetChildAtPoint(localPos2) as Microsoft.Glee.GraphViewerGdi.GViewer;
          if (viewer != null)
          {
            // render graph to clipboard
            var gw = viewer.GraphWidth;
            var gh = viewer.GraphHeight;
            var sz = viewer.ClientSize;
            var sx = sz.Width / gw;
            var sy = sz.Height / gh;
            var ss = sz.Width > sz.Height ? sx : sy;
            using (var bmp = new Bitmap((int)(ss * gw), (int)(ss * gh), System.Drawing.Imaging.PixelFormat.Format32bppArgb))
            {
              using (var gfx = Graphics.FromImage(bmp))
              {
                // yay!
                gfx.Clear(Color.White);
                gfx.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                var bg = viewer.Graph.GraphAttr.Backgroundcolor;
                viewer.Graph.GraphAttr.Backgroundcolor = Microsoft.Glee.Drawing.Color.Transparent;
                var renderer = new Microsoft.Glee.GraphViewerGdi.GraphRenderer(viewer.Graph);
                renderer.CalculateLayout();
                renderer.Render(gfx, 0, 0, bmp.Width, bmp.Height);
                viewer.Graph.GraphAttr.Backgroundcolor = bg;
              }
              Clipboard.SetImage(bmp);
            }
          }
        }
      };
      f.Controls.Add(dockPanel);
      f.Controls.Add(flowLayout);
      _form = f;
      _dockPanel = dockPanel;
      _flowLayout = flowLayout;
    }

    public void AddGraph(string key, Microsoft.Glee.Drawing.Graph drawingGraph)
    {
      _flowLayout.Controller.AddGraph(key, drawingGraph);
    }

    public void Show()
    {
      Application.Run(_form);
    }
  }
}
