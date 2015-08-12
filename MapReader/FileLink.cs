using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XdbReader
{
	public class FileLink
	{
		public string File { get; private set; }

		public string Pointer { get; private set; }

		public string[] PointerLinks { get; private set; }

		public FileLink(string rootDirectory, string currentFile, string href)
		{
			int hashIndex = href.IndexOf('#');
			string filePart = href.Substring(0, hashIndex);
			string path;
			if (filePart[0] == '/')
			{
				path = Path.Combine(rootDirectory, filePart.Substring(1));
			}
			else
			{
				path = Path.Combine(Path.GetDirectoryName(currentFile), filePart);
			}
			this.File = path;

			string suffix = href.Substring(hashIndex + 1);
			this.Pointer = suffix.Substring(suffix.IndexOf('/') + 1, suffix.Length - "xpointer(/)".Length);
			this.PointerLinks = this.Pointer.Split(new char[]{'/'}, StringSplitOptions.RemoveEmptyEntries);
		}
	}
}
