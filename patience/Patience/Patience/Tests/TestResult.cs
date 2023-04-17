namespace Patience.Tests
{
	public class TestResult
	{
		// execution output
		public int ExitCode = 0;
		public List<string> StandardOutput = new();
		public List<string> StandardError = new();

		// output reasoning
		public List<string> Info = new();
		public List<(string outFile, string refFile)> RefErrors = new();
	}
}
