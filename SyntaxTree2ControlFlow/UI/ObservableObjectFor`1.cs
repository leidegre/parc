using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc.UI
{
  public class ObservableObjectFor<TView> : ObservableObject
  {
    public Container Container { get; set; }
    public TView View { get; set; }

    public virtual void Initialize()
    {
    }
  }
}
