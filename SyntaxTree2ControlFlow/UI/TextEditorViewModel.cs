using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Parc.UI
{
  public class TextEditorViewModel : ObservableObjectFor<TextEditor>
  {
    public override void Initialize()
    {
      var textBox = (TextBox)View.FindName("TextBox");
      var cc = Container.Resolve<CompilerInfrastructure>();
      textBox.Text = cc.Text;
      Observable.FromEventPattern(textBox, nameof(textBox.TextChanged))
        .Throttle(TimeSpan.FromSeconds(1))
        .Subscribe(x => {
          textBox.Dispatcher.Invoke(() => {
            var text = textBox.Text;
            cc.Text = text;
          });
        });
    }
  }
}
