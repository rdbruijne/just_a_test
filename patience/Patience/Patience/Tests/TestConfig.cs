using Patience.Loggers;
using Patience.Output;

namespace Patience.Tests
{
    internal class TestConfig
	{
		public class YamlDoc
		{
			public class SetupNode
			{
				public string program = "";
				public Dictionary<string, List<string>> copy_files = new();
			}

			public class TestNode
			{
				public int level = 0;
				public int retries = 0;
				public int timeout = -1; // in ms
				public string test = "";
				public string description = "";
				public string arguments = "";
				public List<string> output = new();
			}

			public SetupNode? setup = null;
			public List<string>? sub_tests = null;
			public List<TestNode>? tests = null;
		}



		public static TestSet? Parse(string path)
		{
			if(!Directory.Exists(path))
			{
				Terminal.Error($"'{path}' does not exist or is not a directory.");
				return null;
			}

			// parse config file
			return ParseYaml("", Path.GetFullPath(path), null);
		}



		// parse test config file
		private static TestSet? ParseYaml(string name, string path, TestSet? parentSet)
		{
			string fullPath = Path.GetFullPath(path);
			string configPath = Path.Combine(fullPath, "test.yml");

			// try parsing the yaml file
			YamlDoc doc = new();
			if (!Utility.TryParseYamlFile(configPath, ref doc))
				return null;

			// create test set
			TestSet testSet = new(name, path, parentSet, doc.setup);

			// parse tests
			if (doc.tests != null)
			{
				foreach (YamlDoc.TestNode testNode in doc.tests)
					testSet.Add(new Test(fullPath, testSet, testNode));
			}

			// parse sub tests
			if (doc.sub_tests != null)
			{
				doc.sub_tests.Sort();
				foreach (string subTest in doc.sub_tests)
				{
					TestSet? subSet = ParseYaml(subTest, Path.GetFullPath(Path.Combine(testSet.DirectoryPath, subTest)), testSet);
					testSet.Add(subSet);
				}
			}

			return testSet;
		}
	}
}
