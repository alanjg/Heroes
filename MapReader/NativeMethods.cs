using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace XdbReader
{
	static class NativeMethods
	{
		[DllImport("user32.dll")]
		public static extern int FindWindow(string lpClassName, String lpWindowName);

		[DllImport("user32.dll")]
		public static extern int SendMessage(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);

	}
}
