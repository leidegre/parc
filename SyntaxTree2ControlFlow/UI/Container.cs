using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Parc.UI
{
  public class Container
  {
    private readonly Dictionary<Type, object> _container = new Dictionary<Type, object>();

    public void Register<T>(Func<T> f)
    {
      _container.Add(typeof(T), f);
    }

    public void RegisterSingleton<T>(Func<T> f)
    {
      var lazy = new Lazy<T>(f);
      Register(() => lazy.Value);
    }

    public T Resolve<T>()
    {
      object action;
      if (_container.TryGetValue(typeof(T), out action))
      {
        return ((Func<T>)action)();
      }
      throw new InvalidOperationException("Type '" + typeof(T) + "' has not been registered with the container.");
    }

    public TViewModel Setup<TView, TViewModel>(IUserControl<TView, TViewModel> view)
      where TView : IUserControl<TView, TViewModel>, new()
      where TViewModel : ObservableObjectFor<TView>, new()
    {
      var viewModel = new TViewModel {
        Container = this,
        View = (TView)view,
      };
      viewModel.Initialize(); // before data context is set
      view.DataContext = viewModel;
      return viewModel;
    }
  }
}
