using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace XdbReader
{
	public class XdbLoader
	{
		public Map Map { get; private set; }
		public Dictionary<string, ModelDefinition> ModelDictionary { get; private set; }
		public string RootDirectory { get; private set; }
		private DocumentManager documentManager;
		public XdbLoader(string rootDirectory)
		{
			this.RootDirectory = rootDirectory;
			this.documentManager = new DocumentManager(rootDirectory);
			this.ModelDictionary = new Dictionary<string, ModelDefinition>();
		}

		public void LoadMap(string file)
		{
			this.Map = new Map();
			string absolutePath = Path.Combine(this.RootDirectory, file);
			XDocument doc = XDocument.Load(absolutePath);
			XElement root = doc.Element("AdvMapDesc");
			XElement objects = root.Element("objects");
			foreach (XElement item in objects.Elements())
			{
				this.LoadMapObject(item, absolutePath);
			}
		}

		public void WriteMap(string file)
		{

		}

		public ModelDefinition LoadCreature(string file)
		{
			string absolutePath = file;
			if (!Path.IsPathRooted(file))
			{
				absolutePath = Path.Combine(this.RootDirectory, file);
			}
			
			XDocument doc = XDocument.Load(absolutePath);
			XElement root = doc.Element("Character");
			XElement model = root.Element("Model");
			XElement arenaAnimSet = root.Element("ArenaAnimSet");
			if (model != null && arenaAnimSet != null)
			{
				XAttribute modelHref = model.Attribute("href");
				XAttribute animSetHref = arenaAnimSet.Attribute("href");
				if (modelHref != null)
				{
					FileLink modelLink = new FileLink(this.RootDirectory, file, modelHref.Value);
					XElement modelElement = this.documentManager.GetLinkedElement(modelLink);
					ModelDefinition def = new ModelDefinition();
					this.LoadMapObjectModel(def, modelElement, modelLink.File);

					if (animSetHref != null)
					{
						FileLink animLink = new FileLink(this.RootDirectory, file, animSetHref.Value);
						XElement animSetElement = this.documentManager.GetLinkedElement(animLink);
						this.LoadMapObjectAnimSet(def, animSetElement, animLink.File);
					}
					string fileName = Path.GetFileName(file);
					def.name = fileName.Substring(0, fileName.IndexOf("."));
					return def;
				}				
			}
			return null;
		}

		public ModelDefinition LoadArenaTownDefinition(string file)
		{
			string absolutePath = file;
			if (!Path.IsPathRooted(file))
			{
				absolutePath = Path.Combine(this.RootDirectory, file);
			}

			XDocument doc = XDocument.Load(absolutePath);
			XElement model = doc.Root;
			ModelDefinition obj = new ModelDefinition();
			this.LoadMapObjectModel(obj, model, file);
			string fileName = Path.GetFileName(file);
			obj.name = fileName.Substring(0, fileName.IndexOf("."));
			return obj;
		}

		public ModelDefinition LoadEffect(string file)
		{
			string absolutePath = file;
			if (!Path.IsPathRooted(file))
			{
				absolutePath = Path.Combine(this.RootDirectory, file);
			}

			XDocument doc = XDocument.Load(absolutePath);
			XElement model = doc.Root;
			ModelDefinition obj = new ModelDefinition();
			this.LoadModelDefinition(obj, model, file);
			string fileName = Path.GetFileName(file);
			obj.name = fileName.Substring(0, fileName.IndexOf("."));
			return obj;
		}


		public ModelDefinition LoadMapObjectDefinition(string file)
		{
			string absolutePath = file;
			if (!Path.IsPathRooted(file))
			{
				absolutePath = Path.Combine(this.RootDirectory, file);
			}
			
			XDocument doc = XDocument.Load(absolutePath);
			XElement root = doc.Root;
			XElement model = root.Element("Model");
			XElement arenaAnimSet = root.Element("AnimSet");
			if (model != null)
			{
				XAttribute modelHref = model.Attribute("href");
				XAttribute animSetHref = arenaAnimSet != null ? arenaAnimSet.Attribute("href") : null;
				if (modelHref != null)
				{
					FileLink modelLink = new FileLink(this.RootDirectory, file, modelHref.Value);
					XElement modelElement = this.documentManager.GetLinkedElement(modelLink);
					ModelDefinition def = new ModelDefinition();
					this.LoadMapObjectModel(def, modelElement, modelLink.File);

					if (animSetHref != null)
					{
						FileLink animLink = new FileLink(this.RootDirectory, file, animSetHref.Value);
						XElement animSetElement = this.documentManager.GetLinkedElement(animLink);
						this.LoadMapObjectAnimSet(def, animSetElement, animLink.File);
					}
					string fileName = Path.GetFileName(file);
					def.name = fileName.Substring(0, fileName.IndexOf("."));
					return def;
				}				
			}
			return null;
		}

		void LoadMapObject(XElement element, string path)
		{
			XElement child = element.Elements().First();
			XElement pos = child.Element("Pos");
			ObjectInstance obj = new ObjectInstance();
			obj.x = float.Parse(pos.Element("x").Value);
			obj.y = float.Parse(pos.Element("y").Value);
			obj.z = float.Parse(pos.Element("z").Value);
			obj.rot = float.Parse(child.Element("Rot").Value);
			string definition = child.Element("Shared").Attribute("href").Value;
			ModelDefinition model;
			if (this.ModelDictionary.TryGetValue(definition, out model))
			{
				obj.model = model;
			}
			else
			{
				FileLink link = new FileLink(this.RootDirectory, path, definition);
				obj.model = new ModelDefinition();

				string fileName = Path.GetFileName(link.File);
				obj.model.name = fileName.Substring(0, fileName.IndexOf("."));
				this.LoadModelDefinition(obj.model, this.documentManager.GetLinkedElement(link), link.File);
				this.ModelDictionary[definition] = obj.model;
			}
			this.Map.Objects.Add(obj);
		}

		void LoadModelDefinition(ModelDefinition model, XElement root, string path)
		{
			XElement mod = root.Element("Model");
			XAttribute modHref = mod.Attribute("href");
			if (modHref != null)
			{
				FileLink modelLink = new FileLink(this.RootDirectory, path, modHref.Value);
				this.LoadMapObjectModel(model, this.documentManager.GetLinkedElement(modelLink), modelLink.File);

				XElement animSet = root.Element("AnimSet");
				if (animSet != null)
				{
					XAttribute animSetHref = animSet.Attribute("href");
					if (animSetHref != null)
					{
						FileLink animSetLink = new FileLink(this.RootDirectory, path, animSetHref.Value);
						this.LoadMapObjectAnimSet(model, this.documentManager.GetLinkedElement(animSetLink), animSetLink.File);
					}
				}
			}
		}

		void LoadMapObjectModel(ModelDefinition obj, XElement model, string path)
		{
			XElement materials = model.Element("Materials");
			foreach(XElement item in materials.Elements())
			{
				XAttribute itemHrefAttrib = item.Attribute("href");
				if (itemHrefAttrib != null)
				{
					string m = itemHrefAttrib.Value;
					XElement material;
					string textureFile = path;
					if (!m.Contains("inline"))
					{
						FileLink materialLink = new FileLink(this.RootDirectory, path, m);
						material = this.documentManager.GetLinkedElement(materialLink);
						textureFile = materialLink.File;
					}
					else
					{
						material = item.Element("Material");
					}
					XElement textureElement = material.Element("Texture");
					XAttribute textureHrefAttribute = textureElement.Attribute("href");
					if (textureHrefAttribute != null)
					{
						string textureHref = textureHrefAttribute.Value;
						FileLink textureLink = new FileLink(this.RootDirectory, textureFile, textureHref);
						XElement textureLinkedElement = this.documentManager.GetLinkedElement(textureLink);
						if (textureLinkedElement != null)
						{
							this.LoadTexture(obj, textureLink.File, textureLinkedElement);
						}
					}
				}
			}
			XElement geom = model.Element("Geometry");
			XAttribute geomHrefAttrib = geom.Attribute("href");
			if (geomHrefAttrib == null) return;
			string geomHref = geomHrefAttrib.Value;
			XElement geomInner;
			if(geomHref.Contains("inline"))
			{
				geomInner = geom.Element("Geometry");
			}
			else
			{
				FileLink geomLink = new FileLink(this.RootDirectory, path, geomHref);
				geomInner = this.documentManager.GetLinkedElement(geomLink);
			}

			string guid = geomInner.Element("uid").Value;
			obj.geometryFile = guid;

			XElement skeleton = model.Element("Skeleton");
			XAttribute skelHrefAttr = skeleton.Attribute("href");
			if(skelHrefAttr != null)
			{
				string skelHref = skelHrefAttr.Value;
				XElement skelInner;
				if(skelHref.Contains("inline"))
				{
					skelInner = skeleton.Element("Skeleton");
				}
				else
				{
					FileLink skelLink = new FileLink(this.RootDirectory, path, skelHref);
					skelInner = this.documentManager.GetLinkedElement(skelLink);
				}

				string suid = skelInner.Element("uid").Value;
				obj.skeletonFile = suid;
			}
		}

		void LoadTexture(ModelDefinition obj, string texturePath, XElement texture)
		{
			string loc = texture.Element("DestName").Attribute("href").Value;
			string path = Path.Combine(Path.GetDirectoryName(texturePath), loc);
			obj.Textures.Add(path);
		}

		void LoadMapObjectAnimSet(ModelDefinition obj, XElement animSet, string path)
		{
			foreach(XElement item in animSet.Element("animations").Elements())
			{
				string name = item.Element("Kind").Value;
				XElement anim = item.Element("Anim");
				if (anim != null)
				{
					XAttribute href = anim.Attribute("href");
					if (href != null)
					{
						FileLink link = new FileLink(this.RootDirectory, path, href.Value);
						this.LoadSkeletalAnimation(obj, this.documentManager.GetLinkedElement(link), name);
					}
				}
			}
		}

		void LoadSkeletalAnimation(ModelDefinition obj, XElement anim, string name)
		{
			string uid = anim.Element("uid").Value;
			obj.Animations[name] = uid;
		}
	}
}
