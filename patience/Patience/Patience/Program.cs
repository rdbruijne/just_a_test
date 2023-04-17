using Patience.Loggers;
using Patience.Output;
using Patience.Tests;

namespace Patience
{
	internal class Program
	{
		const string Indent = "    ";
		const string Indent2 = $"{Indent}{Indent}";



		static void RunTestsForSet(TestSet testSet, int setIx = 0, int setCount = 0)
		{
			// run the setup for the set
			testSet.RunSetup();

			if (testSet.Tests.Count == 0)
				return;

			string infoMessage = $"[{setIx}/{setCount}] {testSet.DisplayName()}";
			Terminal.Info(infoMessage);

			// execute the tests
			foreach (Test test in testSet.Tests)
			{
				// run the test
				Utility.ProcessResult processResult = Utility.RunProcess(test.ParentSet.ProgramPath, test.Arguments, test.ParentSet.DirectoryPath, test.Timeout);

				// set the result
				test.Result = new TestResult()
				{
					ExitCode = processResult.ExitCode,
					StandardOutput = processResult.StandardOut,
					StandardError = processResult.StandardError,
				};

				if (processResult.TimeoutTriggered)
				{
					test.Result.Info.Add($"Timeout after {test.Timeout} ms");
					test.Status = Test.TestStatus.Failed;
				}

				// check the result
				test.CheckResults();
				testSet.Result.Add(test);
			}

			Terminal.AppendRunResult(infoMessage, testSet.Result.FailedTests.Count == 0);
		}



		// helpers
		static void RunTestSets(TestSet testSet)
		{
			TestSetResult result = new();

			int setIx = 0;
			List<TestSet> allSets = testSet.RecursiveTestSets();
			int setCount = testSet.RecursiveTestSetCount();
			foreach (TestSet set in allSets)
			{
				// start the test set
				RunTestsForSet(set, setIx++, setCount);
			}
		}



		static bool LogSummary(TestSetResult result)
		{
			// no tests ran
			if (result.FailedTests.Count + result.SucceededTests.Count == 0)
			{
				Terminal.Info("No tests ran.");
				return false;
			}

			int failedCount = result.FailedTests.Count;

			// no failed tests
			if (failedCount == 0)
			{
				// all tests 
				Terminal.Info("All tests ran successful");
				return false;
			}

			// some failed tests
			Terminal.Info($"{failedCount} tests failed:");
			foreach (Test test in result.FailedTests)
			{
				// log failed test
				Terminal.Verbose($"{Indent}{test.DisplayPath}");

				if (test.Result != null && Terminal.LogLevel >= Terminal.LogLevels.Verbose)
				{
					if (test.Result.ExitCode != 0)
						Terminal.Verbose($"{Indent2}Exitcode: {test.Result.ExitCode}");
					foreach (string info in test.Result.Info)
						Terminal.Verbose($"{Indent2}{info}");
					foreach (var (outFile, refFile) in test.Result.RefErrors)
						Terminal.Verbose($"{Indent2}'{Utility.SanitizePath(outFile)}' does not match '{Utility.SanitizePath(refFile)}'");
				}
			}


			// review differences?
			Terminal.EmptyLine();
			char answer = Terminal.Ask($"Review test difference{(failedCount == 1 ? "" : "s")}? (y/N): ");
			return answer == 'y';
		}



		static bool AcceptTestResult(ref bool acceptAll)
		{
			while (true)
			{
				char answer = Terminal.Ask("Accept test difference? (yNaqh) ");

				// use default option on enter
				if (answer == 'n' || answer == '\n')
					return false;

				// quit on escape
				if (answer == 'q' || answer == 27)
				{
					System.Environment.Exit(1);
					return false;
				}

				// accept all
				if (answer == 'a')
					acceptAll = true;

				// accept
				if (answer == 'a' || answer == 'y')
					return true;

				// check for validity (or help request)
				Terminal.Info("Valid options are:");
				Terminal.Info($"{Indent}y: yes, accept this difference");
				Terminal.Info($"{Indent}N: no, don't accept this difference (default)");
				Terminal.Info($"{Indent}a: accept this difference, and all subsequent differnces");
				Terminal.Info($"{Indent}q: quit");
			}
		}



		static void ReviewResults(TestSet testSet, string difftool)
		{
			if (string.IsNullOrWhiteSpace(difftool))
			{
				Terminal.Warning("No diff tool configured, cannot review results.");
				return;
			}

			bool acceptAll = false;
			foreach (Test test in testSet.Result.FailedTests)
			{
				foreach (var (outFile, refFile) in test.Result.RefErrors)
				{
					bool acceptResult = false;
					if (!acceptAll)
					{
						Terminal.Info(test.DisplayName);
						Utility.RunProcess(difftool, $"{outFile} {refFile}");
						acceptResult = AcceptTestResult(ref acceptAll);
					}

					if (acceptAll || acceptResult)
					{
						Terminal.Info($"Accepting test result for {test.DisplayPath}...");
						File.Copy(outFile, refFile, true);
					}
				}
			}
		}



		static int Main(string[] args)
		{
			// parse the config file
			ProgramConfig config = new();
			config.Parse("patience.yml");
			Terminal.LogLevel = config.LogLevel;

			// TODO: parse command line
			// ...

			// last arg is the root test directory
			Utility.RootPath = args.Last();

			// parse test configs
			TestSet? testSet = TestConfig.Parse(Utility.RootPath);
			if (testSet == null)
			{
				Terminal.Error($"Could not find any tests for {args.Last()}");
				Terminal.WaitForInput();
				return -1;
			}

			// filter tests
			testSet.SkipTests(config.SkipTests);

			// run tests
			Terminal.Info($"Running tests for '{Utility.PrintPath(Path.GetFullPath(Utility.RootPath))}'");
			RunTestSets(testSet);
			Terminal.EmptyLine();

			// log the summary
			TestSetResult combinedResult = TestSetResult.GatherAll(testSet);
			bool reviewResults = LogSummary(combinedResult);

			// compare differences
			if (reviewResults)
				ReviewResults(testSet, config.DiffTool);

			// write rapports
			new TextRapport().WriteRapport(testSet);

			// wait for user input before closing
			Terminal.EmptyLine();
			Terminal.Info("Done.");
			Terminal.WaitForInput();
			return 0;
		}
	}
}
