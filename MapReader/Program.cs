using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XdbReader
{
	class Program
	{
		const string sourceFolder = @"G:\Code\HeroesAnimation\Heroes5Models\";
		const string destFolder = @"G:\Code\HeroesAnimation\HeroesAnimations2\";
		const string gr2converterLocation = @"C:\Users\alan_\Documents\GitHub\Heroes\Release\Gr2Converter.exe";

		static string[] badShotModels = new string[] {
		};

		static string[] badCreatureModels = new string[] { 
			//bone count mismatch
			"T4_BattleRager", 
			"Brawler", 

	//		"Militiaman", // animation missing, probably from renegade
	//		"Peasant", // animation missing, probably from renegade
	//		"WalkingDead", // animation missing, probably from renegade

			// texture count mismatch
			"PeasantHut",
			"DirtTree1x1_6",
			"DirtTree1x1_1",
			"DirtTree4x5_2",
			"DirtTree1x1_3",
			"DirtTree1x1_2",
			"DirtTree1x1_5",
			"DirtTree1x1_4",
			"DirtTree2x3_4",

			// texture count mismatch
			"Elemantal_Stockpile_Snow",

			// bad texture
			"Artifact.(AdvMapArtifactShared)",
		};

		static string[] badMapObjectsModels = new string[] { 
			//bone count mismatch 
			"Zehir",
			"Freida",
			"Freyda",
			"OrcCommonHero_DSU",
			"Ornella_DSM",
			"Ornella_DSU",
			"Ornella_Heaven_DSU",
			"Matriarch",
			"Matron",
			"Battlerager",
			"Duncan",
			"Freyda",
			"Ornella",
			"Ornella_necro",
			"Gottai",
			"Hero1",
			"Hero2",
			"Hero3",
			"Hero4",
			"Hero5",
			"Hero6",
			"Hero7",
			"Hero8",
			"Hero9",
			"Kujin",

			// texture count mismatch
			"PeasantHut",
			"DirtTree1x1_6",
			"DirtTree1x1_1",
			"DirtTree4x5_2",
			"DirtTree1x1_3",
			"DirtTree1x1_2",
			"DirtTree1x1_5",
			"DirtTree1x1_4",
			"DirtTree2x3_4",

			// texture count mismatch
			"Elemantal_Stockpile_Snow",
			"Artifact",
			"Zehir_DSU",
			"Lake4x4_1",
			"Lake5x5_1",
			"Waterfall_01",
			"WaterObj",
			"DragonHeart",

			// no geometry
			"HeroCharacterAdventure",
			"HeroCharacterArena",
			"Mosstree01",
			"01",
			"MountainBig",


			// bad texture
			"Artifact.(AdvMapArtifactShared)",
			"River_T-river",

			// no vertices in mesh
			"SmallStone_1x1_01"
		};

		static string[] badArenaTownModels = new string[]
		{
			"river_noLM",
		};

		static void Main(string[] args)
		{
			bool loadMap = false;
			XdbLoader loader = new XdbLoader(sourceFolder);
			if (loadMap)
			{				
				loader.LoadMap(@"Maps\Multiplayer\XL1\XL1.xdb");
				int limit = 300;
				int count = 0;
				foreach (ModelDefinition def in loader.ModelDictionary.Values)
				{
					if (count == limit) break;
					if (badMapObjectsModels.Any(n => n == def.name)) continue;
					ConvertCreature(def, Path.Combine(destFolder, @"ConvertedMap2\"));
					count++;
				}
			}
			bool loadFlag = false;
			if (loadFlag)
			{
				string armyFlagFile = Path.Combine(sourceFolder, @"Effects\_(ModelInstance)\Effects\UI\ArmyFlag.xdb");
				ModelDefinition mod = loader.LoadEffect(armyFlagFile);
				ConvertCreature(mod, Path.Combine(destFolder, @"ConvertedEffects\"));
			}
			//loader.LoadMapObjectDefinition(@"E:\Heroes5Models\MapObjects\Haven\ArchersTower.(AdvMapDwellingShared).xdb");
			//		LoadAllMapObjects(loader, null);
			//		foreach (ObjectInstance obj in loader.Map.Objects)
			//		{
			//			ConvertCreature(obj.model, @"e:\HeroesAnimations\ConvertedMap2");
			//		}
			//	ModelDefinition creature = loader.LoadCreature(@"Characters\Creatures\Haven\Archangel.(Character).xdb");
			//	ConvertCreature(creature, Path.Combine(destFolder, @"Converted\"));

			bool loadShot = true;
			if (loadShot)
			{
				ModelDefinition arrow = loader.LoadShot(@"GameMechanics\Shot\Creatures\Haven\Archer.xdb");
				ConvertCreature(arrow, Path.Combine(destFolder, @"Converted\"));
			}
			LoadAllShots(loader, null);
	//		LoadAllCreatures(loader, null);
	//LoadAllCreatures(loader, null);

			//		ConvertArenaTownsDirectory(loader, "river_noLM");
		}

		static void LoadAllShots(XdbLoader loader, string skipTo)
		{
			List<ModelDefinition> shots = new List<ModelDefinition>();
			string shotFolder = Path.Combine(sourceFolder, @"GameMechanics\Shot");
			foreach (string file in Directory.GetFiles(shotFolder, "*.xdb", SearchOption.AllDirectories))
			{
				ModelDefinition shot = loader.LoadShot(file);
				if (shot == null) continue;
				shots.Add(shot);

				// skip to this
				if (skipTo != null)
				{
					if (skipTo == Path.GetFileName(file))
					{
						skipTo = null;
					}
					else
					{
						continue;
					}
				}
				if (badShotModels.Any(n => n == Path.GetFileName(file))) continue;

				ConvertCreature(shot, Path.Combine(destFolder, @"Converted\Shots\"));
			}
		}

		static void ConvertArenaTownsDirectory(XdbLoader loader, string skipTo)
		{
			string[] skipList = new string[] { "(Texture)", "(Material)", "(Camera)", "-geom", "(ArenaModObject)", "(ArenaLocObject)", "(ArenaEnvObject)", "(ArenaDesc)" };
			List<ModelDefinition> mapObjects = new List<ModelDefinition>();
			string mapObjectsFolder = Path.Combine(sourceFolder, @"Arenas\Town\NewHaven");
			foreach (string file in Directory.GetFiles(mapObjectsFolder, "*.xdb", SearchOption.AllDirectories))
			{
				if (skipList.Any(s => file.Contains(s)))
				{
					continue;
				}

				ModelDefinition mapObject = loader.LoadArenaTownDefinition(file);
				if (mapObject == null) continue;
				mapObjects.Add(mapObject);

				// skip to this
				if (skipTo != null)
				{
					if (skipTo == mapObject.name)
					{
						skipTo = null;
					}
					else
					{
						continue;
					}
				}
				if (badArenaTownModels.Any(n => n == mapObject.name)) continue;

				ConvertCreature(mapObject, Path.Combine(destFolder, @"ConvertedArenas\Town\NewHaven"));
			}
		}

		static void LoadAllMapObjects(XdbLoader loader, string skipTo)
		{
			List<ModelDefinition> mapObjects = new List<ModelDefinition>();
			string mapObjectsFolder = Path.Combine(sourceFolder, @"MapObjects");
			foreach (string file in Directory.GetFiles(mapObjectsFolder, "*.xdb", SearchOption.AllDirectories))
			{
				if (file.Contains("(Texture)") || file.Contains("(Material)"))
				{
					continue;
				}

				ModelDefinition mapObject = loader.LoadMapObjectDefinition(file);
				if (mapObject == null) continue;
				mapObjects.Add(mapObject);

				// skip to this
				if (skipTo != null)
				{
					if (skipTo == mapObject.name)
					{
						skipTo = null;
					}
					else
					{
						continue;
					}
				}
				if (badMapObjectsModels.Any(n => n == mapObject.name)) continue;

				ConvertCreature(mapObject, Path.Combine(destFolder, @"ConvertedMap3"));
			}
		}

		static void LoadAllCreatures(XdbLoader loader, string skipTo)
		{
			List<ModelDefinition> creatures = new List<ModelDefinition>();
			string creatureFolder = Path.Combine(sourceFolder, @"Characters\Creatures");
			foreach (string file in Directory.GetFiles(creatureFolder, "*.xdb", SearchOption.AllDirectories))
			{
				if (file.Contains("_LOD") || file.Contains("(CharacterView)") || !file.Contains("(Character)") || file.Contains("Renegades"))
				{
					continue;
				}
				
				ModelDefinition creature = loader.LoadCreature(file);
				if (creature == null) continue;
				creatures.Add(creature);
				
				// skip to this
				if (skipTo != null)
				{
					if(skipTo == creature.name)
					{
						skipTo = null;
					}
					else
					{
						continue;
					}
				}
				if (badCreatureModels.Any(n => n == creature.name)) continue;

				ConvertCreature(creature, Path.Combine(destFolder, @"Converted\"));
			}
		}

		static void ConvertCreature(ModelDefinition model, string targetDirectory)
		{
			string binFolder = Path.Combine(sourceFolder, "bin");
			string modelsFolder = Path.Combine(binFolder, "Geometries");
			string skeletonFolder = Path.Combine(binFolder, "Skeletons");
			string animationsFolder = Path.Combine(binFolder, "animations");

			bool hasGeometry = model.geometryFile != null;

			if (!hasGeometry)
			{
				return;
			}
			bool hasSkeleton = model.skeletonFile != null;
			bool hasAnimations = hasSkeleton && model.Animations.Count(kvp => File.Exists(Path.Combine(animationsFolder, kvp.Value))) > 0;
			
			string creatureDirectory = Path.Combine(targetDirectory, model.name);
			string meshName = hasSkeleton && hasAnimations ? "geometry.skinnedmesh" : "geometry.staticmesh";
			string meshSMDName = meshName + ".smd";
			string meshSMDBinName = meshSMDName + ".bin";
			string meshFileName = Path.Combine(creatureDirectory, meshName);
			
			if (Directory.Exists(creatureDirectory))
			{
				foreach (string file in Directory.GetFiles(creatureDirectory, "*.*", SearchOption.AllDirectories))
				{
					File.SetAttributes(file, FileAttributes.Normal);
				}
				Directory.Delete(creatureDirectory, true);
			}
			while (Directory.Exists(creatureDirectory))
			{
				System.Threading.Thread.Sleep(16);
			}
			Directory.CreateDirectory(creatureDirectory);

			File.Copy(Path.Combine(modelsFolder, model.geometryFile), Path.Combine(creatureDirectory, meshFileName));
			if (hasSkeleton)
			{
				File.Copy(Path.Combine(skeletonFolder, model.skeletonFile), Path.Combine(creatureDirectory, "skeleton.gr2"));
			}

			List<string> animations = new List<string>();
			if (hasAnimations)
			{
				foreach (KeyValuePair<string, string> animation in model.Animations)
				{
					string targetFile = Path.Combine(creatureDirectory, animation.Key + ".gr2");
					string source = Path.Combine(animationsFolder, animation.Value);
					if (!File.Exists(source))
					{
						continue;
					}
					File.Copy(source, targetFile);

					animations.Add(targetFile);
				}
			}

			List<string> textures = new List<string>();
			foreach (string texture in model.Textures)
			{
				string targetFile = Path.Combine(creatureDirectory, Path.GetFileName(texture));
				if (!File.Exists(targetFile))
				{
					File.Copy(texture, targetFile);
				}
				textures.Add(targetFile);
			}
			ArchangelDriver driver = new ArchangelDriver();
			if (hasSkeleton)
			{
				driver.ConvertModelWithSkeleton(model.name, "Foo", creatureDirectory, meshName, "skeleton.gr2", textures);
			}
			else
			{
				driver.ConvertModel(model.name, "Foo", creatureDirectory, meshName, textures);
			}

			if (hasSkeleton && hasAnimations)
			{
				ConvertSkinnedSMDFile(Path.Combine(creatureDirectory, meshSMDName), Path.Combine(creatureDirectory, meshSMDBinName));
			}
			else
			{
				ConvertStaticSMDFile(Path.Combine(creatureDirectory, meshSMDName), Path.Combine(creatureDirectory, meshSMDBinName));
			}

			if (hasSkeleton)
			{
				ConvertGr2File(Path.Combine(creatureDirectory, "skeleton.gr2"), Path.Combine(creatureDirectory, "skeleton"), "skeleton");
			}

			if (hasAnimations)
			{
				string animationDirectory = Path.Combine(creatureDirectory, "animations");
				Directory.CreateDirectory(animationDirectory);

				foreach (string animation in animations)
				{
					ConvertGr2File(animation, Path.Combine(animationDirectory, Path.GetFileNameWithoutExtension(animation)), "animation");
				}
			}
		}

		static void ConvertGr2File(string source, string dest, string type)
		{
			string args = type + " \"" + source + "\" \"" + dest + "\"";
			Process p = Process.Start(gr2converterLocation, args);
			p.WaitForExit();
		}

		static void ConvertSkinnedSMDFile(string source, string dest)
		{
			string args = "skinnedmesh" + " \"" + source + "\" \"" + dest + "\"";
			Process p = Process.Start(gr2converterLocation, args);
			p.WaitForExit();
		}

		static void ConvertStaticSMDFile(string source, string dest)
		{
			string args = "staticmesh" + " \"" + source + "\" \"" + dest + "\"";
			Process p = Process.Start(gr2converterLocation, args);
			p.WaitForExit();
		}
	}
}