using Patience.Rapports;
using Patience.Tests;

namespace Patience.Output
{
	public class TextRapport : IRapporter
	{
		List<string> Lines = new();

		public void LogTest(Test test)
		{
			Lines.Add(test.Command);
			Lines.Add($"> {test.ParentSet.Program} {test.Arguments}");
			Lines.AddRange(test.Result.StandardOutput);
		}



		public void LogTestSet(TestSet testSet)
		{
			Lines.Add(testSet.Name);
			foreach (Test test in testSet.Tests)
				LogTest(test);
		}



		public void WriteRapport(TestSet rootSet)
		{
			File.WriteAllLines(Path.Combine(rootSet.DirectoryPath, "testResults.log"), Lines.ToArray());
		}
	}
}
