using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc.UI
{
  public interface IUserControl<TView, TViewModel>
    where TView : IUserControl<TView, TViewModel>, new()
    where TViewModel : ObservableObjectFor<TView>, new()
  {
    object DataContext { get; set; }
  }
}
