using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XdbReader
{
	public class ModelDefinition
	{
		public ModelDefinition()
		{
			this.Textures = new List<string>();
			this.Animations = new Dictionary<string, string>();
		}
		public string name;
		public string geometryFile;
		public string skeletonFile;
		public Dictionary<string, string> Animations { get; private set; }
		public List<string> Textures { get; private set; }
	}
}
