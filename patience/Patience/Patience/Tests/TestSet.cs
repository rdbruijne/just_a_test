namespace Patience.Tests
{
	public class TestSet
	{
		// config
		public string Name { get; private set; } = "";
		public string DirectoryPath { get; private set; } = "";
		public string ConfigPath { get; private set; } = "";
		public string Program { get; private set; } = "";
		public List<(string, string)> CopyFilePaths { get; private set; } = new();

		public List<Test> Tests { get; private set; } = new();
		public List<TestSet> TestSets { get; private set; } = new();
		public TestSet? ParentSet { get; private set; } = null;

		// skip
		public bool Skip { get; private set; } = false;

		// result
		public TestSetResult Result = new();

		// helpers
		public string RefDir => Path.GetFullPath(Path.Combine(DirectoryPath, "ref"));
		public string ResultDir => Path.GetFullPath(Path.Combine(DirectoryPath, "result"));
		public string ProgramPath => Path.GetFullPath(Path.Combine(DirectoryPath, Program));

		public string DisplayName()
		{
			string parentDisplayName = ParentSet?.DisplayName() ?? "";
			if(!string.IsNullOrWhiteSpace(parentDisplayName))
				parentDisplayName += "/";
			return parentDisplayName + Name;
		}

		// private members
		private bool SetupDone = false;



		internal TestSet(string name, string path, TestSet? parentSet, TestConfig.YamlDoc.SetupNode? setup)
		{
			Name = name;
			if (name.ToLower().EndsWith("/test"))
				Name = Name[..^5];
			DirectoryPath = path;
			ConfigPath = Path.Combine(DirectoryPath, "test.yml");

			ParentSet = parentSet;

			if (setup != null)
			{
				Program = setup.program;
				foreach ((string from, List<string> toList) in setup.copy_files)
					foreach (string to in toList)
						CopyFilePaths.Add((from, to));
			}
		}



		// tests
		public void Add(Test? test)
		{
			if (test != null)
				Tests.Add(test);
		}



		public int RecursiveTestCount()
		{
			int count = Tests.Where(t => !t.Skip).Count();
			foreach (TestSet set in TestSets)
			{
				if (!set.Skip)
					count += set.RecursiveTestCount();
			}
			return count;
		}



		public List<Test> RecursiveTests()
		{
			List<Test> tests = new();
			tests.AddRange(Tests.Where(t => !t.Skip));
			foreach (TestSet set in TestSets)
			{
				if (!set.Skip)
					tests.AddRange(set.RecursiveTests());
			}
			return tests;
		}



		// test sets
		public void Add(TestSet? set)
		{
			if (set != null)
				TestSets.Add(set);
		}



		public int RecursiveTestSetCount()
		{
			int count = TestSets.Count;
			foreach (TestSet set in TestSets)
			{
				if(!set.Skip)
					count += set.RecursiveTestSetCount();
			}
			return count;
		}



		public List<TestSet> RecursiveTestSets()
		{
			List<TestSet> sets = new() { this };
			foreach (TestSet set in TestSets)
			{
				if (!set.Skip)
					sets.AddRange(set.RecursiveTestSets());
			}
			return sets;
		}



		public TestSetResult GatherResults()
		{
			TestSetResult result = Result;
			foreach (TestSet set in TestSets)
				result.Add(set.GatherResults());
			return result;
		}



		// setup
		public void RunSetup()
		{
			if (SetupDone)
				return;

			// clean 'result' dir (make new if doesn't exist)
			if (Directory.Exists(ResultDir))
				Directory.Delete(ResultDir, true);
			Directory.CreateDirectory(ResultDir);

			// sopy files
			foreach ((string from, string to) in CopyFilePaths)
				File.Copy(from, to);

			SetupDone = true;
		}



		// skip tests
		public void SkipTests(IEnumerable<string> testsToSkip)
		{
			if (testsToSkip.Contains(DisplayName()))
				Skip = true;

			foreach (Test test in Tests)
				test.SkipTests(testsToSkip);

			foreach (TestSet set in TestSets)
				set.SkipTests(testsToSkip);
		}
	}
}
