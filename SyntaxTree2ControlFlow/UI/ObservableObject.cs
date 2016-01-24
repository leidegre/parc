using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Parc.UI
{
  public abstract class ObservableObject : INotifyPropertyChanged
  {
    public event PropertyChangedEventHandler PropertyChanged;
    private Subject<string> _propertyChanged;

    protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
    {
      var propertyChanged = PropertyChanged;
      if (propertyChanged != null)
      {
        propertyChanged(this, new PropertyChangedEventArgs(propertyName));
      }
      if (_propertyChanged != null)
      {
        _propertyChanged.OnNext(propertyName);
      }
    }

    public IObservable<string> AsObservable()
    {
      if (_propertyChanged == null)
      {
        _propertyChanged = new Subject<string>();
      }
      return _propertyChanged;
    }
  }
}
