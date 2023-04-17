using System.Diagnostics.CodeAnalysis;
using Patience.Comparing;
using static System.Net.Mime.MediaTypeNames;

namespace Patience.Tests
{
	public class Test
	{
		// TODO: single output for multiple tests with the same path+name
		public enum TestStatus
		{
			Unkown,
			Succes,
			Failed,
			Skipped
		}

		// config
		public string ConfigPath { get; private set; } = "";

		public int Level { get; private set; } = 0;
		public int Timeout { get; private set; } = -1;
		public int Retries { get; private set; } = 0;
		public string Name { get; private set; } = "";
		public string Description { get; private set; } = "";
		public string Arguments { get; private set; } = "";
		public List<string> ResultFiles { get; private set; } = new();

		// skip
		public bool Skip { get; private set; } = false;

		// runtime helpers
		public TestSet ParentSet { get; private set; }

		// helpers
		public string DirectoryPath => ParentSet.DirectoryPath;
		public string RefOutFile => Path.Combine(ParentSet.RefDir, Name + ".ref");
		public string ResultOutFile => Path.Combine(ParentSet.ResultDir, Name + ".log");
		public string DisplayName => $"{ParentSet.Name} -> {Name}";
		public string DisplayPath => $"{ParentSet.Name}/{Name}";
		public string Command => $"{ParentSet.Program} {Arguments}".Trim();

		// test result
		public TestStatus Status = TestStatus.Unkown;
		public TestResult Result = new();



		internal Test(string path, TestSet parentSet, TestConfig.YamlDoc.TestNode testNode)
		{
			ConfigPath = path;

			ParentSet = parentSet;

			Level = testNode.level;
			Timeout = testNode.timeout;
			Retries = testNode.retries;
			Name = testNode.test;
			Description = testNode.description;
			Arguments = testNode.arguments;
			ResultFiles = testNode.output;
		}



		// skip tests
		public void SkipTests(IEnumerable<string> testsToSkip)
		{
			if (testsToSkip.Contains(DisplayPath))
				Skip = true;
		}



		// determine if we succeeded or not
		public void CheckResults()
		{
			// write output to file
			File.WriteAllLines(ResultOutFile, new string[]{ Command });
			File.AppendAllLines(ResultOutFile, Result.StandardOutput.ToArray());

			// output
			List<string> extraInfo = new();
			if (!Compare.Files(ResultOutFile, RefOutFile, ref extraInfo))
			{
				Status = TestStatus.Failed;
				Result.Info.AddRange(extraInfo);
				Result.RefErrors.Add((ResultOutFile, RefOutFile));
			}

			// output files
			foreach (string resultFile in ResultFiles)
			{
				string fileName = Path.GetFileName(resultFile);
				string refFile = Path.Combine(ParentSet.RefDir, fileName);
				string outFile = Path.Combine(ParentSet.ResultDir, fileName);
				extraInfo.Clear();
				if (!Compare.Files(outFile, refFile, ref extraInfo))
				{
					Status = TestStatus.Failed;
					Result.Info.AddRange(extraInfo);
					Result.RefErrors.Add((outFile, refFile));
				}
			}

			// check exit code
			if (Result.ExitCode != 0)
				Status = TestStatus.Failed;

			// If the test hasn't failed by now, it's a success
			if (Status != TestStatus.Failed)
				Status = TestStatus.Succes;
		}
	}
}
