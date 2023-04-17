namespace Patience.Tests
{
	public class TestSetResult
	{
		public List<Test> FailedTests = new();
		public List<Test> SucceededTests = new();
		public List<Test> SkippedTests = new();



		public void Add(TestSetResult result)
		{
			FailedTests.AddRange(result.FailedTests);
			SucceededTests.AddRange(result.SucceededTests);
			SkippedTests.AddRange(result.SkippedTests);
		}



		public void Add(Test test)
		{
			if (test.Status == Test.TestStatus.Failed)
				FailedTests.Add(test);
			else if (test.Status == Test.TestStatus.Succes)
				SucceededTests.Add(test);
		}



		// gather
		public static TestSetResult GatherAll(TestSet testSet)
		{
			TestSetResult result = testSet.Result;
			foreach (TestSet set in testSet.TestSets)
				result.Add(GatherAll(set));
			return result;
		}
	}
}
