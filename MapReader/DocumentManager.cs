using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace XdbReader
{
	public class DocumentManager
	{
		public string RootDirectory { get; private set; }
		
		public DocumentManager(string rootDirectory)
		{
			this.RootDirectory = rootDirectory;
		}

		private Dictionary<string, XDocument> documents = new Dictionary<string, XDocument>();

		public XElement GetLinkedElement(FileLink link)
		{
			string path = link.File;
			XDocument doc;
			if(!documents.TryGetValue(path, out doc))
			{
				if (!File.Exists(path))
				{
					return null;
				}
				doc = XDocument.Load(path);
				documents[path] = doc;
			}
			XElement current = doc.Root;
			foreach(string p in link.PointerLinks.Skip(1))
			{
				current = current.Element(p);
			}
			return current;
		}
	}
}
