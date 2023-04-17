using Patience.Loggers;
using Patience.Output;
using YamlDotNet.Serialization;

namespace Patience
{
	public class ProgramConfig
	{
		public string DiffTool = "";
		public Terminal.LogLevels LogLevel = Terminal.LogLevels.Info;
		public List<string> SkipTests = new();



		private class YamlDoc
		{
			public string loglevel = Terminal.LogLevels.Info.ToString().ToLower();
			public string difftool = "";
			public string[] skip_tests = Array.Empty<string>();
		}



		public void Parse(string configFile)
		{
			// create new config file if none exists
			if (!File.Exists(configFile))
			{
				File.WriteAllText(configFile, (new SerializerBuilder()).Build().Serialize(new YamlDoc()));
				return;
			}

			// parse yaml file
			YamlDoc doc = new();
			if (!Utility.TryParseYamlFile(configFile, ref doc))
				return;

			// log level
			foreach (Terminal.LogLevels level in Enum.GetValues<Terminal.LogLevels>())
			{
				if (level.ToString().ToLower() == doc.loglevel.ToLower())
				{
					LogLevel = level;
					break;
				}
			}

			// diff tool
			if (!string.IsNullOrWhiteSpace(doc.difftool))
			{
				if (!File.Exists(doc.difftool))
					Terminal.Error($"Diff tool could not be found at '{doc.difftool}'");
				else
					DiffTool = doc.difftool;
			}

			// skip tests
			SkipTests = doc.skip_tests.ToList();
		}
	}
}
