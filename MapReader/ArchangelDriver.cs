using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Automation;

namespace XdbReader
{
	public class ArchangelDriver
	{
		string MakeDirectoryFile(string sourceFolderPath, string path)
		{
			return Path.Combine(sourceFolderPath, Path.GetFileName(path));
		}
		public void ConvertModel(string name, string factionName, string sourceFolderPath, string meshFile, List<string> textureFiles)
		{
			this.Convert(name, factionName, sourceFolderPath, meshFile, null, textureFiles, false);
		}

		public void ConvertModelWithSkeleton(string name, string factionName, string sourceFolderPath, string meshFile, string skeletonFile, List<string> textureFiles)
		{
			this.Convert(name, factionName, sourceFolderPath, meshFile, skeletonFile, textureFiles, true);
		}

		private void Convert(string name, string factionName, string sourceFolderPath, string meshFile, string skeletonFile, List<string> textureFiles, bool useSkeleton)
		{
			Process process = Process.Start(@"C:\Program Files (x86)\Archangel\Archangel.v0.4.exe");
			process.WaitForInputIdle();
			AutomationElement archangelWindow = AutomationElement.RootElement.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.ProcessIdProperty, process.Id));

			while (archangelWindow == null)
			{
				Thread.Sleep(16);
				archangelWindow = AutomationElement.RootElement.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.ProcessIdProperty, process.Id));
			}

			this.MakeOpenFileDialogChoice(archangelWindow, Path.Combine(sourceFolderPath, meshFile));
			foreach (string texture in textureFiles)
			{
				this.MakeOpenFileDialogChoice(archangelWindow, Path.Combine(sourceFolderPath, Path.GetFileName(texture)));
			}

			if (useSkeleton)
			{
				this.InvokeMenuItem(process, archangelWindow, "Tools", "Load GR2 Skeleton");
				this.MakeOpenFileDialogChoice(archangelWindow, Path.Combine(sourceFolderPath, skeletonFile));
			}
			this.InvokeMenuItem(process, archangelWindow, "Tools", "Export Half-Life SMD");
			this.InvokeDialogOk(process, archangelWindow, "#32770", "OK");
			process.WaitForInputIdle();
			process.Kill();
		}

		void WalkTree(AutomationElement element)
		{
			if (element != null)
			{
				do
				{
					string id = element.Current.AutomationId;
					string className = element.Current.ClassName;
					string name = element.Current.Name;
					this.WalkTree(TreeWalker.ControlViewWalker.GetFirstChild(element));
					element = TreeWalker.ControlViewWalker.GetNextSibling(element);
				} while (element != null);
			}
		}
		private void MakeOpenFileDialogChoice(AutomationElement window, string file)
		{
			AutomationElement archangelFileTextBox = window.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.AutomationIdProperty, "1148"));

			while (archangelFileTextBox == null)
			{
				Thread.Sleep(16);
				archangelFileTextBox = window.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.AutomationIdProperty, "1148"));
			}

			object valuePattern = null;
			AutomationPattern[] patterns = archangelFileTextBox.GetSupportedPatterns();
			if (archangelFileTextBox.TryGetCurrentPattern(ValuePattern.Pattern, out valuePattern))
			{
				((ValuePattern)valuePattern).SetValue(file);
			}
			else
			{
				return;
			}
			var archangelFileOK = window.FindFirst(TreeScope.Descendants, new AndCondition(new PropertyCondition(AutomationElement.AutomationIdProperty, "1"), new PropertyCondition(AutomationElement.ClassNameProperty, "Button")));
			object invokePattern;
			if (archangelFileOK.TryGetCurrentPattern(InvokePattern.Pattern, out invokePattern))
			{
				((InvokePattern)invokePattern).Invoke();
			}
			else
			{
				return;
			}
		}

		private void InvokeMenuItem(Process process, AutomationElement window, string menu, string menuItem)
		{
			AutomationElement menuElement = window.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.NameProperty, menu));
			object menuExpand;
			if (menuElement.TryGetCurrentPattern(ExpandCollapsePattern.Pattern, out menuExpand))
			{
				((ExpandCollapsePattern)menuExpand).Expand();
				process.WaitForInputIdle();
				AutomationElement menuItemElement = menuElement.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.NameProperty, menuItem));
				while (menuItemElement == null)
				{
					Thread.Sleep(16);
					menuItemElement = menuElement.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.NameProperty, menuItem));
				}
				object menuItemInvoke;
				if (menuItemElement.TryGetCurrentPattern(InvokePattern.Pattern, out menuItemInvoke))
				{
					((InvokePattern)menuItemInvoke).Invoke();
				}
			}
		}

		private void InvokeDialogOk(Process process, AutomationElement window, string dialogClass, string okName)
		{
			process.WaitForInputIdle();
			AutomationElement dialog = window.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.ClassNameProperty, dialogClass));
			while(dialog == null)
			{
				Thread.Sleep(16);
				dialog = window.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.ClassNameProperty, dialogClass));
			}
			AutomationElement button = dialog.FindFirst(TreeScope.Descendants, new PropertyCondition(AutomationElement.NameProperty, okName));

			object invoke;
			if (button.TryGetCurrentPattern(InvokePattern.Pattern, out invoke))
			{
				((InvokePattern)invoke).Invoke();
			}
		}
	}
}
