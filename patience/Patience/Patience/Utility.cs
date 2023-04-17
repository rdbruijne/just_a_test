using Patience.Loggers;
using Patience.Output;
using System.Diagnostics;
using YamlDotNet.Serialization;
using static Patience.Program;
using static Patience.Tests.TestConfig;

namespace Patience
{
    public static class Utility
	{
		// path helpers
		public static string RootPath = "";

		public static string PrintPath(string path) => path.Replace('\\', '/');
		public static string RelPath(string path) => Path.GetRelativePath(RootPath, path);
		public static string SanitizePath(string path) => PrintPath(RelPath(path));



		// Run a process
		public class ProcessResult
		{
			public readonly bool TimeoutTriggered = false; // True if the process was killed due to a timeout expiring;
			public readonly int ExitCode = 0; // The value specified when the application terminated.
			public readonly List<string> StandardOut = new(); // The application's standard output.
			public readonly List<string> StandardError = new(); // The application's standard error.
			public readonly DateTime StartTime; // The time that the process was started.
			public readonly DateTime ExitTime; // The time that the process exited.

			public TimeSpan Duration => ExitTime - StartTime;

			public ProcessResult(Process process, bool timeoutTriggered)
			{
				ExitCode = process.ExitCode;
				TimeoutTriggered = timeoutTriggered;

				// program output
				while (!process.StandardOutput.EndOfStream)
					StandardOut.Add(process.StandardOutput.ReadLine() ?? "");

				while (!process.StandardError.EndOfStream)
					StandardError.Add(process.StandardError.ReadLine() ?? "");

				// additional info
				StartTime = process.StartTime;
				ExitTime = process.ExitTime;
			}
		}



		public static ProcessResult RunProcess(string path, string arguments, string? workdir = null, int timeout = -1)
		{
			// set up the process info
			ProcessStartInfo startInfo = new()
			{
				WindowStyle = ProcessWindowStyle.Hidden,
				FileName = path,
				Arguments = arguments,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				WorkingDirectory = workdir ?? Environment.CurrentDirectory
			};

			// start the process
			bool timeoutTriggered = false;
			using Process process = new() { StartInfo = startInfo };
			try
			{
				process.Start();
				if (!process.WaitForExit(timeout))
				{
					timeoutTriggered = true;
					process.Kill(true);
				}
			}
			catch (Exception e)
			{
				Terminal.Error($"Failure runing process '{path} {arguments}': {e.Message}");
			}

			// return the result
			return new ProcessResult(process, timeoutTriggered);
		}



		// yaml
		public static bool TryParseYamlFile<T>(string filepath, ref T result)
		{
			try
			{
				// yaml reader
				string yaml = File.ReadAllText(filepath);
				DeserializerBuilder deserializerBuilder = new();
				IDeserializer deserializer = deserializerBuilder.Build();
				result = deserializer.Deserialize<T>(yaml);
				return true;
			}
			catch (YamlDotNet.Core.YamlException e)
			{
				Terminal.Error($"YAML parse error for '{filepath}' ({e.Start}): {e.Message}");
			}
			catch (Exception e)
			{
				Terminal.Error($"Error parsing '{filepath}': {e.Message}");
			}
			return false;
		}
	}
}
