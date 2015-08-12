using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XdbReader
{
	public class Map
	{
		public List<ObjectInstance> Objects { get; private set; }
		public Map()
		{
			this.Objects = new List<ObjectInstance>();
		}
	}
}
